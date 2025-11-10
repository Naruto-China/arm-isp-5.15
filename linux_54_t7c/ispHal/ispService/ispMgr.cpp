/*
 * Copyright (c) 2025 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */
#define LOG_TAG "IspMgr"

#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL | ATRACE_TAG_ALWAYS)
#include <dlfcn.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include "ispMgr.h"

// copy from driver; aml_isp/hw/ folder. please keep the same.
//#include "aml_isp_cfg.h"
static int getInterface() {
    auto ispIF = &IspMgr::mIspIF;
    if (ispIF->lib) {
        ERR("alg lib already open");
        return 0;
    }
    auto lib = ::dlopen("libispaml.so", RTLD_NOW);
    if (!lib) {
        char const* err_str = ::dlerror();
        ERR("dlopen: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    ispIF->alg2User = (isp_alg2user)::dlsym(lib, "aisp_alg2user");
    if (!ispIF->alg2User) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIF->alg2Kernel = (isp_alg2kernel)::dlsym(lib, "aisp_alg2kernel");
    if (!ispIF->alg2Kernel) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIF->algEnable = (isp_enable)::dlsym(lib, "aisp_enable");
    if (!ispIF->algEnable) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIF->algDisable = (isp_disable)::dlsym(lib, "aisp_disable");
    if (!ispIF->algDisable) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIF->algFwInterface  = (isp_fw_interface)::dlsym(lib, "aisp_fw_interface");
    if (!ispIF->algFwInterface) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIF->lib = lib;
    ERR("getInterface success");
    return 0;
}

struct ispIF IspMgr::mIspIF;
static int ret = getInterface();

IspMgr::IspMgr(int id) {
    memset(&mCalibInfo, 0, sizeof(aisp_calib_info_t));
    memset(&mPstAlgCtx, 0, sizeof(AML_ALG_CTX_S));
    mFlushFd[0] = -1;
    mFlushFd[1] = -1;
    mId = id;
    mStart = false;
    mNeedStopispThread = false;
    mWdrEnable = false;
}

IspMgr::~IspMgr() {
    close(mFlushFd[0]);
    close(mFlushFd[1]);
    mFlushFd[0] = -1;
    mFlushFd[1] = -1;
    ERR("~IspMgr");
}

int IspMgr::configure(struct media_stream *stream, int wdr, aisp_calib_info_t *otp, int fps) {
    ERR("configure +");
    int rc;
    std::unique_lock<std::mutex> lk(mLock);
    mMediaStream = stream;
    mPollingDevices.clear();

    if (mFlushFd[1] != -1 || mFlushFd[0] != -1) {
        close(mFlushFd[0]);
        close(mFlushFd[1]);
        mFlushFd[0] = -1;
        mFlushFd[1] = -1;
    }

    rc = pipe(mFlushFd);
    if (rc < 0) {
        ERR("Failed to create Flush pipe: %s", strerror(errno));
        return -1;
    }

    /**
     * make the reading end of the pipe non blocking.
     * This helps during flush to read any information left there without
     * blocking
     */
    rc = fcntl(mFlushFd[0], F_SETFL, O_NONBLOCK);
    if (rc < 0) {
        ERR("Fail to set flush pipe flag: %s", strerror(errno));
        return -1;
    }

    mPollingDevices.push_back(mMediaStream->video_param);
    mPollingDevices.push_back(mMediaStream->video_stats);

    stream_configuration_t           stream_config;
    stream_config.format.width     = kIspStatsWidth;
    stream_config.format.height    = kIspStatsHeight;
    stream_config.format.nplanes   = 1;

    rc = setDataFormat(mMediaStream, &stream_config);
    if (rc < 0) {
        ERR("[stats] Failed to set format");
        return -1;
    }
    stream_config.format.width     = kIspParamsWidth;
    stream_config.format.height    = kIspParamsHeight;
    stream_config.format.nplanes   = 1;

    rc = setConfigFormat(mMediaStream, &stream_config);
    if (rc < 0) {
        ERR("[params] Failed to set format");
        return -1;
    }

    mSensorConfig = matchSensorConfigByStream(mMediaStream);
    if (mSensorConfig == nullptr) {
        ERR("Failed to matchSensorConfig");
        return -1;
    }
    if (wdr == WDR_MODE_2To1_FRAME) {
        cmos_set_sensor_entity(mSensorConfig, mMediaStream->sensor_ent, 1);
        mWdrEnable = true;
    } else
        cmos_set_sensor_entity(mSensorConfig, mMediaStream->sensor_ent, 0);
    cmos_sensor_control_cb(mSensorConfig, &mPstAlgCtx.stSnsExp);
    cmos_get_sensor_calibration(mSensorConfig, mMediaStream->sensor_ent, &mCalibInfo);
    return rc;
}

int IspMgr::start() {
    ERR("start +");
    int rc;
    std::unique_lock<std::mutex> lk(mLock);
    mStart = true;

    memset (&mISParams.rb, 0, sizeof (struct v4l2_requestbuffers));
    mISParams.rb.count  = kIspParamsNbBuffers;
    mISParams.rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mISParams.rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(mMediaStream->video_param, &mISParams.rb);
    if (rc < 0) {
        ERR("[params] Failed to req_bufs");
        return -1;
    }

    /* map buffers */
    for (int i = 0; i < kIspParamsNbBuffers; i++) {
        struct v4l2_buffer v4l2_buf;
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_query_buf(mMediaStream->video_param, &v4l2_buf);
        if (rc < 0) {
            ERR("[params] error: query buffer %d", rc);
            return -1;
        }

        mISParams.mem[i].size = v4l2_buf.length;
        ERR("[params] type video capture. length: %u offset: %u", v4l2_buf.length, v4l2_buf.m.offset);
        mISParams.mem[i].addr = mmap (0, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
            mMediaStream->video_param->fd, v4l2_buf.m.offset);
        ERR("[params] Buffer[%d] mapped at address 0x%p length: %u offset: %u", i, mISParams.mem[i].addr, v4l2_buf.length, v4l2_buf.m.offset);
        if (mISParams.mem[i].addr == MAP_FAILED) {
            ERR("[params] error: mmap buffers");
            mISParams.mem[i].addr = nullptr;
            return -1;
        }
    }

    char alg_init[kIspParamsHeight * kIspParamsWidth];
    memset(alg_init, 0, sizeof(alg_init));
    (IspMgr::mIspIF.algEnable)(mId, &mPstAlgCtx, &mCalibInfo);

    for (int i = 0; i < kIspParamsNbBuffers; i++) {
        (IspMgr::mIspIF.alg2User)(mId, alg_init);
        (IspMgr::mIspIF.alg2Kernel)(mId, mISParams.mem[i].addr);

        /* queue buffers */
        ERR("[params] begin to Queue buf.");
        struct v4l2_buffer v4l2_buf;
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_q_buf(mMediaStream->video_param, &v4l2_buf);
        if (rc < 0) {
            ERR("[params] error: queue buffers, rc:%d i:%d", rc, i);
        }
    }
    rc = v4l2_video_stream_on(mMediaStream->video_param, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[params] error: streamon");
        return 0;
    }

    memset (&mISPStats.rb, 0, sizeof (struct v4l2_requestbuffers));
    mISPStats.rb.count  = kIspStatsNbBuffers;
    mISPStats.rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mISPStats.rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(mMediaStream->video_stats, &mISPStats.rb);
    if (rc < 0) {
        ERR("[stats] Failed to req_bufs");
        return -1;
    }

    /* map buffers */
    for (int i = 0; i < kIspStatsNbBuffers; i++) {
        struct v4l2_buffer v4l2_buf;
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_query_buf(mMediaStream->video_stats, &v4l2_buf);
        if (rc < 0) {
            ERR("[stats] error: query buffer %d", rc);
            return -1;
        }

        mISPStats.mem[i].size = v4l2_buf.length;
        ERR("[stats] video capture. length: %u offset: %u", v4l2_buf.length, v4l2_buf.m.offset);
        mISPStats.mem[i].addr = mmap (0, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
            mMediaStream->video_stats->fd, v4l2_buf.m.offset);
        ERR("[stats] Buffer[%d] mapped at address 0x%p length: %u offset: %u", i, mISPStats.mem[i].addr, v4l2_buf.length, v4l2_buf.m.offset);
        if (mISPStats.mem[i].addr == MAP_FAILED) {
            ERR("[stats] error: mmap buffers");
            mISPStats.mem[i].addr = nullptr;
            return -1;
        }
    }

    for (int i = 0; i < kIspStatsNbBuffers; i++) {
        /* queue buffers */
        ERR("begin to Queue buf.");
        struct v4l2_buffer v4l2_buf;
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_q_buf( mMediaStream->video_stats, &v4l2_buf);
        if (rc < 0) {
            ERR("[stats] error: queue buffers, rc:%d i:%d", rc, i);
        }
    }

    rc = v4l2_video_stream_on(mMediaStream->video_stats, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[stats] error: streamon");
        return 0;
    }

    //setMaxfps(30);

    ERR("Video stream is on");
    mNeedStopispThread = false;
    pthread_create(&mtid, NULL, &ispThread, this);
    ERR("start -");
    return rc;
}

int IspMgr::stop() {
    ERR("stop +");
    int rc;
    {
        std::unique_lock<std::mutex> lk(mLock);
        if (mStart == false) {
            ERR("IspMgr not working");
            return 0;
        } else {
            if (mFlushFd[1] != -1) {
                char buf = 0xf;  // random value to write to flush fd.
                unsigned int size = write(mFlushFd[1], &buf, sizeof(char));
                if (size != sizeof(char))
                    ERR("Flush write not completed");
            }
        }
    }
    mNeedStopispThread = true;
    pthread_join(mtid, NULL);
    std::unique_lock<std::mutex> lk(mLock);
    {
        char readbuf;
        if (mFlushFd[0] != -1) {
            unsigned int size = read(mFlushFd[0], (void*) &readbuf, sizeof(char));
            if (size != sizeof(char))
                ERR("Flush read not completed.");
        }
    }

    /* stream off */
    rc = v4l2_video_stream_off(mMediaStream->video_stats, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[stats] error: streamoff");
        return 0;
    }
    rc = v4l2_video_stream_off(mMediaStream->video_param, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[params] error: streamoff");
        return 0;
    }

    (IspMgr::mIspIF.algDisable)(mId);

    memset (&mISPStats.rb, 0, sizeof (struct v4l2_requestbuffers));
    mISPStats.rb.count  = 0;
    mISPStats.rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mISPStats.rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(mMediaStream->video_stats, &mISPStats.rb);
    if (rc < 0) {
        ERR("[stats] error: request buffer.");
    }
    /* unmap buffers */
    for (int i = 0; i < kIspStatsNbBuffers; i++) {
        if (mISPStats.mem[i].addr != nullptr)
            munmap (mISPStats.mem[i].addr, mISPStats.mem[i].size);
        if (mISPStats.mem[i].dma_fd >= 0)
            close(mISPStats.mem[i].dma_fd);
    }

    memset (&mISParams.rb, 0, sizeof (struct v4l2_requestbuffers));
    mISParams.rb.count  = 0;
    mISParams.rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    mISParams.rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(mMediaStream->video_param, &mISParams.rb);
    if (rc < 0) {
        ERR("[params] Failed to req_bufs");
        return -1;
    }
    /* unmap buffers */
    for (int i = 0; i < kIspParamsNbBuffers; i++) {
        if (mISParams.mem[i].addr != nullptr)
            munmap (mISParams.mem[i].addr, mISParams.mem[i].size);
        if (mISParams.mem[i].dma_fd >= 0)
            close(mISParams.mem[i].dma_fd);
    }
    {
        struct v4l2_ext_control gain;
        gain.id = V4L2_CID_GAIN;
        gain.value = 0xFFFF;
        ERR("update again  = %d", gain.value);
        v4l2_subdev_set_ctrls(mMediaStream->sensor_ent, &gain, 1);

        struct v4l2_ext_control expo;
        expo.id = V4L2_CID_EXPOSURE;
        expo.value = 0xFFFF;
        ERR("update exposure  = %d", expo.value);
        v4l2_subdev_set_ctrls(mMediaStream->sensor_ent, &expo, 1);
    }
    ERR("stop -");
    return rc;
}


int IspMgr::set_exposure_time(int shuttime_value) {
  aisp_api_type_t param;
  isp_exposure_attr_s data;
  aisp_api_type_t *api_type = &param;
  isp_exposure_attr_s *attr = &data;
  api_type->u8Direction = AML_CMD_GET;
  api_type->u8CmdType = 0; // not used
  api_type->u8CmdId = AML_MBI_ISP_ExposureAttr;
  api_type->u32Value = 0; // not used
  api_type->pData = (uint32_t *)&data;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);

  attr->stManual.enExpTimeOpType = OP_TYPE_MANUAL;
  attr->stManual.u32ExpTime = shuttime_value;
  api_type->u8Direction = AML_CMD_SET;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);
  return 0;
}

int IspMgr::set_awb(int awb) {
  aisp_api_type_t param;
  isp_wb_attr_s data;
  aisp_api_type_t *api_type = &param;
  isp_wb_attr_s *attr = &data;
  api_type->u8Direction = AML_CMD_GET;
  api_type->u8CmdType = 0; // not used
  api_type->u8CmdId = AML_MBI_ISP_WBAttr;
  api_type->u32Value = 0; // not used
  api_type->pData = (uint32_t *)&data;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);

  if (awb < 0)
    attr->bByPass = mbp_false;
  else {
      attr->bByPass = mbp_true;
      attr->stAuto.enManMode = ISP_AWB_MODE_COLOR_TEM;
      attr->stManual.u32ManualTemperature = awb;
  }
  api_type->u8Direction = AML_CMD_SET;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);
  return 0;
}

int IspMgr::set_csc(int brightness, int contrast) {
  aisp_api_type_t param;
  aml_isp_csc_attr data;
  aisp_api_type_t *api_type = &param;
  aml_isp_csc_attr *attr = &data;
  api_type->u8Direction = AML_CMD_GET;
  api_type->u8CmdType = 0; // not used
  api_type->u8CmdId = AML_MBI_ISP_CSCAttr;
  api_type->u32Value = 0; // not used
  api_type->pData = (uint32_t *)&data;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);

  if (brightness > 0 || contrast > 0) {
      attr->csc_enable = 1;
      if (brightness > 0)
        attr->glb_brightness = brightness;
      if (contrast > 0)
        attr->glb_contrast  = contrast;
  }
  api_type->u8Direction = AML_CMD_SET;
  (IspMgr::mIspIF.algFwInterface)(mId, api_type);
  return 0;
}


#if 0
int IspMgr::getAWBInfo(void* data)
{
    Mutex::Autolock _l(mLock);
    ERR("getAWBInfo mStart %d", mStart);
    if (data == NULL || mStart == false)
    {
        ERR("IspMgr not working or invalid data");
        return -1;
    }
    aisp_api_type_t param;
    memset(&param, 0, sizeof(aisp_api_type_t));
    aml_isp_wb_info_attr _data;
    memset(&_data, 0, sizeof(aml_isp_wb_info_attr));
    aisp_api_type_t *api_type = &param;
    api_type->u8Direction = AML_CMD_GET;
    api_type->u8CmdType = 0;
    api_type->u8CmdId = AML_MBI_ISP_QueryWBinfo;
    api_type->u32Value = 0;
    api_type->pData = (uint32_t *)&_data;
    (IspMgr::mIspIF.algFwInterface)(mId, api_type);

    ERR("getAWBInfo data %p", data);
    memcpy((uint8_t *)data, (uint8_t *)&_data, sizeof(aml_isp_wb_info_attr));
    return 0;
}

int IspMgr::getAEInfo(void* data)
{
    Mutex::Autolock _l(mLock);
    ERR("getAEInfo mStart %d", mStart);
    if (data == NULL || mStart == false)
    {
        ERR("IspMgr not working or invalid data");
        return -1;
    }
    aisp_api_type_t param;
    memset(&param, 0, sizeof(aisp_api_type_t));
    aml_isp_exp_info_attr _data;
    memset(&_data, 0, sizeof(aml_isp_exp_info_attr));
    aisp_api_type_t *api_type = &param;
    api_type->u8Direction = AML_CMD_GET;
    api_type->u8CmdType = 0;
    api_type->u8CmdId = AML_MBI_ISP_QueryEXPinfo;
    api_type->u32Value = 0;
    api_type->pData = (uint32_t *)&_data;
    (IspMgr::mIspIF.algFwInterface)(mId, api_type);

    ERR("getAEInfo data %p", data);
    memcpy((uint8_t *)data, (uint8_t *)&_data, sizeof(aml_isp_exp_info_attr));
    return 0;
}

#endif
int IspMgr::setMaxfps(int fps)
{
    std::unique_lock<std::mutex> lk(mLock);
    ERR("setMaxfps fps %d", fps);
    if (mStart == false)
    {
        ERR("IspMgr not working");
        return -1;
    }
    //set fps
    aisp_api_type_t param;
    int ret = 0;
    memset(&param, 0, sizeof(aisp_api_type_t));
    aisp_api_type_t *api_type = &param;
    api_type->u8Direction = AML_CMD_SET;
    api_type->u8CmdType = 0;
    api_type->u8CmdId = AML_MBI_ISP_CfgAlgFps;
    api_type->u32Value = 0;
    api_type->pRetValue = (uint32_t *)&ret;
    api_type->pData = (uint32_t *)&fps;
    (IspMgr::mIspIF.algFwInterface)(mId, api_type);
    return 0;
}

int IspMgr::pollDevices(const std::vector<struct media_entity *> &devices,
                                std::vector<struct media_entity *> &activeDevices,
                                std::vector<struct media_entity *> &inactiveDevices,
                                int timeOut, int flush_Fd, int events) {
    //ERR("pollDevices +");
    int numFds = devices.size();
    int totalNumFds = (flush_Fd != -1) ? numFds + 1 : numFds; //adding one more fd if flush given.
    struct pollfd pollFds[totalNumFds];
    int ret = 0;

    for (int i = 0; i < numFds; i++) {
        pollFds[i].fd = devices[i]->fd;
        pollFds[i].events = events | POLLERR; // we always poll for errors, asked or not
        pollFds[i].revents = 0;
    }

    if (flush_Fd != -1) {
        pollFds[numFds].fd = flush_Fd;
        pollFds[numFds].events = POLLPRI | POLLIN;
        pollFds[numFds].revents = 0;
    }

    ret = poll(pollFds, totalNumFds, timeOut);
    if (ret <= 0) {
        for (uint32_t i = 0; i < devices.size(); i++) {
            ERR("Device %s poll failed (%s)", devices[i]->info.name,
                                              (ret == 0) ? "timeout" : "error");
            if (pollFds[i].revents & POLLERR) {
                ERR("%s: device %s received POLLERR", __FUNCTION__, devices[i]->info.name);
            }
        }
        return ret;
    }

    activeDevices.clear();
    inactiveDevices.clear();

    //check first the flush
    if (flush_Fd != -1) {
        if ((pollFds[numFds].revents & POLLIN) || (pollFds[numFds].revents & POLLPRI)) {
            ERR("%s: Poll returning from flush", __FUNCTION__);
            return ret;
        }
    }

    // check other active devices.
    for (int i = 0; i < numFds; i++) {
        if (pollFds[i].revents & POLLERR) {
            ERR("%s: received POLLERR", __FUNCTION__);
            return -1;
        }
        // return nodes that have data available
        if (pollFds[i].revents & events) {
            activeDevices.push_back(devices[i]);
        } else
            inactiveDevices.push_back(devices[i]);
    }
    return ret;
}

bool IspMgr::threadLoop(void * _ispmgr) {
    int rc;
    //ERR("threadLoop+");
    IspMgr * ispmgr = static_cast<IspMgr *>(_ispmgr);
    auto pollingDevices = ispmgr->mPollingDevices;
    do {
        rc = IspMgr::pollDevices(pollingDevices, ispmgr->mActiveDevices,
                                 ispmgr->mInactiveDevices, kSyncWaitTimeout, ispmgr->mFlushFd[0]);
        if (ispmgr->mInactiveDevices.size() > 0) {
            pollingDevices = ispmgr->mInactiveDevices;
            //ERR("not all device is ready, continue polling");
            for (int i = 0; i < ispmgr->mInactiveDevices.size(); ++i)
                //ERR("InactiveDevices %d: %s", i, ispmgr->mInactiveDevices[i]->info.name);
            continue;
        } else if (ispmgr->mActiveDevices.size() > 0 && ispmgr->mInactiveDevices.size() == 0) {
            //ERR("all device is ready");
        } else {
            ERR("return from flush or error");
            return false;
        }

        struct v4l2_buffer v4l2_buf_stats;
        // dqbuf from video node
        memset (&v4l2_buf_stats, 0, sizeof (struct v4l2_buffer));
        v4l2_buf_stats.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf_stats.memory = V4L2_MEMORY_MMAP;
        rc = v4l2_video_dq_buf(ispmgr->mMediaStream->video_stats, &v4l2_buf_stats);
        if (rc < 0) {
            ERR ("[stats] error: dequeue buffer");
            continue;
        }

        struct v4l2_buffer v4l2_buf_param;
        memset (&v4l2_buf_param, 0, sizeof (struct v4l2_buffer));
        v4l2_buf_param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf_param.memory = V4L2_MEMORY_MMAP;
        rc = v4l2_video_dq_buf(ispmgr->mMediaStream->video_param, &v4l2_buf_param);
        if (rc < 0) {
            ERR ("[params] error: dequeue buffer");
            continue;
        }

        (IspMgr::mIspIF.alg2User)(ispmgr->mId, ispmgr->mISPStats.mem[v4l2_buf_stats.index].addr);
        (IspMgr::mIspIF.alg2Kernel)(ispmgr->mId, ispmgr->mISParams.mem[v4l2_buf_param.index].addr);

        rc = v4l2_video_q_buf(ispmgr->mMediaStream->video_stats,  &v4l2_buf_stats);
        if (rc < 0) {
            ERR ("[stats] error: queue buffer");
            break;
        }
        rc = v4l2_video_q_buf(ispmgr->mMediaStream->video_param,  &v4l2_buf_param);
        if (rc < 0) {
            ERR ("[params] error: queue buffer");
            break;
        }
        
        char value[1024*3];
        int br, constrast;
        int user_set_value;

        if (!(ispmgr->mWdrEnable)) {
            memset(value, 0 ,sizeof(value));
            property_get_str(USER_SET_EXP_TIME, value, "999999999");
            user_set_value = atoi(value);
            if (user_set_value <= 0 || user_set_value >= 999999999) {
                ispmgr->set_exposure_time(33000);
            } else {
                ispmgr->set_exposure_time(user_set_value);
            }
        }
        memset(value, 0 ,sizeof(value));
        property_get_str(USER_SET_AWB, value, "999999999");
        user_set_value = atoi(value);
        if (user_set_value <= 0 || user_set_value >= 999999999) {
            ispmgr->set_awb(-1);
        } else {
            ispmgr->set_awb(user_set_value);
        }
        memset(value, 0 ,sizeof(value));
        property_get_str(USER_SET_BRIGHTNESS, value, "-1");
        br = user_set_value = atoi(value);
        memset(value, 0 ,sizeof(value));
        property_get_str(USER_SET_CONTRAST, value, "-1");
        constrast = user_set_value = atoi(value);
        ispmgr->set_csc(br, constrast);
        break;
    } while(1);
    //ERR("threadLoop-");
    return true;
}

void* IspMgr::ispThread(void * _ispmgr) {
   bool ret;
   IspMgr * ispmgr = static_cast<IspMgr *>(_ispmgr);
   while (false == ispmgr->mNeedStopispThread) {
       ret = threadLoop(_ispmgr);
       if (ret == false)
        return NULL;
   }
   return ((void *)0);
}


