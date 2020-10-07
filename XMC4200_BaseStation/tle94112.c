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
#include "tle94112.h"
#include "spi_drv.h"

uint16_t resposne = 0;


volatile uint8_t HB_ACT_CTRL[3]={0};
volatile uint8_t HB_MODE_CTRL[3]={0};
volatile uint8_t FW_MCTRL[2];
volatile uint8_t PWM_FREQ_CTRL=0;




void tle_init()
{
	uint8_t spiframe[2]={0};

	spiframe[0] = HB_ACT_1_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = HB_ACT_2_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = HB_ACT_3_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = HB_MODE_1_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = HB_MODE_2_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = HB_MODE_3_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = PWM1_DC_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

	spiframe[0] = PWM2_DC_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);


	spiframe[0] = PWM3_DC_CTRL | 0x80;
	spiframe[1] |= 0x0;
	resposne = spi_write_buffer(spiframe,2);

    
}

uint16_t tle_config12_read()
{
    uint8_t spiframe[2]={0};
    uint16_t response=0;
    
    spiframe[0] = HB_ACT_1_CTRL;
    spiframe[1] = 0x0; 
    
    response = spi_read(spiframe,2);
    
    return response;
}

void tle_setpwm1(uint8_t value)
{
    uint8_t spiframe[2]={0};
    spiframe[0] = PWM1_DC_CTRL | 0x80;
    spiframe[1] = value;
    resposne  = spi_write_buffer(spiframe,2);
}

void tle_setpwm2(uint8_t value)
{
    uint8_t spiframe[2]={0};
    spiframe[0] = PWM2_DC_CTRL | 0x80;
    spiframe[1] = value;
    resposne  = spi_write_buffer(spiframe,2);
}

void tle_setpwm3(uint8_t value)
{
    uint8_t spiframe[2]={0};
    spiframe[0] = PWM3_DC_CTRL | 0x80;
    spiframe[1] = value;
    resposne  = spi_write_buffer(spiframe,2);
}

void tle_setpwmModFreq(uint8_t channel, uint8_t mod, uint8_t freq)
{
    uint8_t spiframe[2]={0};

    spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
    PWM_FREQ_CTRL &= ~(0x3<<(channel*2));
    PWM_FREQ_CTRL |= (freq & 0x3) << (channel*2);
    PWM_FREQ_CTRL &= ~(0xc0);
    PWM_FREQ_CTRL |=  mod<<6;
    spiframe[1] = PWM_FREQ_CTRL;
    resposne  = spi_write_buffer(spiframe,2);

}

void tle_configHB1LS(uint8_t out_en, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel)
{
    uint8_t spiframe[2]={0};
    
    spiframe[0] = HB_ACT_1_CTRL | 0x80;

    HB_ACT_CTRL[0] &= ~0x1;
    HB_ACT_CTRL[0] |= out_en & 0x1;
    spiframe[1] = HB_ACT_CTRL[0];
    spi_write_buffer(spiframe,2);

    spiframe[0] = HB_MODE_1_CTRL | 0x80;

    HB_MODE_CTRL[0] &= ~0x3;
    HB_MODE_CTRL[0] |= pwm_mode & 0x3;
    spiframe[1] = HB_MODE_CTRL[0];
    spi_write_buffer(spiframe,2);
#if 0

    spiframe[0] = FW_OL_CTRL | 0x80;

    FW_MCTRL[0] &= ~0x4;
    FW_MCTRL[0] |= fw_en & 0x4;
    spiframe[1] = FW_MCTRL[0];
    spi_write_buffer(spiframe,2);
#endif
}

void tle_configHB_LS12(uint8_t value, uint8_t pwm_mode)
{
    uint8_t spiframe[2]={0};

    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = value |  value<<2;
    spi_write_buffer(spiframe,2);

    spiframe[0] = HB_MODE_1_CTRL | 0x80;
    spiframe[1] = pwm_mode | pwm_mode<<2;
    spi_write_buffer(spiframe,2);

}




void tle_configHB2LS(uint8_t out_en, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel)
{
    uint8_t spiframe[2]={0};

    spiframe[0] = HB_ACT_1_CTRL | 0x80;

    HB_ACT_CTRL[0] &= ~0x4;
    HB_ACT_CTRL[0] |= out_en<<2 & 0x4;
    spiframe[1] = HB_ACT_CTRL[0];
    spi_write_buffer(spiframe,2);

    spiframe[0] = HB_MODE_1_CTRL | 0x80;

    HB_MODE_CTRL[0] &= ~0xc;
    HB_MODE_CTRL[0] |= (pwm_mode<<2) & 0xc;
    spiframe[1] = HB_MODE_CTRL[0];
    spi_write_buffer(spiframe,2);

#if 0
    spiframe[0] = FW_OL_CTRL | 0x80;

    FW_MCTRL[0] &= ~0x8;
    FW_MCTRL[0] |= fw_en & 0x8;
    spiframe[1] = FW_MCTRL[0];
    spi_write_buffer(spiframe,2);

#endif

}

void tle_configHB3(uint8_t out, uint8_t pwm_mode,uint8_t fw_en, uint8_t ol_sel)
{
    uint8_t spiframe[2]={0};
    
    spiframe[0] = HB_ACT_1_CTRL | 0x80;

    HB_ACT_CTRL[0] &= ~0x30;
    HB_ACT_CTRL[0] |= (1<<out)<<4 & 0x30;
    spiframe[1] = HB_ACT_CTRL[0];
    spi_write_buffer(spiframe,2);

    spiframe[0] = HB_MODE_1_CTRL | 0x80;

    HB_MODE_CTRL[0] &= ~0x30;
    HB_MODE_CTRL[0] |= (pwm_mode<<4) & 0x30;
    spiframe[1] = HB_MODE_CTRL[0];
    spi_write_buffer(spiframe,2);


    spiframe[0] = FW_OL_CTRL | 0x80;

    FW_MCTRL[0] &= ~(0x1<<4);
    FW_MCTRL[0] |= fw_en & (0x1<<4);
    spiframe[1] = FW_MCTRL[0];
    spi_write_buffer(spiframe,2);
      
}

void tle_configHB23(uint8_t pwm)
{
    uint8_t spiframe[2]={0};
    
    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = 0x28 | 0x1;//14;  
    spi_write_buffer(spiframe,2);

    spiframe[0] = FW_OL_CTRL | 0x80;
    spiframe[1] = 0x00 | 0x5;
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = HB_MODE_1_CTRL | 0x80;
    spiframe[1] = 0x00 | 0x01;  
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
    spiframe[1] = 0x2; //0x01;   
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = PWM1_DC_CTRL | 0x80;
    spiframe[1] = pwm;   
    spi_write_buffer(spiframe,2);
      
}


void tle_config1(uint8_t pwm)
{
    uint8_t spiframe[2]={0};
    
    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = 0x1;  
    spi_write_buffer(spiframe,2);

    spiframe[0] = FW_OL_CTRL | 0x80;
    spiframe[1] = 0x5;
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = HB_MODE_1_CTRL | 0x80;
    spiframe[1] = 0x01;  
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
    spiframe[1] = 0x1; //0x01;   
    spi_write_buffer(spiframe,2);
    
    spiframe[0] = PWM1_DC_CTRL | 0x80;
    spiframe[1] = pwm;   
    spi_write_buffer(spiframe,2);
    
}

void tle_config12(int8_t dir, uint8_t pwm)
{
    uint8_t spiframe[2]={0};

    if(dir == 1)
    {
    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = 0x9;
    
    }
    else if(dir == -1)
    {
    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = 0x6;        
    }
    else
    {
    spiframe[0] = HB_ACT_1_CTRL | 0x80;
    spiframe[1] = 0x0;       
    }

    resposne  = spi_write_buffer(spiframe,2);
    
    #if 1
    if(dir == 1 ){
        spiframe[0] = FW_OL_CTRL | 0x80;
        spiframe[1] = 0x8;   
    }
    else if( dir == -1)
    {
        spiframe[0] = FW_OL_CTRL | 0x80;
        spiframe[1] = 0x4;  
    }
    else
    {
        spiframe[0] = FW_OL_CTRL | 0x80;
        spiframe[1] = 0x0;  
    }

    resposne  = spi_write_buffer(spiframe,2);
    #endif 
    
    if(dir == 1 ){
        spiframe[0] = HB_MODE_1_CTRL | 0x80;
        spiframe[1] = 0x01;   
    }
    else if( dir == -1)
    {
        spiframe[0] = HB_MODE_1_CTRL | 0x80;
        spiframe[1] = 0x4;  
    }
    else
    {
        spiframe[0] = HB_MODE_1_CTRL | 0x80;
        spiframe[1] = 0x0;        
    }
    
    resposne  = spi_write_buffer(spiframe,2);
    
    if( dir != 0){
    
    spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
    spiframe[1] = 0x3; //0x01;   
    resposne  = spi_write_buffer(spiframe,2);
    
    
    spiframe[0] = PWM1_DC_CTRL | 0x80;
    spiframe[1] = pwm;   
    resposne  = spi_write_buffer(spiframe,2);
    
    }
    else
    {
    spiframe[0] = PWM_CH_FREQ_CTRL | 0x80;
    spiframe[1] = 0x0; //0x01;   
    resposne  = spi_write_buffer(spiframe,2);
    
    
    spiframe[0] = PWM1_DC_CTRL | 0x80;
    spiframe[1] = 0;   
    resposne  = spi_write_buffer(spiframe,2);
    }

}


void tle_clear_errors()
{
    uint8_t spiframe[2]={0};
    
    spiframe[0] =  SYS_DIAG1 | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);

    spiframe[0] =  OP_ERROR_1_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
    
    spiframe[0] =  OP_ERROR_2_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
    
    spiframe[0] =  OP_ERROR_3_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
    
    spiframe[0] =  OP_ERROR_4_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
    
    spiframe[0] =  OP_ERROR_5_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
    
    spiframe[0] =  OP_ERROR_6_STAT | 0x80;
    spiframe[1] = 0;     
    spi_write_buffer(spiframe,2);
}

/* [] END OF FILE */
