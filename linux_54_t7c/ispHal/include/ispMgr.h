/*
 * Copyright (c) 2021 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#ifndef __ISP_MGR_H__
#define __ISP_MGR_H__

#include <cstdlib>
#include <thread>
#include <vector>
#include <mutex>

#include "mediactl.h"
#include "v4l2subdev.h"
#include "v4l2videodev.h"
#include "mediaApi.h"
#include "logs.h"

#include "sensor_config.h"
#include "lens_config.h"
#include "aml_isp_adapt.h"
#include "aisp_command_api.h"
#include "fileProperty.h"

const size_t  kMaxRetryCount   = 100;
const int64_t kSyncWaitTimeout = 300000000LL; // 300ms

const size_t kIspStatsNbBuffers = 3;
const size_t kIspStatsWidth = 1024;
const size_t kIspStatsHeight = 256;

const size_t kIspParamsNbBuffers = 1;
const size_t kIspParamsWidth = 1024;
const size_t kIspParamsHeight = 256;

typedef void (*isp_alg2user)(uint32_t ctx_id, void *param);
typedef void (*isp_alg2kernel)(uint32_t ctx_id, void *param);
typedef void (*isp_enable)(uint32_t ctx, void *pstAlgCtx, void *calib);
typedef void (*isp_disable)(uint32_t ctx_id);
typedef void (*isp_fw_interface)(uint32_t ctx_id, void *param);

struct ispIF {
    void *lib = nullptr;
    isp_alg2user   alg2User   = nullptr;
    isp_alg2kernel alg2Kernel = nullptr;
    isp_enable     algEnable  = nullptr;
    isp_disable    algDisable = nullptr;
    isp_fw_interface algFwInterface = nullptr;
};

struct bufferInfo {
    void* addr = nullptr;
    int   size = 0;
    int   dma_fd = -1;
};

struct v4l2BufferInfo {
    v4l2BufferInfo() {
        memset(&rb, 0, sizeof(struct v4l2_requestbuffers));
        memset(&format, 0, sizeof(struct v4l2_format));
    }
    struct v4l2_requestbuffers rb;
    struct v4l2_format         format;
    struct bufferInfo          mem[8];
};

class IspMgr {
  public:
    IspMgr(int id);
    ~IspMgr();
  public:
    int configure(struct media_stream *stream, int wdr = 0, aisp_calib_info_t *otp = nullptr, int fps = 30);
    int start();
    int stop();
    int set_exposure_time(int shuttime_value);
    //int getAWBInfo(void* data);
    //int getAEInfo(void* data);
    int setMaxfps(int fps);
    int set_awb(int awb);
    int set_csc(int brightness, int contrast, int saturation);
  public:
    static struct ispIF  mIspIF;
  protected:
    //virtual int     readyToRun();
    //virtual bool         threadLoop();
    static  int     pollDevices(const std::vector<struct media_entity *> &devices,
                             std::vector<struct media_entity *> &activeDevices,
                             std::vector<struct media_entity *> &inactiveDevices,
                             int timeOut, int flush_Fd = -1,
                             int events = POLLPRI | POLLIN | POLLERR);
  private:
    int                                mId;
    std::mutex                              mLock;
    bool                               mStart;
    bool                               mWdrEnable;
    struct media_stream*               mMediaStream  = nullptr;
    struct sensorConfig*               mSensorConfig = nullptr;
    int                                mFlushFd[2];
    std::vector<struct media_entity *> mPollingDevices;
    std::vector<struct media_entity *> mActiveDevices;
    std::vector<struct media_entity *> mInactiveDevices;
    v4l2BufferInfo                     mISPStats;
    v4l2BufferInfo                     mISParams;
    aisp_calib_info_t                  mCalibInfo;
    AML_ALG_CTX_S                      mPstAlgCtx;
    pthread_t                          mtid;
    volatile bool                      mNeedStopispThread;
    static bool threadLoop(void * _ispmgr);
    static void* ispThread(void * _ispmgr);
};
#endif
