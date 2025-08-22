/*
 * Copyright (C) 2019-2024 Amlogic, Inc. All rights reserved.
 *
 * All information contained herein is Amlogic confidential.
 *
 * T_s software is provided to you pursuant to Software License Agreement
 * (SLA) with Amlogic Inc ("Amlogic"). T_s software may be used
 * only in accordance with the terms of t_s agreement.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification is strictly pro_bited without prior written permission from
 * Amlogic.
 *
 * TMBPS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF TMBPS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AML_COMM_ISP_ADAPT_H__
#define __AML_COMM_ISP_ADAPT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define GAMMA_NODE_NUM                  (129)
#define PREGAMMA_NODE_NUM               (129)
#define PREGAMMA_SEG_NUM                (8)

#define WDR_SPT_CHANNEL   4
#define EXPO_MAX_CHANNEL  2

#define LTM_HIST_BLK 79

#define LDCI_SATUR_LUT_NODE 63

#define CCM_MATRIX_SIZE                 (12)
#define CCM_MATRIX_NUM                  (201)
#define CCM_ADJ_SIZE                    (10)

#define DRC_STA_BLK_REG_NUM  96

#define DHZ_NODES_NUM 5

#pragma pack(1)

typedef enum _OPERATION_MODE_E
{
    OPERATION_MODE_AUTO   = 0,
    OPERATION_MODE_MANUAL = 1,
    OPERATION_MODE_BUTT
} OPERATION_MODE_E;

typedef enum _ISP_FMW_STATE_E
{
    ISP_FMW_STATE_RUN = 0,
    ISP_FMW_STATE_FREEZE,
    ISP_FMW_STATE_BUTT
} ISP_FMW_STATE_E;

typedef enum _ISP_OP_TYPE_E
{
    OP_TYPE_AUTO    = 0,
    OP_TYPE_MANUAL  = 1,
    OP_TYPE_LUT     = 2,
    OP_TYPE_MAX
} ISP_OP_TYPE_E;

typedef enum _ISP_AWB_INDOOR_OUTDOOR_STATUS_E
{
    AWB_INDOOR_MODE = 0,
    AWB_OUTDOOR_MODE = 1,
    AWB_INDOOR_OUTDOOR_BUTT
} ISP_AWB_INDOOR_OUTDOOR_STATUS_E;

typedef enum {
    mbp_false = 0,
    mbp_true = 1,
} mbp_bool_e;

typedef enum _ISP_EXPOSURE_MODE_E
{
    ISP_EXP_MODE_AVG    = 0,
    ISP_EXP_MODE_CENT_SPOT  = 1,
    ISP_EXP_MODE_CENT_WET = 2,
    ISP_EXP_MODE_HALF_TOP = 3,
    ISP_EXP_MODE_HALF_BUTT = 4,
    ISP_EXP_MODE_MAX
} ISP_EXP_MODE_E;

typedef enum _ISP_STRATEGY_E
{
    ISP_STRATEGY_NONE    = 0,
    ISP_STRATEGY_OUTDOOR    = 1,
    ISP_STRATEGY_INDOOR  = 2,
    ISP_STRATEGY_MAX
} ISP_STRATEGY_E;

typedef enum _ISP_ROUTE_STRATEGY_E
{
    ISP_ROUTE_STRATEGY_EXP_PRIO    = 0,
    ISP_ROUTE_STRATEGY_GAIN_PRIO  = 1,
    ISP_ROUTE_STRATEGY_AE_TABLE  = 2,
    ISP_ROUTE_STRATEGY_MAX
} ISP_ROUTE_STRATEGY_E;

typedef enum _ISP_ROUTE_DEFLICKER_MODE_E
{
    ISP_ROUTE_DEFLICKER_MODE_NONE    = 0,
    ISP_ROUTE_DEFLICKER_MODE_50  = 1,
    ISP_ROUTE_DEFLICKER_MODE_60  = 2,
    ISP_ROUTE_DEFLICKER_MODE_DET  = 3,
    ISP_ROUTE_DEFLICKER_MODE_MAX
} ISP_ROUTE_DEFLICKER_MODE_E;

typedef ISP_OP_TYPE_E     aml_isp_op_type;

typedef struct _ISP_AE_ATTR_S
{
    ISP_EXP_MODE_E enExposMode;
    ISP_STRATEGY_E enExposStrategy;
    ISP_ROUTE_STRATEGY_E enRouteStrategy;
    ISP_ROUTE_DEFLICKER_MODE_E enRouteDeflkrMode;
    uint32_t u32Convergence;
    uint32_t u32Compensation;
    uint32_t u32LumaTarget;
    uint32_t u32LumaHdrTarget;
    uint32_t u32LowlightMode;
    uint32_t u32LowlightStr;
    uint32_t u32LowlightGainMax;
    uint32_t u32HighlightTh;
    uint32_t u32HighlightStr;
    uint32_t u32Tolerance;
    mbp_bool_e bEnDelay;
    uint32_t u32DelayCnt;
    uint32_t u32DelayTol;
    uint32_t u32Wsdrlth;
    uint32_t u32LongClip;
    uint32_t u32ErAvgCoeff;
    mbp_bool_e bEnReduceFps;
    uint32_t u32ReduceFps;
    uint32_t u32ReduceFpsTh;
    uint32_t u32ReduceFpsLag;
    uint32_t s32SysMaxIgain;
    uint32_t u32SysMaxShuttime;
    uint32_t s32SysMaxTotalGain;
    uint32_t u32SysMaxRatio;
    uint32_t u32SysCalibLux;
    uint32_t u32SysFdbakDly;
    uint32_t u32EnableGdg;
} ISP_AE_ATTR_S;

typedef struct {
    aml_isp_op_type enExpTimeOpType;
    aml_isp_op_type enAGainOpType;
    aml_isp_op_type enDGainOpType;
    aml_isp_op_type enISPDGainOpType;
    aml_isp_op_type enExposureOpType;

    uint32_t u32ExpTime;
    uint32_t u32AGain;
    uint32_t u32DGain;
    uint32_t u32ISPDGain;
	uint32_t u32ExposureRatio;
} ISP_ME_ATTR_S;

typedef struct _ISP_EXPOSURE_ATTR_S
{
    mbp_bool_e         bByPass;    /*RW; Range:[0, 1]; Format:1.0; */
    ISP_ME_ATTR_S   stManual;
    ISP_AE_ATTR_S   stAuto;
} ISP_EXPOSURE_ATTR_S;

typedef struct {
    mbp_bool_e      bByPass;
    ISP_ME_ATTR_S	stManual;
    ISP_AE_ATTR_S	stAuto;
} isp_exposure_attr_s;

typedef enum {
    ISP_AWB_MODE_GAIN         = 0,
    ISP_AWB_MODE_COLOR_TEM    = 1,
    ISP_AWB_MODE_MAX
} isp_awb_man_mode_e;

typedef struct {
    mbp_bool_e bEnable;

    isp_awb_man_mode_e enManMode;
    uint32_t u32ConvgcSpeed;
    ISP_STRATEGY_E u32MixCtMode;
    uint32_t u32CoverRange;
    mbp_bool_e bEnCustomGrayZone;
    mbp_bool_e bEnAwbCtDynCvrng;    /**< u1, color temperature dynamic cover range enable */
    mbp_bool_e bEnAwbCtLumaWgt;     /**< u1, color temperature luma weighted calculation enable by luma value of local block */
    mbp_bool_e bEnAwbCtWgt;          /**< u1, color temperature luma weighted calculation enable */
    mbp_bool_e bEnAwbCtAdj;          /**< u1, color temperature adjust enable */
} ISP_AWB_ATTR_S;

typedef struct {
    uint32_t u32ManualRGain;
    uint32_t u32ManualBGain;
    uint32_t u32ManualTemperature;
} ISP_MWB_ATTR_S;

typedef struct {
	mbp_bool_e bByPass;
    ISP_MWB_ATTR_S	stManual;
    ISP_AWB_ATTR_S	stAuto;
} isp_wb_attr_s;


/*
Defines the structure of ISP module parameters.
*/
typedef union {
    uint64_t  key;
    struct {
        uint64_t  top_src_inp_chn   : 2 ;
        uint64_t  top_wdr_en        : 1 ;
        uint64_t  top_wdr_inp_chn   : 2 ;
        uint64_t  top_decmp_en      : 1 ;
        uint64_t  top_ifmt_en       : 1 ;
        uint64_t  top_bac_en        : 1 ;
        uint64_t  top_fpnr_en       : 1 ;
        uint64_t  top_ge_en         : 1 ;
        uint64_t  top_dpc_en        : 1 ;
        uint64_t  top_pat_en        : 1 ;
        uint64_t  top_og_en         : 1 ;
        uint64_t  top_sqrt_eotf_en  : 1 ;
        uint64_t  top_lcge_en       : 1 ;
        uint64_t  top_pdpc_en       : 1 ;
        uint64_t  top_cac_en        : 1 ;
        uint64_t  top_rawcnr_en     : 2 ;
        uint64_t  top_snr1_en       : 1 ;
        uint64_t  top_mc_tnr_en     : 1 ;
        uint64_t  top_tnr0_en       : 1 ;
        uint64_t  top_cubic_cs_en   : 1 ;
        uint64_t  top_ltm_en        : 1 ;
        uint64_t  top_gtm_en        : 1 ;
        uint64_t  top_lns_mesh_en   : 1 ;
        uint64_t  top_lns_rad_en    : 1 ;
        uint64_t  top_wb_en         : 1 ;
        uint64_t  top_blc_en        : 1 ;
        uint64_t  top_nr_en         : 1 ;
        uint64_t  top_pk_en         : 1 ;
        uint64_t  top_dnlp_en       : 1 ;
        uint64_t  top_dhz_en        : 1 ;
        uint64_t  top_lc_en         : 1 ;
        uint64_t  top_bsc_en        : 1 ;
        uint64_t  top_cnr2_en       : 1 ;
        uint64_t  top_gamma_en      : 1 ;
        uint64_t  top_ccm_en        : 1 ;
        uint64_t  top_dmsc_en       : 1 ;
        uint64_t  top_csc_en        : 1 ;
        uint64_t  top_ptnr_en       : 1 ;
        uint64_t  top_amcm_en       : 1 ;
        uint64_t  top_flkr_stat_en  : 1 ;
        uint64_t  top_flkr_stat_sw  : 2 ;
        uint64_t  top_awb_stat_en   : 1 ;
        uint64_t  top_awb_stat_sw   : 3 ;
        uint64_t  top_ae_stat_en    : 1 ;
        uint64_t  top_ae_stat_sw    : 2 ;
        uint64_t  top_af_stat_en    : 1 ;
        uint64_t  top_af_stat_sw    : 2 ;
        uint64_t  top_wdr_stat_en   : 1 ;
        uint64_t  top_dbg_path_en   : 1 ;
        uint64_t  top_dbg_data_sel  : 1 ;
        uint64_t  bitRsv            : 6; /* H  ; [58:63] */
    };
} aml_isp_module_ctrl;

typedef struct {
    uint32_t hlc_en;
    uint32_t hlc_luma_thd;
    uint32_t hlc_luma_trgt;
} aml_isp_hlc_attr;

#define AE_HIST_SIZE 7152
typedef struct exps_zone_info_s{
    uint16_t blk_hist[4];/**< local block hist-5bin */
} exps_zone_info_t;

typedef struct {
    uint32_t hist[AE_HIST_SIZE];
    uint32_t hist_sum;
    uint32_t reserve[3];
} aml_isp_ae_statistics;

#define AWB_STAT_BLKHNUM 32
#define AWB_STAT_BLKVNUM 24
typedef struct _isp_awb_stats_pack_t {
    uint32_t pack0;
    uint32_t pack1;
} isp_awb_stats_pack_t;

typedef struct wb_zone_info_s {
    uint16_t rg;/**< red gain (r*4096/g) */
    uint16_t bg;/**< blue gain (b*4096/g) */
    uint16_t avg_r;/**< average red value normalize to 0x10000  */
    uint16_t avg_g;/**< average green value normalize to 0x10000  */
    uint16_t avg_b;/**< average blue value normalize to 0x10000  */
    uint16_t avg_luma;/**< average blue value normalize to 0x10000  */
    uint32_t sum;/**< valid pixel sum on the slice */
} wb_zone_info_t;

typedef struct {
    isp_awb_stats_pack_t data[4*AWB_STAT_BLKHNUM*AWB_STAT_BLKVNUM];

    wb_zone_info_t  zones[4][AWB_STAT_BLKHNUM*AWB_STAT_BLKVNUM];
    uint32_t        zones_rows;
    uint32_t        zones_cols;
    uint32_t        zones_num;
    uint32_t        zones_size;
} aml_isp_awb_statistics;

typedef struct {
    uint32_t  enable;
    uint16_t  table[GAMMA_NODE_NUM];
} aml_isp_gamma_attr;

typedef struct {
    uint32_t   enable;
    uint16_t   table[PREGAMMA_NODE_NUM];
} aml_isp_pregamma_attr;

typedef ISP_FMW_STATE_E aml_isp_fmw_state;

typedef struct {
    int32_t ccm_str;
    int32_t ccm_coef[12];
} aml_isp_saturation_manual;

typedef struct {
    uint32_t ccm_adj[10];
    uint32_t ccm_lut[201];
} aml_isp_saturation_auto;

// -------------- restructure ---------- //
typedef struct {
    int32_t ccm_str[10];
} aml_ccm_str_lut;

typedef struct {
    int32_t ccm_coef[10];
} aml_ccm_coef_calib;

typedef struct {
    int32_t                 size;
    aml_ccm_coef_calib  lut[20];
} aml_ccm_coef_lut;

typedef struct {
    aml_ccm_str_lut   ccm_adj;
    aml_ccm_coef_lut  ccm_lut;
} aml_isp_saturation_auto_s;

typedef struct {
    aml_isp_op_type op_type;
    aml_isp_saturation_manual manual_attr;
    union {
    aml_isp_saturation_auto   auto_attr;
    aml_isp_saturation_auto_s auto_attr_s;
    };
} aml_isp_saturation_attr;

typedef struct {
    int32_t wdr_motiondect_en;
    int32_t wdr_mdetc_withblc_mode;
    int32_t wdr_mdetc_chksat_mode;
    int32_t wdr_mdetc_motionmap_mode;
    int32_t wdr_mdeci_chkstill_mode;
    int32_t wdr_mdeci_addlong;
    int32_t wdr_mdeci_still_thd;
    int32_t wdr_forcelong_en;
    int32_t wdr_forcelong_thdmode;
    int32_t wdr_expcomb_maxavg_mode;
    int32_t wdr_expcomb_maxavg_ratio;
    int32_t wdr_stat_flt_en;
    // 2) fw for lo/hi_weight
    int32_t wdr_mdetc_lo_weight_offset[3];
    int32_t wdr_mdetc_hi_weight_offset[3];
    // 3)fw regs for motion detection
    int32_t wdr_auto_en;
    int32_t wdr_mdetc_sat_thd_mode;
    int32_t wdr_mdetc_weight_mode;
    int32_t wdr_mdetc_sat_gr_offset;
    int32_t wdr_mdetc_sat_gb_offset;
    int32_t wdr_mdetc_sat_rg_offset;
    int32_t wdr_mdetc_sat_bg_offset;
    int32_t wdr_mdetc_sat_ir_offset;
    // 3)fw regs for motion blur correction
    int32_t wdr_flong1_mode;
    int32_t wdr_day_thd;
    int32_t wdr_night_thd;
    int32_t wdr_flong2_day_lothd;
    int32_t wdr_flong2_day_hithd;
    int32_t wdr_flong2_night_lothd;
    int32_t wdr_flong2_night_hithd;
    // 3)fw regs for force long exp
    int32_t wdr_force_exp_en;
    int32_t wdr_force_exp_mode;
} aml_isp_wdr_auto_attr;

typedef struct {
    int32_t sat_gr_thd;
    int32_t sat_gb_thd;
    int32_t sat_rg_thd;
    int32_t sat_bg_thd;
    int32_t sat_ir_thd;
    int32_t wdr_mdetc_lo_weight[EXPO_MAX_CHANNEL];
    int32_t wdr_mdetc_hi_weight[EXPO_MAX_CHANNEL];
    int32_t sqrt_again_g;
    int32_t sqrt_again_rg;
    int32_t sqrt_again_bg;
    int32_t sqrt_again_ir;
    int32_t sqrt_dgain_g;
    int32_t sqrt_dgain_rg;
    int32_t sqrt_dgain_bg;
    int32_t sqrt_dgain_ir;
    int32_t wdr_mdetc_tot_noiseflr_g[EXPO_MAX_CHANNEL];
    int32_t wdr_mdetc_tot_noiseflr_rg[EXPO_MAX_CHANNEL];
    int32_t wdr_mdetc_tot_noiseflr_bg[EXPO_MAX_CHANNEL];
    int32_t mdeci_sexpstill_gr_lsthd[EXPO_MAX_CHANNEL];
    int32_t mdeci_sexpstill_gb_lsthd[EXPO_MAX_CHANNEL];
    int32_t mdeci_sexpstill_rg_lsthd[EXPO_MAX_CHANNEL];
    int32_t mdeci_sexpstill_bg_lsthd[EXPO_MAX_CHANNEL];
    int32_t flong2_thd0[EXPO_MAX_CHANNEL];
    int32_t flong2_thd1[EXPO_MAX_CHANNEL];
    int32_t wdr_flong1_thd0;
    int32_t wdr_flong1_thd1;
} aml_isp_wdr_manual_attr;

// -------------- restructure ---------- //
typedef struct {
    aml_isp_op_type                    op_type;
    aml_isp_wdr_manual_attr            wdr_manual;
    aml_isp_wdr_auto_attr              wdr_auto;
} aml_isp_wdr_attr;

typedef struct {
    int32_t ltm_auto_en;
    int32_t ltm_damper64;
    int32_t ltm_lmin_alpha;
    int32_t ltm_lmax_alpha;
    int32_t ltm_hi_gm_u7;
    int32_t ltm_lo_gm_u6;
    int32_t ltm_dtl_ehn_en;
    int32_t ltm_vs_gtm_alpha;
    int32_t ltm_cc_en;
    int32_t ltm_lmin_med_en;
    int32_t ltm_lmax_med_en;
    int32_t ltm_bld_lvl_adp_en;
    int32_t ltm_lo_hi_gm_auto;
    int32_t ltm_luma_alpha;
} aml_isp_drc_auto_attr;

typedef struct {
    uint32_t ltm_dark_noise;
    uint32_t reg_ltm_lr_u28;
    int32_t reg_ltm_lmin_blk[96];
    int32_t reg_ltm_lmax_blk[96];
    int32_t ltm_expblend_thd0_u14;
    int32_t ltm_expblend_thd1_u14;
    int32_t ltm_gmin_total;
    int32_t ltm_gmax_total;
    int32_t ltm_lgm;
    int32_t ltm_hgm;
    int32_t ltm_lh_gm_auto;
    int32_t ltm_pow_y_u20;
    uint32_t tmp_reg_ltm_pow_divisor;
} aml_isp_drc_manual_attr;

typedef struct {
    uint32_t ltm_hist_blk65[LTM_HIST_BLK];
} aml_isp_ltm_hist_attr;

typedef struct {
    aml_isp_op_type          op_type;
    aml_isp_drc_manual_attr  DrcManual;
    aml_isp_drc_auto_attr    DrcAuto;
    aml_isp_ltm_hist_attr    HistAttr;
} aml_isp_drc_attr;

typedef struct {
    int ltm_lo_gamma_str;
    int ltm_ho_gamma_str;
} aml_isp_drc_gamma_lut;

typedef struct {
    aml_isp_drc_gamma_lut lo_hi_gm[ISO_NUM_MAX];
} aml_isp_drc_gamma_auto_attr;

typedef struct {
    int ltm_lo_gamma_str;
    int ltm_ho_gamma_str;
} aml_isp_drc_gamma_manual_attr;

typedef struct {
    aml_isp_op_type                op_type;
    aml_isp_drc_gamma_manual_attr  DrcGammaManual;
    aml_isp_drc_gamma_auto_attr    DrcGammaAuto;
} aml_isp_drc_gamma_attr;

/* Defines the ISP dehaze attribute */
typedef struct {
    int32_t dhz_dlt_rat;
    int32_t dhz_hig_dlt_rat;
    int32_t dhz_low_dlt_rat;
    int32_t dhz_lmtrat_lowc;
    int32_t dhz_lmtrat_higc;
    int32_t dhz_cc_en;
    int32_t dhz_sky_prot_en;
    int32_t dhz_sky_prot_stre;
    int32_t dhz_str_fixed;
} aml_isp_dehaze_auto_attr;

typedef struct {
    int ram_dhz_nodes[96*5];
    int32_t reg_dhz_sky_prot_stre;
    int32_t reg_dhz_atmos_light;
    int32_t reg_dhz_atmos_light_inver;
    int32_t reg_dhz_sky_prot_stre_offset;
    int32_t reg_dhz_satura_ratio_sky;
    int32_t reg_dhz_str_fixed;
} aml_isp_dehaze_manual_attr;

typedef struct {
    aml_isp_op_type                op_type;
    aml_isp_dehaze_manual_attr     dhz_manual;
    aml_isp_dehaze_auto_attr       dhz_auto;
} aml_isp_dehaze_attr;

typedef struct {
    int32_t lc_blkblend_mode;
    int32_t lc_lmtrat_minmax;
    int32_t lc_contrast_low;
    int32_t lc_contrast_hig;
    int32_t lc_cc_en;
    int32_t lc_ypkbv_slope_lmt_1;
    int32_t lc_ypkbv_slope_lmt_0;
    int32_t lc_str_fixed;
    uint32_t lc_damper64;
    int32_t lc_nodes_alpha;
    int32_t lc_single_bin_th;
    int32_t lc_single_bin_prot_en;
    int32_t lc_single_bin_prot_strgth;
} aml_isp_ldci_ctl_attr;

typedef struct {
    int32_t lmt0;
    int32_t lmt1;
} aml_isp_ldci_strength_attr;

typedef struct {
    aml_isp_ldci_ctl_attr lc_ctl;
    aml_isp_ldci_strength_attr lc_strength[ISO_NUM_MAX];
} aml_isp_ldci_auto_attr;

typedef struct {
    int32_t curve_nodes[96*6];
    uint32_t reg_lc_histvld_thrd;
    uint32_t reg_lc_blackbar_mute_thrd;
    uint32_t reg_lc_pk_vld;
    uint32_t reg_lc_pk_no_trd_mrgn;
    uint32_t reg_lc_pk_1stb_th;
    uint32_t lc_str_fixed;
} aml_isp_ldci_manual_attr;

typedef struct {
    aml_isp_op_type                op_type;
    aml_isp_ldci_manual_attr       ldci_manual;
    aml_isp_ldci_auto_attr         ldci_auto;
} aml_isp_ldci_attr;

typedef struct {
    uint16_t satur_lut[LDCI_SATUR_LUT_NODE];
} aml_isp_ldci_lut_attr;

typedef struct {
    uint32_t dnlp_cuvbld_min;
    uint32_t dnlp_cuvbld_max;
    uint32_t dnlp_clashBgn;
    uint32_t dnlp_clashEnd;
    uint32_t dnlp_blkext_ofst;
    uint32_t dnlp_whtext_ofst;
    uint32_t dnlp_blkext_rate;
    uint32_t dnlp_whtext_rate;
    uint32_t dnlp_dbg_map;
    uint32_t dnlp_final_gain;
    uint32_t dnlp_scurv_low_th;
    uint32_t dnlp_scurv_mid1_th;
    uint32_t dnlp_scurv_mid2_th;
    uint32_t dnlp_scurv_hgh1_th;
    uint32_t dnlp_scurv_hgh2_th;
    uint32_t dnlp_mtdrate_adp_en;
    uint32_t dnlp_ble_en;
    uint32_t dnlp_scn_chg_th;
    uint32_t dnlp_mtdbld_rate;
    uint32_t dnlp_str_fixed;
    int32_t dnlp_by_iso_luma;
    int32_t dnlp_scurv_gain_mode;
    int32_t dnlp_luma_dbg;
} aml_isp_dnlp_auto_attr;

typedef struct {
    int32_t reg_dnlp_schg;
    int32_t reg_dnlp_bld_lvl;
    int32_t alvl_gain;
    int32_t alvl_ofst;
} aml_isp_dnlp_manual_attr;

typedef struct {
    aml_isp_op_type           op_type;
    aml_isp_dnlp_manual_attr  dnlp_manual;
    aml_isp_dnlp_auto_attr    dnlp_auto;
} aml_isp_dnlp_attr;

typedef struct {
    uint16_t peaking_gain1;/**< peaking hp final gain */
    uint16_t peaking_gain2;/**< peaking hp final gain */
    uint16_t peaking_nr_str;/**< peaking pre-flt strength */
    uint16_t peaking_os_up;/**< peaking overshoot up */
    uint16_t peaking_os_down;/**< peaking overshoot dn */
    uint16_t peaking_nr_range;
} aml_isp_sharpen_manual_attr;

typedef struct {
    int32_t pk_flt1_v1d[3];
    int32_t pk_flt2_v1d[3];
    int32_t pk_flt1_h1d[5];
    int32_t pk_flt2_h1d[5];
    int32_t pk_osht_vsluma_lut[9];
    int32_t pk_flt1_2d[3][4];
    int32_t pk_flt2_2d[3][4];
    int32_t pk_motion_adp_en;
    int32_t pk_dejaggy_en;
    int32_t pk_debug_mode;
    int32_t pk_osht_hsize;
    int32_t pk_osht_vsize;
} aml_isp_sharpen_auto_attr;

typedef struct {
    aml_isp_op_type                 op_type;
    aml_isp_sharpen_manual_attr     sharpen_manual;
    aml_isp_sharpen_auto_attr       sharpen_auto;
} aml_isp_sharpen_attr;
typedef struct {
    int32_t rawcnr_totblk_higfrq_en;
    int32_t rawcnr_curblk_higfrq_en;
    int32_t rawcnr_ishigfreq_mode;
} aml_isp_rawcnr_auto_attr;

typedef struct {
    uint16_t rawcnr_sad_cor_np_gain;
    uint16_t rawcnr_higfrq_sublk_sum_dif_thd[2];
    uint16_t rawcnr_higfrq_curblk_sum_difnxn_thd[2];
    uint16_t rawcnr_thrd_ya_min;
    uint16_t rawcnr_thrd_ya_max;
    uint16_t rawcnr_thrd_ca_min;
    uint16_t rawcnr_thrd_ca_max;
    uint16_t reserve;
} aml_isp_rawcnr_adj_attr;

typedef struct {
    aml_isp_rawcnr_adj_attr rawcnr_adj;
    uint8_t rawcnr_meta_gain_lut[8];
    int8_t rawcnr_sps_csig_weight5x5[25];
} aml_isp_rawcnr_manual_attr;

typedef struct {
    aml_isp_op_type               op_type;
    aml_isp_rawcnr_manual_attr    rawcnr_manual;
    aml_isp_rawcnr_auto_attr      rawcnr_auto;
} aml_isp_nr_rawcnr_attr;

typedef struct {
    int16_t snr_wt;
    int16_t snr_np_adj;
    int16_t snr_sad_cor_profile_adj;
    int16_t snr_sad_cor_profile_ofst;
    int16_t snr_sad_wt_sum_th[2];
    int16_t snr_var_flat_th_x[3];
    int16_t snr_var_flat_th_y[3];
    int16_t snr_sad_meta_ratio[4];
} aml_isp_snr_adj_attr;

typedef struct {
    int16_t psnr_y_str;
    int16_t psnr_c_str;
} aml_isp_psnr_adj_attr;

typedef struct {
    uint16_t snr_np_lut16_glb_adj;
    uint16_t snr_meta2alp_glb_adj;
    uint16_t snr_meta_gain_glb_adj;
    uint16_t snr_wt_luma_gain_glb_adj;
    uint16_t snr_grad_gain_glb_adj;
    uint16_t snr_sad_th_mask_gain_glb_adj;
} aml_isp_snr_glb_adj_attr;

typedef struct {
    aml_isp_snr_adj_attr  snr_adj;
    aml_isp_snr_glb_adj_attr snr_glb_adj;
    uint8_t snr_wt_luma_gain[8];
    uint8_t snr_sad_meta2alp[8];
    uint8_t snr_meta_adj[8];
    uint16_t snr_cur_wt[8];
    aml_isp_psnr_adj_attr psnr_adj;
} aml_isp_snr_manual_t;

typedef struct {
    int32_t snr_luma_adj_en;
    int32_t snr_sad_wt_adjust_en;
    int32_t snr_mask_en;
    int32_t snr_meta_en;
    int32_t snr_rad_en;
    int32_t snr_grad_gain[5];
    int32_t snr_sad_th_mask_gain[4];
    int32_t snr_coring_mv_gain_x;
    int32_t snr_coring_mv_gain_xn;
    int32_t snr_coring_mv_gain_y[2];
    int32_t snr_wt_var_adj_en;
    int32_t snr_wt_var_meta_th;
    int32_t snr_wt_var_th_x[3];
    int32_t snr_wt_var_th_y[3];
    int32_t snr_mask_adj[8];
} aml_isp_snr_auto_t;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_snr_manual_t    snr_manual;
    aml_isp_snr_auto_t      snr_auto;
} aml_isp_nr_snr_attr;

typedef struct {
    int32_t tnr_ma_mix_th_x[3];
    int32_t tnr_rad_tnr0_en;
    int32_t tnr_ma_sad_pdtl4_x[3];
    int32_t tnr_ma_sad_pdtl4_y[3];
    int32_t tnr_ma_adp_dtl_mix_th_nfl;
    int32_t tnr_ma_sad_th_mask_gain[4];
    int32_t tnr_ma_mix_th_mask_gain[4];
    int32_t tnr_ma_mix_ratio;
    int32_t tnr_ma_sad_luma_adj_x[4];
    int32_t tnr_ma_sad_luma_adj_y[5];
    int32_t tnr_ma_mix_th_iso_gain;
} aml_isp_tnr_auto_attr;

typedef struct {
    int16_t tnr_sad_cor_np_gain;
    int16_t tnr_sad_cor_np_ofst;
    int16_t tnr_ma_mix_h_th_gain[4];
    int16_t tnr_ma_mix_h_th_y[3];
    int16_t tnr_ma_mix_l_th_y[3];
    int16_t tnr_ma_sad_var_th_x[3];
    int16_t tnr_ma_sad_var_th_y[3];
    int16_t tnr_me_sad_cor_np_gain;
    int16_t tnr_me_sad_cor_np_ofst;
    int16_t tnr_me_meta_sad_th0[3];
    int16_t tnr_me_meta_sad_th1[3];
} aml_isp_tnr_adj_attr;

typedef struct {
    uint16_t tnr_ma_mix_h_th_glb_adj;
    uint16_t tnr_ma_np_lut16_glb_adj;
    uint16_t tnr_ma_sad2alp_glb_adj;
    uint16_t tnr_mc_meta2alp_glb_adj;
} aml_isp_tnr_glb_attr;

typedef struct {
    aml_isp_tnr_adj_attr  tnr_adj;
    aml_isp_tnr_glb_attr  tnr_glb_adj;
    uint8_t tnr_ma_sad2alpha[64];
    uint8_t tnr_mc_meta2alpha[64];
    uint8_t ptnr_alp_lut[8];
} aml_isp_tnr_manual_attr;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_tnr_manual_attr tnr_manual;
    aml_isp_tnr_auto_attr   tnr_auto;
} aml_isp_nr_tnr_attr;

//-------------------------------------
typedef struct {
    int32_t cnr_map_xthd;
    int32_t cnr_map_kappa;
    int32_t cnr_map_ythd0;
    int32_t cnr_map_ythd1;
    int32_t cnr_map_norm;
    int32_t cnr_map_str;
} aml_isp_cnr_auto_attr;

typedef struct {
    uint16_t cnr_wt;
    uint16_t cnr_umargin_up;
    uint16_t cnr_umargin_dw;
    uint16_t cnr_vmargin_up;
    uint16_t cnr_vmargin_dw;
    uint16_t alp_mode;
} aml_isp_cnr_adj_attr;

typedef struct {
    aml_isp_cnr_adj_attr  cnr_adj;
} aml_isp_cnr_manual_attr;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_cnr_manual_attr cnr_manual;
    aml_isp_cnr_auto_attr   cnr_auto;
} aml_isp_nr_cnr_attr;

//---------------------------------------
typedef struct {
    int32_t purple_luma_osat_thd;
    int32_t pfr_mode;
} aml_isp_purple_auto_attr;

typedef struct {
    uint16_t purple_wt;
    uint16_t purple_umargin_up;
    uint16_t purple_umargin_dw;
    uint16_t purple_vmargin_up;
    uint16_t purple_vmargin_dw;
    uint16_t purple_cst_thd;
    uint16_t purple_desat_en;
} aml_isp_purple_adj_attr;

typedef struct {
    aml_isp_purple_adj_attr  purple_adj;
} aml_isp_purple_manual_attr;

typedef struct {
    aml_isp_op_type            op_type;
    aml_isp_purple_manual_attr purple_manual;
    aml_isp_purple_auto_attr   purple_auto;
} aml_isp_purple_attr;

typedef struct {
    uint16_t dms_plp_alp;
    uint16_t detail_non_dir_th_min;
    uint16_t detail_non_dir_th_max;
} aml_isp_dms_manual_attr;

typedef struct {
    uint16_t    dms_adj[ISO_NUM_MAX*3];
} aml_isp_dms_auto_attr;

// -------------- restructure ---------- //
typedef struct {
    uint16_t plp_alp;
    uint16_t detail_non_dir_th_min;
    uint16_t detail_non_dir_th_max;
} aml_dms_adj_lut;

typedef struct {
    aml_dms_adj_lut    dms_adj[ISO_NUM_MAX];
} aml_isp_dms_auto_attr_s;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_dms_manual_attr dms_manual;
    union {
    aml_isp_dms_auto_attr   dms_auto;
    aml_isp_dms_auto_attr_s dms_auto_s;
    };
} aml_isp_dms_attr;

//--------------------------------------
typedef struct {
    uint16_t np_lut[16];
} aml_isp_np_manual_attr;

typedef struct {
    uint16_t np_lut[9*16];
} aml_isp_np_auto_attr;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_np_manual_attr  np_manual;
    aml_isp_np_auto_attr    np_auto;
} aml_isp_np_attr;

typedef struct {
    uint32_t csc_enable;
    uint32_t glb_brightness;
    uint32_t glb_contrast;
    uint32_t glb_sharpness;
    uint32_t glb_sturation;
    uint32_t glb_hue;
    uint32_t glb_vibrance;
} aml_isp_csc_attr;

typedef struct {
    uint32_t    lsc_mesh_split;
    uint32_t    lsc_mesh_norm;
    uint32_t    lsc_mesh_hnum;
    uint32_t    lsc_mesh_vnum;
} aml_isp_mesh_ctl_t;

typedef struct {
    uint16_t lsc_mesh_str;
    uint32_t lsc_mesh_alp[4];
} aml_isp_shading_manual_attr;

typedef struct {
    uint16_t lens_shading_adj[10*2];
    uint32_t lens_shading_ct_correct[2];
} aml_isp_shading_auto_attr;

// -------------- restructure ---------- //
typedef struct {
    uint16_t radial_strength;
    uint16_t mesh_strength;
} aml_lens_shading_adj_lut;

typedef struct {
    uint32_t TL40_diff;
    uint32_t CWF_color_diff;
} aml_lens_shading_ct_correct_lut;

typedef struct {
    aml_lens_shading_adj_lut        lens_shading_adj[10];
    aml_lens_shading_ct_correct_lut lens_shading_ct_correct;
} aml_isp_shading_auto_attr_s;

typedef struct {
    aml_isp_op_type             op_type;
    aml_isp_mesh_ctl_t          mesh_ctl;
    aml_isp_shading_manual_attr mesh_manual;
    union {
    aml_isp_shading_auto_attr   mesh_auto;
    aml_isp_shading_auto_attr_s mesh_auto_s;
    };
} aml_isp_shading_attr;

typedef struct {
    uint8_t  rgain[1024];
    uint8_t  ggain[1024];
    uint8_t  bgain[1024];
} aml_isp_shading_gain_attr;

typedef struct {
    aml_isp_shading_gain_attr aD65Lut;
    aml_isp_shading_gain_attr aCWFLut;
    aml_isp_shading_gain_attr aTL84Lut;
    aml_isp_shading_gain_attr aALut;
} aml_isp_shading_lut_attr;

typedef struct {
    aml_isp_op_type             op_type;
    uint16_t                    lsc_rad_str;
    union {
    aml_isp_shading_auto_attr   radial_auto;
    aml_isp_shading_auto_attr_s radial_auto_s;
    };
} aml_isp_radial_shading_attr;

typedef struct {
    uint16_t  rgain[129];
    uint16_t  ggain[129];
    uint16_t  bgain[129];
} aml_isp_radial_shading_gain_lut;

typedef struct {
    aml_isp_radial_shading_gain_lut rlsc_gain_lut;
} aml_isp_radial_shading_lut_attr;

typedef struct {
    uint16_t ge_stat_edge_thd;
    uint16_t ge_hv_thrd;
    uint16_t ge_hv_wtlut[4];
    uint16_t reserve[2];
} aml_isp_cr_manual_attr;

typedef struct {
    uint16_t ge_adj[ISO_NUM_MAX*8];
} aml_isp_cr_auto_attr;

// -------------- restructure ---------- //
typedef struct {
    uint16_t ge_stat_edge_thd;
    uint16_t ge_hv_thrd;
    uint16_t ge_hv_wtlut[4];
    uint16_t reserve[2];
} aml_ge_adj_lut;

typedef struct {
    aml_ge_adj_lut ge_adj[ISO_NUM_MAX];
} aml_isp_cr_auto_attr_s;

typedef struct {
    aml_isp_op_type           op_type;
    aml_isp_cr_manual_attr    cr_manual;
    union {
    aml_isp_cr_auto_attr      cr_auto;
    aml_isp_cr_auto_attr_s    cr_auto_s;
    };
} aml_isp_cr_attr;

//--------------------------------------
typedef struct {
    int32_t     blc_r;
    int32_t     blc_gr;
    int32_t     blc_gb;
    int32_t     blc_b;
    int32_t     blc_ir;
} aml_isp_blc_manual_attr;

typedef struct {
    uint32_t u32BlackLevelLut[45];
} aml_isp_blc_auto_attr;

// -------------- restructure ---------- //
typedef struct {
    uint32_t black_level[5];
} aml_black_level_lut;

typedef struct {
    aml_black_level_lut u32BlackLevelLut[9];
} aml_isp_blc_auto_attr_s;

typedef struct {
    aml_isp_op_type           op_type;
    aml_isp_blc_manual_attr   blc_manual;
    union {
    aml_isp_blc_auto_attr     blc_auto;
    aml_isp_blc_auto_attr_s   blc_auto_s;
    };
} aml_isp_black_level_attr;

typedef struct {
    uint8_t u8LutTable[20480];
} aml_isp_fpn_calibrate_attr;

typedef struct {
    aml_isp_op_type op_type;
    int32_t fpnr_en;
    int32_t fpnr_xphs_ofst;
    int32_t fpnr_yphs_ofst;
    int32_t fpnr_cali_thrd;
    int32_t fpnr_cali_num;
    int32_t fpnr_trigger;
    int32_t fpnr_corr_offline_mode;
    int32_t fpnr_cali_u32caliIso;
    int32_t fpnr_corr_u32Iso;
    int32_t fpnr_noise_profile[16];
    int32_t fpnr_cali_trigger;
    int32_t fpnr_corr_val;
    int32_t fpnr_corr_ofst;
    int32_t fpnr_corr_mode;
    int32_t fpnr_cali_corr_sel;
} aml_isp_fpn_attr;

typedef struct {
    int32_t ccm_str;
    int32_t ccm_coef[12];
} aml_isp_colormatrix_manual_s;

typedef struct {
    uint32_t ccm_adj[10];
    uint32_t ccm_lut[201];
} aml_isp_colormatrix_auto_s;

typedef struct {
    aml_isp_op_type enOpType;
    aml_isp_colormatrix_manual_s manual_attr;
    union {
    aml_isp_colormatrix_auto_s   auto_attr;
    aml_isp_saturation_auto_s    auto_attr_s;
    };
} aml_isp_colormatrix_attr;

typedef struct {
    int16_t s16AwbMeshCtTab[225];
    int32_t s32AwbCtRgCurve[4];
    int32_t s32AwbCtBgCurve[4];
} aml_isp_ca_attr;

typedef struct {
    uint32_t u32TotalNum;
    int32_t astRouteNode[33];
} aml_isp_ae_route_attr;

typedef struct {
    uint32_t flkr_det_valid_sel;
    uint32_t flkr_det_avg_chnen_mode;
    uint32_t flkr_det_lpf;
    uint32_t flkr_det_cnt_thd;
    uint32_t flkr_det_stat_pk_dis_thr;
    uint32_t flkr_det_stat_pk_val_thr;
    uint32_t flkr_det_stat_pk_val_diff_thd;
    uint32_t flkr_det_fft_det_en;
    uint32_t flkr_det_fft_nlen;
    uint32_t flkr_det_fft_mlen;
    uint32_t flkr_det_fft_norm;
    uint32_t flkr_det_fft_valid_thrd;
    uint32_t flkr_det_sns_exp_info_adj_gain;
    uint32_t flkr_det_jdg_ratio;
    uint32_t flkr_det_sum_pdif_th;
    uint32_t flkr_det_scan_ofst;
    uint32_t flkr_det_wave_ofst;
    uint32_t flkr_det_ae_diff_th;
    uint32_t flkr_det_noflkr_cnt_th;
    uint32_t flkr_det_pdif_ratio;
} aml_isp_calcflicker_input_attr;

typedef struct {
    uint32_t flkr_det_fft_flag;
    uint32_t flkr_det_50hz;
    uint32_t flkr_det_cnt;
    uint32_t flkr_det_r50;
    uint32_t flkr_det_r60;
} aml_isp_calcflicker_output_attr;

typedef struct {
    uint32_t ae_roi_en;            /**< u1, AE ROI0 work mode. 0:off 1:on */
    uint32_t ae_roi_wgt;           /**< u4, ROI0 zone exposure weight */
    uint32_t ae_roi_x_st;          /**< u14, ROI0 rectangle zone start position x  */
    uint32_t ae_roi_y_st;          /**< u14, ROI0 rectangle zone start position y  */
    uint32_t ae_roi_width;         /**< u14, ROI0 rectangle zone width  */
    uint32_t ae_roi_height;        /**< u14, ROI0 rectangle zone height */
    uint32_t ae_roi_target;
} aisp_ae_roi;

typedef struct {
    aisp_ae_roi ae_roi[16];
} aml_isp_ae_roi_attr;

typedef struct {
    uint32_t region_en;
    uint32_t awb_xstart;
    uint32_t awb_xsize;
    uint32_t awb_ystart;
    uint32_t awb_ysize;
    uint32_t rgain;
    uint32_t bgain;
} aisp_awb_region;

typedef struct {
    aisp_awb_region awb_region[2];
} aml_isp_awb_region_attr;

typedef struct {
    int16_t u32AwbRgPos[15];
    int16_t u32AwbBgPos[15];
    int16_t u32AwbMeshDisTab[225];
} aml_isp_awb_attr_ex;

typedef ISP_AWB_INDOOR_OUTDOOR_STATUS_E    aml_isp_awb_indoor_outdoor_status_e;

typedef struct {
    uint16_t u16Rgain;
    uint16_t u16Grgain;
    uint16_t u16Gbgain;
    uint16_t u16Bgain;
    uint16_t u16Saturation;
    uint16_t u16ColorTemp;
    uint16_t u16ColorTempDiff;
    int32_t au32CCM[CCM_MATRIX_SIZE];
} aml_isp_wb_info_attr;

typedef struct {
    uint32_t pack0;
    uint32_t pack1;
} aml_isp_wb_pack_t;

typedef struct {
    aml_isp_wb_pack_t stData[4*32*24];
    uint32_t zones_num;
} aml_isp_wb_stats_attr;

typedef struct {
    uint32_t ae_converged;
    uint32_t ae_slight_change;
    int32_t  ae_sys_expos_log2;
    uint32_t ae_sys_ratio;
    uint32_t ae_sys_min_ratio;
    uint32_t ae_sys_expos_full;
    uint32_t ae_sns_expos_lines;
    uint32_t ae_sns_sexpos_lines;
    uint32_t ae_sns_vsexpos_lines;
    uint32_t ae_sns_vvsexpos_lines;
    int32_t  ae_sns_expo_log2;
    int32_t  ae_sns_shuttime;
    int32_t  ae_sns_again;
    int32_t  ae_sns_hc_again;
    int32_t  ae_sns_dgain;
    int32_t  ae_sns_hc_dgain;
    int32_t  ae_isp_gain;
    int32_t  ae_total_gain;
    int32_t  ae_lowlight_enh_ratio;
    uint32_t ae_daylight;
} aml_isp_exp_info_attr;

typedef struct {
    int32_t dn_light_control; //1:0n, 0: off
    int32_t dn_hist_stat_mode; //0: average based AE, 1: weight
    int32_t dn_predict_day_thr;  //default is 50
    int32_t dn_predict_night_thr; //default is 50
    int32_t dn_det_tran_ratio; //default 16/128
    int32_t dn_det_day_thr;    //default 60
    int32_t dn_det_night_thr;  //default 240
    int32_t dn_det_light_ct_low;
    int32_t dn_det_light_ct_high;
    int32_t dn_wdr_mean_ratio;     //default 280, normalize with 256
    int32_t dn_rg_blk_sum_thr;     //low thr of block sum based on rg info
    int32_t dn_rg_thr;
    int32_t dn_bg_thr;
    int32_t dn_debug; // 0:not print 1:print
} aml_isp_ir_attr;


//--------------------------------
typedef struct {
    uint8_t dpc_cor_en;
    uint8_t dpc_avg_dev_mode;
    uint8_t dpc_avg_mode;
    uint8_t dpc_avg_thd2_en;
    uint8_t dpc_highlight_en;
    uint8_t dpc_correct_mode;
    uint8_t dpc_write_to_lut;
    uint8_t reserve;
} aml_isp_dpc_auto_attr;

typedef struct {
    uint16_t dpc_avg_gain_l0;
    uint16_t dpc_avg_gain_h0;
    uint16_t dpc_avg_gain_l1;
    uint16_t dpc_avg_gain_h1;
    uint16_t dpc_avg_gain_l2;
    uint16_t dpc_avg_gain_h2;
    uint16_t dpc_cond_en;
    uint16_t dpc_max_min_bias_thd;
    uint16_t dpc_std_diff_gain;
    uint16_t dpc_std_gain;
    uint16_t dpc_avg_dev_offset;
    uint16_t reserve;
} aml_isp_dpc_manual_attr;

typedef struct {
    aml_isp_op_type         enOpType;
    aml_isp_dpc_manual_attr manual_attr;
    aml_isp_dpc_auto_attr   auto_attr;
} aml_isp_dpc_attr;

//-----------------------------------------
typedef struct {
    int32_t cm_sat;
    int32_t cm_hue;
    int32_t cm_contrast;
    int32_t cm_brightness;
} aml_isp_cm2_manual_attr;

typedef struct {
    aml_isp_cm2_manual_attr stMan[ISO_NUM_MAX];
} aml_isp_cm2_auto_attr;

typedef struct {
    aml_isp_op_type         op_type;
    aml_isp_cm2_manual_attr manual_attr;
    aml_isp_cm2_auto_attr   auto_attr;
} aml_isp_cm2_attr;
#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __AML_COMM_ISP_ADAPT_H__ */
