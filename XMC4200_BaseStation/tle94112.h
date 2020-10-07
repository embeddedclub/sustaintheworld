/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <xmc_common.h>

#define HB_ACT_1_CTRL       0x03
#define HB_ACT_2_CTRL       0x43
#define HB_ACT_3_CTRL       0x23

#define HB_MODE_1_CTRL      0x63
#define HB_MODE_2_CTRL      0x13
#define HB_MODE_3_CTRL      0x53

#define PWM_CH_FREQ_CTRL    0x33
#define PWM1_DC_CTRL        0x73
#define PWM2_DC_CTRL        0x0B
#define PWM3_DC_CTRL        0x4B

#define FW_OL_CTRL          0x2B
#define FW_CTRL             0x6B


#define SYS_DIAG1           0x1B
#define OP_ERROR_1_STAT     0x5B
#define OP_ERROR_2_STAT     0x3B
#define OP_ERROR_3_STAT     0x7B
#define OP_ERROR_4_STAT     0x07
#define OP_ERROR_5_STAT     0x47
#define OP_ERROR_6_STAT     0x27


void tle_init();
void tle_setpwm(uint8_t pwm);
void tle_config12(int8_t dir, uint8_t pwm);
void tle_clear_errors();
uint16_t tle_config12_read();
void tle_config1(uint8_t pwm);

void tle_configHB123(uint8_t value);

void tle_setpwm1(uint8_t value);
void tle_setpwm2(uint8_t value);
void tle_setpwm3(uint8_t value);
void tle_setpwmModFreq(uint8_t channel, uint8_t mod, uint8_t freq);
void tle_configHB1LS(uint8_t out_en, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel);
void tle_configHB2LS(uint8_t out_en, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel);
void tle_configHB3(uint8_t out, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel);

void tle_configHB_LS12(uint8_t value, uint8_t pwm_mode);

/* [] END OF FILE */
