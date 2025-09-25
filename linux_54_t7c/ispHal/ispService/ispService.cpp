/*
 * Copyright (c) 2018 Amlogic, Inc. All rights reserved.
 *
 * This source code is subject to the terms and conditions defined in the
 * file 'LICENSE' which is part of this source code package.
 *
 * Description:
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>


#include "logs.h"

#include "mediactl.h"
#include "v4l2subdev.h"
#include "v4l2videodev.h"
#include "mediaApi.h"

#include "staticPipe.h"
#include "ispMgr.h"


//#define WDR_ENABLE
#define DUAL_CAMERA

#define NB_BUFFER                4
#define NB_BUFFER_PARAM          1

static volatile bool running = true;
void sig_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        running = false;
    }
}

struct ispIF ispIf;

struct isp_info {
    aisp_calib_info_t calib;
    AML_ALG_CTX_S pstAlgCtx;
};

/* config parameters */
struct config_param {
    /* v4l2 variables */
    struct media_stream         v4l2_media_stream;
    void                        *v4l2_mem_param[NB_BUFFER_PARAM];
    void                        *v4l2_mem[NB_BUFFER];
    int                          param_buf_length;
    int                          stats_buf_length;
    struct sensorConfig          *sensorCfg;

    char                        *mediadevname;

    uint32_t                    fmt_code;
    uint32_t                    wdr_mode;
    uint32_t                    width;
    uint32_t                    height;

    isp_info                    info;
    IspMgr*                     ispmgr;
    int                         media_fd;
    int                         fps;
};

static int getInterface() {
    auto lib = ::dlopen("libispaml.so", RTLD_NOW);
    if (!lib) {
        char const* err_str = ::dlerror();
        ERR("dlopen: error:%s", (err_str ? err_str : "unknown"));
        dlclose(lib);
        return -1;
    }
    ispIf.alg2User = (isp_alg2user)::dlsym(lib, "aisp_alg2user");
    if (!ispIf.alg2User) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    ispIf.alg2Kernel = (isp_alg2kernel)::dlsym(lib, "aisp_alg2kernel");
    if (!ispIf.alg2Kernel) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    ispIf.algEnable = (isp_enable)::dlsym(lib, "aisp_enable");
    if (!ispIf.algEnable) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    ispIf.algDisable = (isp_disable)::dlsym(lib, "aisp_disable");
    if (!ispIf.algDisable) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    ispIf.algFwInterface = (isp_fw_interface)::dlsym(lib, "aisp_fw_interface");
    if (!ispIf.algFwInterface) {
        char const* err_str = ::dlerror();
        ERR("dlsym: error:%s", (err_str ? err_str : "unknown"));
        return -1;
    }
    INFO("%s success", __FUNCTION__);
    return 0;
}

int isp_param_init(struct media_stream v4l2_media_stream, struct config_param *tparm)
{
    struct v4l2_requestbuffers  v4l2_rb;
    int rc, i;
    int total_mapped_mem=0;
    struct v4l2_buffer v4l2_buf;
    char alg_init[256*1024];

    stream_configuration_t     stream_config;
    stream_config.format.width = 1024;
    stream_config.format.height = 256;
    stream_config.format.nplanes   = 1;

    rc = setDataFormat(&v4l2_media_stream, &stream_config);
    if (rc < 0) {
        ERR("Failed to set stats format");
        return -1;
    }

    rc = setConfigFormat(&v4l2_media_stream, &stream_config);
    if (rc < 0) {
        ERR("Failed to set param format");
        return -1;
    }

    /* request buffers */
    memset (&v4l2_rb, 0, sizeof (struct v4l2_requestbuffers));
    v4l2_rb.count  = NB_BUFFER;
    v4l2_rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(tparm->v4l2_media_stream.video_stats, &v4l2_rb);
    if (rc < 0) {
        ERR("Error: request buffer.\n");
        return -1;
    }

    memset (&v4l2_rb, 0, sizeof (struct v4l2_requestbuffers));
    v4l2_rb.count  = NB_BUFFER_PARAM;
    v4l2_rb.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_rb.memory = V4L2_MEMORY_MMAP;
    rc = v4l2_video_req_bufs(v4l2_media_stream.video_param, &v4l2_rb);
    if (rc < 0) {
        ERR("Failed to req_bufs");
        return -1;
    }

    /* map stats buffers */
    for (i = 0; i < NB_BUFFER; i++) {
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_query_buf(tparm->v4l2_media_stream.video_stats, &v4l2_buf);
        if (rc < 0) {
            ERR("Error: query buffer %d.\n", rc);
            return -1;
        }

        if (v4l2_buf.type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
            tparm->stats_buf_length = v4l2_buf.length;
            INFO("video capture. length: %u offset: %u\n", v4l2_buf.length, v4l2_buf.m.offset);
            tparm->v4l2_mem[i] = mmap (0, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                tparm->v4l2_media_stream.video_stats->fd, v4l2_buf.m.offset);
            ++total_mapped_mem;
            INFO("Buffer[%d] mapped at address 0x%p total_mapped_mem:%d.\n", i, tparm->v4l2_mem[i], total_mapped_mem);
        }
        if (tparm->v4l2_mem[i] == MAP_FAILED) {
            ERR("Error: mmap buffers.\n");
            return -1;
        }
    }

    /* map buffers */
    for (i = 0; i < NB_BUFFER_PARAM; i++) {
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_query_buf(v4l2_media_stream.video_param, &v4l2_buf);
        if (rc < 0) {
            ERR("Failed to query bufs");
            return -1;
        }

        if (v4l2_buf.type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
            tparm->param_buf_length = v4l2_buf.length;
            INFO("[T#2] type video capture. length: %u offset: %u\n", v4l2_buf.length, v4l2_buf.m.offset);
            tparm->v4l2_mem_param[i] = mmap (0, v4l2_buf.length, PROT_READ | PROT_WRITE, MAP_SHARED,
                v4l2_media_stream.video_param->fd, v4l2_buf.m.offset);
            INFO("[T#2] Buffer[0] mapped at address 0x%p total_mapped_mem:%d.\n", tparm->v4l2_mem_param[i], total_mapped_mem);
        }
        if (tparm->v4l2_mem_param[i] == MAP_FAILED) {
            ERR("[T#2] Error: mmap buffers.\n");
            return -1;
        }
    }

    static int ret = getInterface();
    if (ret == -1) {
        ERR("Failed to getInterface");
        return -1;
    }

    tparm->sensorCfg = matchSensorConfigByStream(&v4l2_media_stream);
    if (tparm->sensorCfg == nullptr) {
        ERR("Failed to matchSensorConfig");
        return -1;
    }

#ifdef WDR_ENABLE
    cmos_set_sensor_entity(tparm->sensorCfg, v4l2_media_stream.sensor_ent, 1);
#else
    cmos_set_sensor_entity(tparm->sensorCfg, v4l2_media_stream.sensor_ent, 0);
#endif
    cmos_sensor_control_cb(tparm->sensorCfg, &tparm->info.pstAlgCtx.stSnsExp);
    cmos_get_sensor_calibration(tparm->sensorCfg, v4l2_media_stream.sensor_ent, &tparm->info.calib);

    (ispIf.algEnable)(0, &tparm->info.pstAlgCtx, &tparm->info.calib);
    memset(alg_init, 0, sizeof(alg_init));

    (ispIf.alg2User)(0, alg_init);
    (ispIf.alg2Kernel)(0, tparm->v4l2_mem_param[0]);

    /* queue buffers */
    DBG("[T#0] begin to Queue buf.\n");
    for (i = 0; i < NB_BUFFER; ++i) {
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_q_buf( tparm->v4l2_media_stream.video_stats, &v4l2_buf );
        if (rc < 0) {
            ERR("Error: queue buffers, rc:%d i:%d\n",rc, i);
            return -1;
        }
    }

    for (i = 0; i < NB_BUFFER_PARAM; ++i) {
        memset (&v4l2_buf, 0, sizeof (struct v4l2_buffer));
        v4l2_buf.index   = i;
        v4l2_buf.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buf.memory  = V4L2_MEMORY_MMAP;
        rc = v4l2_video_q_buf( v4l2_media_stream.video_param, &v4l2_buf );
        if (rc < 0) {
            ERR("Error: queue buffers, rc:%d i:%d\n", rc, i);
            return -1;
        }
    }

    rc = v4l2_video_stream_on(tparm->v4l2_media_stream.video_stats, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[T#0] Error: stats streamon.\n");
        return -1;
    }

    rc = v4l2_video_stream_on(v4l2_media_stream.video_param, V4L2_BUF_TYPE_VIDEO_CAPTURE);
    if (rc < 0) {
        ERR("[T#0] Error: param streamon.\n");
        return -1;
    }

    /* stream off */
    rc = v4l2_video_stream_off(tparm->v4l2_media_stream.video_stats, V4L2_BUF_TYPE_VIDEO_CAPTURE);

    /* unmap buffers */
    for (i = 0; i < NB_BUFFER; i++) {
        munmap (tparm->v4l2_mem[i], tparm->stats_buf_length);
    }

    /* stream off */
    rc = v4l2_video_stream_off(tparm->v4l2_media_stream.video_param, V4L2_BUF_TYPE_VIDEO_CAPTURE);

    /* unmap buffers */
    for (i = 0; i < NB_BUFFER_PARAM; i++) {
        munmap (tparm->v4l2_mem_param[i], tparm->param_buf_length);
    }

    DBG("[T#0] Finished alg_param_init.\n");
    return 0;
}

int media_stream_config(media_stream_t * stream, stream_configuration_t *cfg)
{
    int rtn = -1;

    INFO("%s %dx%d ++", __FUNCTION__, cfg->format.width, cfg->format.height);

    rtn = setSdFormat(stream, cfg);
    if (rtn < 0) {
        ERR("Failed to set subdev format");
        return rtn;
    }

    rtn = createLinks(stream);
    if (rtn) {
        ERR( "Failed to create links");
        return rtn;
    }

    INFO("Success to config media stream ");
    return 0;
}

int prepare_media_stream(struct config_param  *tparm)
{
    int rc = 0;
    tparm->v4l2_media_stream.media_dev = media_device_new(tparm->mediadevname);

    rc = mediaStreamInit(&tparm->v4l2_media_stream, tparm->v4l2_media_stream.media_dev);
    if (0 != rc) {
        ERR("The %s device init fail.\n", tparm->mediadevname);
        return -1;
    }
    tparm->media_fd = tparm->v4l2_media_stream.media_dev->fd;
    INFO("The %s device was opened successfully. stream init ok\n", tparm->mediadevname);

    media_set_wdrMode(&tparm->v4l2_media_stream, 0);
    media_set_wdrMode(&tparm->v4l2_media_stream, tparm->wdr_mode);

    if (tparm->width <= 0 || tparm->height <= 0)
        fetchPipeMaxResolution(&tparm->v4l2_media_stream, &tparm->width, &tparm->height);

    ERR("media_stream_config %d %d", tparm->width, tparm->height);

    /* config & set format */
    stream_configuration     stream_config ;
    memset(&stream_config, 0, sizeof(stream_configuration));
    stream_config.format.width =  tparm->width;
    stream_config.format.height = tparm->height;
    stream_config.format.code   = tparm->fmt_code;
    stream_config.format.nplanes   = 1;
    stream_config.fps    = tparm->fps;

    rc = media_stream_config(&tparm->v4l2_media_stream, &stream_config);
    if (rc < 0) {
        ERR("fail config stream\n");
        return -1;
    }
    ERR("fail config stream\n");
    rc = tparm->ispmgr->configure(
        &tparm->v4l2_media_stream, 0, nullptr, 30);
    rc = tparm->ispmgr->start();
    return 0;
}

void usage(char * prog){
    INFO("%s\n", prog);
    INFO("usage:\n");
    INFO(" example   : ./ispService -m /dev/media0 \n");
    INFO("    m : media dev name: /dev/media0 or /dev/media1 \n");
}

int main(int argc, char *argv[])
{
    int rtn = 0;
    char v4l2mediadevname[128] = "/dev/media0";

    int name_bytes = 0;
    struct media_stream         v4l2_media_stream;
    int fps = 30;
    int sensor_W = 0;
    int sensor_H = 0;
    bool off_line = false;

    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);

    if (argc < 1) {
        usage(argv[0]);
        return -1;
    }

    int c;

    while (optind < argc) {
        if ((c = getopt (argc, argv, "m:f:W:H:M:")) != -1) {
            switch (c) {
            case 'm':
                strcpy(v4l2mediadevname, optarg);
                break;
            case 'f':
                fps = atoi(optarg);
                break;
            case 'W':
                sensor_W = atoi(optarg);
                break;
            case 'H':
                sensor_H = atoi(optarg);
                break;
            case 'M':
                off_line = (atoi(optarg) > 0) ;
                break;
            case '?':
                usage(argv[0]);
                exit(1);
            }
        }else{
            MSG("Invalid argument %s\n",argv[optind]);
            usage(argv[0]);
            exit(1);
        }
    }

    IspMgr* ispmgr = new IspMgr(0);

    struct config_param tparam_raw = {
        .mediadevname = v4l2mediadevname,

#if defined (DUAL_CAMERA)
        .fmt_code   = MEDIA_BUS_FMT_SRGGB12_1X12,
        .wdr_mode   = (off_line ? ISP_SDR_DCAM_MODE : WDR_MODE_NONE),
#elif defined (WDR_ENABLE)
        .fmt_code   = MEDIA_BUS_FMT_SBGGR10_1X10,//MEDIA_BUS_FMT_SRGGB12_1X12,//
        .wdr_mode   = WDR_MODE_2To1_LINE,//WDR_MODE_2To1_LINE,
#else
        .fmt_code   = MEDIA_BUS_FMT_SRGGB12_1X12,//
        .wdr_mode   = WDR_MODE_NONE,
#endif
        .ispmgr     = ispmgr,
        .fps        = fps,
    };
    if (sensor_W > 0 && sensor_H > 0) {
        tparam_raw.width = sensor_W;
        tparam_raw.height = sensor_H;
    }
    MSG("user set sensor w %d, h %d", tparam_raw.width, tparam_raw.height);
    rtn = prepare_media_stream(&tparam_raw);
    if (0 != rtn ) {
        ERR("prepare pipeline fail\n");
        return -1;
    }

    while (running) {
        pause();
    }

    if (ispmgr) {
        ispmgr->stop();
        delete ispmgr;
    }

    if ( tparam_raw.v4l2_media_stream.media_dev ) {
        media_device_unref( tparam_raw.v4l2_media_stream.media_dev );
    }
    if (tparam_raw.media_fd > 0) close(tparam_raw.media_fd);

    return 0;
}


