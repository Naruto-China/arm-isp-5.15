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

#include "aml_isp_api.h"

#include "imx585_api.h"
#include "logs.h"

typedef struct
{
    int  enWDRMode;
    ALG_SENSOR_DEFAULT_S snsAlgInfo;
    struct media_entity  * sensor_ent;
} ISP_SNS_STATE_S;

static ISP_SNS_STATE_S sensor;

extern int dynamic_wdr_calibrations_init_imx585(aisp_calib_info_t *calib);
extern int dynamic_sdr_calibrations_init_imx585(aisp_calib_info_t *calib);
void cmos_set_sensor_entity_imx585(struct media_entity * sensor_ent, int wdr)
{
    memset(&sensor.snsAlgInfo, 0, sizeof(ALG_SENSOR_DEFAULT_S));
    sensor.sensor_ent = sensor_ent;
    sensor.enWDRMode = wdr;
}

void cmos_get_sensor_calibration_imx585(struct media_entity *sensor_ent, aisp_calib_info_t *calib)
{
    if (sensor.enWDRMode == 1)
        dynamic_wdr_calibrations_init_imx585(calib);
    else
        dynamic_sdr_calibrations_init_imx585(calib);
}

int cmos_get_ae_default_imx585(int ViPipe, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{
    INFO("cmos_get_ae_default\n");

    sensor.snsAlgInfo.active.width = 3840;
    sensor.snsAlgInfo.active.height = 2160;


    if (sensor.enWDRMode == 1) {
	    sensor.snsAlgInfo.fps = 30*256;
	    sensor.snsAlgInfo.sensor_exp_number = 2;
	    sensor.snsAlgInfo.bits = 10;

	    sensor.snsAlgInfo.sensor_gain_number = 1;
	    sensor.snsAlgInfo.total.width = 550; //HMAX register value
	    sensor.snsAlgInfo.total.height = 2250; // VMAX register value

	    sensor.snsAlgInfo.lines_per_second = sensor.snsAlgInfo.total.height * sensor.snsAlgInfo.fps*2 / 256; //lines_per_second：vts * fps，Hardware-related values
	    sensor.snsAlgInfo.pixels_per_line = sensor.snsAlgInfo.total.width; //pixels_per_line： Sensor HTS, using HMAX

        sensor.snsAlgInfo.integration_time_min = 10 <<SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_max = (78 - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_long_max = (sensor.snsAlgInfo.total.height*2 - (78 + 10)) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_limit = (78 - 8)<<SHUTTER_TIME_SHIFT;
    } else {
	    sensor.snsAlgInfo.fps = 60*256;
	    sensor.snsAlgInfo.sensor_exp_number = 1;
	    sensor.snsAlgInfo.bits = 10;

	    sensor.snsAlgInfo.sensor_gain_number = 1;
	    sensor.snsAlgInfo.total.width = 550; //HMAX register value
	    sensor.snsAlgInfo.total.height = 2250; // VMAX register value

	    sensor.snsAlgInfo.lines_per_second = sensor.snsAlgInfo.total.height * sensor.snsAlgInfo.fps / 256; //lines_per_second：vts * fps，Hardware-related values
	    sensor.snsAlgInfo.pixels_per_line = sensor.snsAlgInfo.total.width; //pixels_per_line： Sensor HTS, using HMAX

        sensor.snsAlgInfo.integration_time_min = 2 << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_max = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_long_max = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_limit = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
    }

    sensor.snsAlgInfo.dgain_log2_max = 0;
    sensor.snsAlgInfo.dgain_high_log2_max = 0;
    sensor.snsAlgInfo.dgain_high_accuracy_fmt = 0;
    sensor.snsAlgInfo.dgain_high_accuracy = 1;
    sensor.snsAlgInfo.dgain_accuracy_fmt = 0;
    sensor.snsAlgInfo.dgain_accuracy = 1;
    sensor.snsAlgInfo.again_log2 = 0 << LOG2_GAIN_SHIFT;
    sensor.snsAlgInfo.again_log2_max = (30 / 6) << (LOG2_GAIN_SHIFT); //using for sdr mode
    sensor.snsAlgInfo.again_high_log2_max = (30 / 6) << (LOG2_GAIN_SHIFT); //using for wdr mode
    sensor.snsAlgInfo.again_high_accuracy_fmt = 1;
    sensor.snsAlgInfo.again_high_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    sensor.snsAlgInfo.again_accuracy_fmt = 1;
    sensor.snsAlgInfo.expos_lines = (2250-8)<<(SHUTTER_TIME_SHIFT);
    sensor.snsAlgInfo.again_accuracy = (1<<(LOG2_GAIN_SHIFT))/20;
    sensor.snsAlgInfo.expos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    sensor.snsAlgInfo.sexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    sensor.snsAlgInfo.vsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));
    sensor.snsAlgInfo.vvsexpos_accuracy = (1<<(SHUTTER_TIME_SHIFT));

    sensor.snsAlgInfo.gain_apply_delay = 0;
    sensor.snsAlgInfo.integration_time_apply_delay = 0;

    memcpy(pstAeSnsDft, &sensor.snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));

    return 0;
}

void cmos_again_calc_table_imx585(int ViPipe, uint32_t *pu32AgainLin, uint32_t *pu32AgainDb)
{
    uint32_t again_reg;
    uint32_t u32AgainDb;

    u32AgainDb = *pu32AgainDb;
    again_reg = ((u32AgainDb * 20) >> LOG2_GAIN_SHIFT); //Setting value: Gain [dB] * 10/3

    again_reg = (uint32_t)(again_reg);
    // INFO("set sensor reg value %d", again_reg);
    if (again_reg > 720 / 3) //72dB, 0.3dB step.
        again_reg = 720 / 3;

    if (sensor.snsAlgInfo.u32AGain[0] != again_reg) {
        sensor.snsAlgInfo.u16GainCnt = sensor.snsAlgInfo.gain_apply_delay + 1;
        sensor.snsAlgInfo.u32AGain[0] = again_reg;
    }

}

void cmos_dgain_calc_table_imx585(int ViPipe, uint32_t *pu32DgainLin, uint32_t *pu32DgainDb)
{
    //INFO("cmos_dgain_calc_table: %d, %d\n", *pu32DgainLin, *pu32DgainDb);
}

void cmos_inttime_calc_table_imx585(int ViPipe, uint32_t pu32ExpL, uint32_t pu32ExpS, uint32_t pu32ExpVS, uint32_t pu32ExpVVS)
{
    //INFO("cmos_inttime_calc_table: %d, %d, %d, %d\n", pu32ExpL, pu32ExpS, pu32ExpVS, pu32ExpVVS);
    uint32_t shutter_time_lines = pu32ExpL >> SHUTTER_TIME_SHIFT;
    uint32_t shutter_time_line_each_frame = sensor.snsAlgInfo.total.height;

    uint32_t shutter_time_lines_short = pu32ExpS >> SHUTTER_TIME_SHIFT;

    // INFO("expo: %d\n", shutter_time_lines);
    // INFO("expoL : %d, exposS : %d\n", shutter_time_lines, shutter_time_lines_short);
    if (sensor.enWDRMode == 0) {
        if (shutter_time_lines > shutter_time_line_each_frame)
            shutter_time_lines = shutter_time_line_each_frame;
        shutter_time_lines = shutter_time_line_each_frame - shutter_time_lines;  //Integraltime conversion, calculation SHS1 Value.
        if (shutter_time_lines)
            shutter_time_lines = shutter_time_lines - 1;
        if (shutter_time_lines < 8)
            shutter_time_lines = 8;
    } else {
        // short exposure lines = RHS1 - SHR1; RHS1 is fixed to 78.
        // SHR1 reg value = 78 - exposure lines;
        if (shutter_time_lines_short < 10)
            shutter_time_lines_short = 10;
        shutter_time_lines_short = 78 - shutter_time_lines_short; //rhs1=78
        shutter_time_lines = shutter_time_line_each_frame * 2  - shutter_time_lines;
        // INFO("expoL : %d, exposS : %d\n", shutter_time_lines, shutter_time_lines_short);
    }

    if (sensor.snsAlgInfo.u32Inttime[0][0] != shutter_time_lines || sensor.snsAlgInfo.u32Inttime[1][0] != shutter_time_lines_short) {
        sensor.snsAlgInfo.u16IntTimeCnt = sensor.snsAlgInfo.integration_time_apply_delay + 1;
        sensor.snsAlgInfo.u32Inttime[0][0] = shutter_time_lines;
        sensor.snsAlgInfo.u32Inttime[1][0] = shutter_time_lines_short;
        // INFO("expoL : %d, exposS : %d\n", shutter_time_lines, shutter_time_lines_short);
    }
}

void cmos_fps_set_imx585(int ViPipe, float f32Fps, ALG_SENSOR_DEFAULT_S *pstAeSnsDft)
{  

    struct v4l2_ext_control fpsCtrl;

    fpsCtrl.id = V4L2_CID_AML_ORIG_FPS;
    fpsCtrl.value = (int32_t)(f32Fps / 256);
	if (fpsCtrl.value <= 0) {
        return;
    }

    if (sensor.enWDRMode == 1 ) {
     //min fps limit
    	if (fpsCtrl.value < 15) {
        	return;
        }
    }
    //update vmax relative parameters
    if (sensor.enWDRMode == 1){
        sensor.snsAlgInfo.total.height = ( 2250 * 30 )/fpsCtrl.value;
        sensor.snsAlgInfo.fps = fpsCtrl.value*256;

        sensor.snsAlgInfo.integration_time_max = (78 - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_long_max = (sensor.snsAlgInfo.total.height*2 - (78 + 10)) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_limit = (78 - 8)<<SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.lines_per_second = sensor.snsAlgInfo.total.height * fpsCtrl.value*2;
    } else {
        sensor.snsAlgInfo.total.height = ( 2250 * 60 )/fpsCtrl.value;
        sensor.snsAlgInfo.fps = fpsCtrl.value*256;
        sensor.snsAlgInfo.integration_time_max = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_long_max = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.integration_time_limit = (sensor.snsAlgInfo.total.height - 8) << SHUTTER_TIME_SHIFT;
        sensor.snsAlgInfo.lines_per_second = sensor.snsAlgInfo.total.height * fpsCtrl.value;
    }

    memcpy(pstAeSnsDft, &sensor.snsAlgInfo, sizeof(ALG_SENSOR_DEFAULT_S));

    v4l2_subdev_set_ctrls(sensor.sensor_ent, &fpsCtrl, 1);
}

void cmos_alg_update_imx585(int ViPipe)
{
    uint32_t shutter_time_lines = 0, shutter_time_lines_short = 0;
    uint32_t i = 0;

    if ( sensor.snsAlgInfo.u16GainCnt || sensor.snsAlgInfo.u16IntTimeCnt ) {
        if ( sensor.snsAlgInfo.u16GainCnt ) {
            sensor.snsAlgInfo.u16GainCnt--;
            struct v4l2_ext_control gain;
            gain.id = V4L2_CID_GAIN;
            gain.value = sensor.snsAlgInfo.u32AGain[sensor.snsAlgInfo.gain_apply_delay];
            v4l2_subdev_set_ctrls(sensor.sensor_ent, &gain, 1);
        }

        // -------- Integration Time ----------
        if ( sensor.snsAlgInfo.u16IntTimeCnt ) {
            sensor.snsAlgInfo.u16IntTimeCnt--;
            shutter_time_lines = sensor.snsAlgInfo.u32Inttime[0][sensor.snsAlgInfo.integration_time_apply_delay];
            if (sensor.enWDRMode == 0) {
                struct v4l2_ext_control expo;
                expo.id = V4L2_CID_EXPOSURE;
                expo.value = shutter_time_lines;
                v4l2_subdev_set_ctrls(sensor.sensor_ent, &expo, 1);
            }

            if (sensor.enWDRMode) {
                shutter_time_lines_short = sensor.snsAlgInfo.u32Inttime[1][sensor.snsAlgInfo.integration_time_apply_delay];
                struct v4l2_ext_control expo;
                expo.id = V4L2_CID_EXPOSURE;
                expo.value = (shutter_time_lines_short << 16) | shutter_time_lines;
                v4l2_subdev_set_ctrls(sensor.sensor_ent, &expo, 1);
            }
        }
    }

    for ( i = 3; i > 0; i --) {
        sensor.snsAlgInfo.u32AGain[i] = sensor.snsAlgInfo.u32AGain[i - 1];
        sensor.snsAlgInfo.u32Inttime[0][i] = sensor.snsAlgInfo.u32Inttime[0][i - 1];
        sensor.snsAlgInfo.u32Inttime[1][i] = sensor.snsAlgInfo.u32Inttime[1][i - 1];
    }

}
