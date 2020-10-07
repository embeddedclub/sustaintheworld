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

#include "ssd1608.h"
#include "spi_comm.h"
#include "stdlib.h"

/* Display resolution */
const uint16_t _EINK154_DISPLAY_WIDTH  = 200;
const uint16_t _EINK154_DISPLAY_HEIGHT = 200;
const uint16_t _EINK154_DISPLAY_RESOLUTIONS = 5000;

/* Fill screen color*/
const uint8_t _EINK154_SCREEN_COLOR_WHITE = 0xFF;
const uint8_t _EINK154_SCREEN_COLOR_BLACK = 0x00;
const uint8_t _EINK154_SCREEN_COLOR_LIGHT_GREY = 0xAA;
const uint8_t _EINK154_SCREEN_COLOR_DARK_GREY  = 0x55;

const uint8_t _EINK154_FO_HORIZONTAL = 0x00;
const uint8_t _EINK154_FO_VERTICAL = 0x01;
const uint8_t _EINK154_FO_VERTICAL_COLUMN = 0x02;



// eINK display 1.54 inch commands
const uint8_t _EINK154_CMD_DRIVER_OUTPUT_CONTROL                      = 0x01;
const uint8_t _EINK154_CMD_BOOSTER_SOFT_START_CONTROL                 = 0x0C;
const uint8_t _EINK154_CMD_GATE_SCAN_START_POSITION                   = 0x0F;
const uint8_t _EINK154_CMD_DEEP_SLEEP_MODE                            = 0x10;
const uint8_t _EINK154_CMD_DATA_ENTRY_MODE_SETTING                    = 0x11;
const uint8_t _EINK154_CMD_SW_RESET                                   = 0x12;
const uint8_t _EINK154_CMD_TEMPERATURE_SENSOR_CONTROL                 = 0x1A;
const uint8_t _EINK154_CMD_MASTER_ACTIVATION                          = 0x20;
const uint8_t _EINK154_CMD_DISPLAY_UPDATE_CONTROL_1                   = 0x21;
const uint8_t _EINK154_CMD_DISPLAY_UPDATE_CONTROL_2                   = 0x22;
const uint8_t _EINK154_CMD_WRITE_RAM                                  = 0x24;
const uint8_t _EINK154_CMD_WRITE_VCOM_REGISTER                        = 0x2C;
const uint8_t _EINK154_CMD_WRITE_LUT_REGISTER                         = 0x32;
const uint8_t _EINK154_CMD_SET_DUMMY_LINE_PERIOD                      = 0x3A;
const uint8_t _EINK154_CMD_SET_GATE_TIME                              = 0x3B;
const uint8_t _EINK154_CMD_BORDER_WAVEFORM_CONTROL                    = 0x3C;
const uint8_t _EINK154_CMD_SET_RAM_X_ADDRESS_START_END_POSITION       = 0x44;
const uint8_t _EINK154_CMD_SET_RAM_Y_ADDRESS_START_END_POSITION       = 0x45;
const uint8_t _EINK154_CMD_SET_RAM_X_ADDRESS_COUNTER                  = 0x4E;
const uint8_t _EINK154_CMD_SET_RAM_Y_ADDRESS_COUNTER                  = 0x4F;
const uint8_t _EINK154_CMD_TERMINATE_FRAME_READ_WRITE                 = 0xFF;

const uint8_t _EINK154_LUT_TABLE[ 30 ] =
{
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

/* Text Variables */
static T_EINK154_FONT _font;
static T_EINK154_COORDINATE _cord;

static uint8_t          _frame[ 5000 ];

uint16_t buffer1_size, buffer2_size;
uint8_t *buffer1_p, *buffer2_p;
uint8_t *blackbuffer_p, *buffer2_p;
uint8_t blackInverted = 0;
uint8_t colorInverted = 0;
uint8_t EPD_command(uint8_t c, uint8_t end); 
uint8_t EPD_command_data(uint8_t cmd, uint8_t *data, uint8_t length);

static void _frame_px( uint16_t x, uint16_t y, uint8_t font_col )
{
    uint16_t  off;
    uint16_t pos;

    pos = (y * (_EINK154_DISPLAY_WIDTH / 8)) + (x / 4);
    off = (3 - (x % 4)) * 2;

    _frame[ pos ] &= ~( 0x03 << off );
    _frame[ pos ] |= ( ( font_col & 0x03 ) << off );
}

static void _char_wr( uint16_t ch_idx )
{
    uint8_t     ch_width = 0;
    uint8_t     x_cnt;
    uint8_t     y_cnt;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint16_t     tmp;
    uint8_t     temp = 0;
    uint8_t     mask = 0;
    uint32_t    offset;
    const uint8_t *ch_table;
    const uint8_t *ch_bitmap;

    if( ch_idx < _font.first_char )
    {
        return;
    }
    if( ch_idx > _font.last_char )
    {
        return;
    }

    offset = 0;
    tmp = (ch_idx - _font.first_char) << 2;
    ch_table = (const uint8_t*)(_font.thisFont + (8 + tmp));
    ch_width = *ch_table;

    offset = (uint32_t)ch_table[1] + ((uint32_t)ch_table[2] << 8) + ((uint32_t)ch_table[3] << 16);

    ch_bitmap = _font.thisFont + offset;

    if( ( _font.orientation == _EINK154_FO_HORIZONTAL ) ||
        ( _font.orientation == _EINK154_FO_VERTICAL_COLUMN ) )
    {
        y = _cord.y;
        for (y_cnt = 0; y_cnt < _font.height; y_cnt++)
        {
            x = _cord.x;
            mask = 0;
            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( !mask )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                {
                    _frame_px( x, y, _font.color );
                    CyDelayUs(80);
                }

                x++;
                mask <<= 1;
            }
            y++;
        }

        if ( _font.orientation == _EINK154_FO_HORIZONTAL )
        {
            _cord.x = x + 1;
        }
        else
        {
            _cord.y = y;
        }
    }
    else
    {
        y = _cord.x;

        for( y_cnt = 0; y_cnt < _font.height; y_cnt++ )
        {
            x = _cord.y;
            mask = 0;

            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( mask == 0 )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                {
                    _frame_px( x, y, _font.color );
                }

                x--;
                mask <<= 1;
            }
            y++;
        }
        _cord.y = x - 1;
    }
}

static void _waitUntilIdle()
{
    uint8_t state;
    do
    {
        /* BUSY PIN */
        state = BUSY_Read();
        CyDelay(100);

    }while(state == 1);
}

void eink154_reset()
{
    RES_Write(0);
    CyDelay(200);
    RES_Write(1);
    CyDelay(200);
}

uint8_t eink154_sendCommand(uint8_t cmd) {
  // SPI
  
  DC_Write(0);
  CS_Write(0);

  uint8_t ret = spi_write(&cmd, 1);
  CS_Write(1);
  return ret;
}

uint8_t eink154_sendData(uint8_t data) {
  // SPI
  
  CS_Write(1);
  DC_Write(1);
  CS_Write(0);

  uint8_t ret = spi_write(&data, 1);
  CS_Write(1);
  return ret;
}


uint8_t eink154_sendDataBurst( uint8_t *data, uint16_t len) {
  // SPI
  
  CS_Write(1);
  DC_Write(1);
  CS_Write(0);
  uint8_t ret = spi_write(data, len);
  CS_Write(1);
  return ret;
}

static void DISPLAY_DELAY()
{
    CyDelay(2);
}

void eink154_sleepMode()
{
    eink154_sendCommand(_EINK154_CMD_DEEP_SLEEP_MODE);
    _waitUntilIdle();
}


void eink154_setLut()
{
    uint8_t cnt;
    
    eink154_sendCommand(_EINK154_CMD_WRITE_LUT_REGISTER);
    
    for (cnt = 0; cnt < 30u; cnt++)
    {
        eink154_sendData( _EINK154_LUT_TABLE[cnt] );
    }
}


void eink154_startConfig()
{
    eink154_reset();
    eink154_sendCommand(_EINK154_CMD_DRIVER_OUTPUT_CONTROL);
    eink154_sendData((_EINK154_DISPLAY_HEIGHT - 1) & 0xFF);
    eink154_sendData(((_EINK154_DISPLAY_HEIGHT - 1) >> 8) & 0xFF);
    eink154_sendData(0x00);                     // GD = 0; SM = 0; TB = 0;
    eink154_sendCommand(_EINK154_CMD_BOOSTER_SOFT_START_CONTROL);
    eink154_sendData(0xD7);
    eink154_sendData(0xD6);
    eink154_sendData(0x9D);
    eink154_sendCommand(_EINK154_CMD_WRITE_VCOM_REGISTER);
    eink154_sendData(0xA8);                     // VCOM 7C
    eink154_sendCommand(_EINK154_CMD_SET_DUMMY_LINE_PERIOD);
    eink154_sendData(0x1A);                     // 4 dummy lines per gate
    eink154_sendCommand(_EINK154_CMD_SET_GATE_TIME);
    eink154_sendData(0x08);                     // 2us per line
    eink154_sendCommand(_EINK154_CMD_DATA_ENTRY_MODE_SETTING);
    eink154_sendData(0x03);                     // X increment; Y increment

    DISPLAY_DELAY();
}

void eink154_setMemoryPointer(uint8_t x, uint8_t y)
{
    eink154_sendCommand(_EINK154_CMD_SET_RAM_X_ADDRESS_COUNTER);
    eink154_sendData((x >> 3) & 0xFF);
    eink154_sendCommand(_EINK154_CMD_SET_RAM_Y_ADDRESS_COUNTER);
    eink154_sendData(y & 0xFF);
    eink154_sendData((y >> 8) & 0xFF);
    _waitUntilIdle();
}


void eink154_setMemoryArea(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
    eink154_sendCommand(_EINK154_CMD_SET_RAM_X_ADDRESS_START_END_POSITION);
    eink154_sendData((x_start >> 3) & 0xFF);
    eink154_sendData((x_end >> 3) & 0xFF);
    eink154_sendCommand(_EINK154_CMD_SET_RAM_Y_ADDRESS_START_END_POSITION);
    eink154_sendData(y_start & 0xFF);
    eink154_sendData((y_start >> 8) & 0xFF);
    eink154_sendData(y_end & 0xFF);
    eink154_sendData((y_end >> 8) & 0xFF);
}

void eink154_updateDisplay()
{
    CyDelay(100);
    eink154_sendCommand(_EINK154_CMD_DISPLAY_UPDATE_CONTROL_2);
    eink154_sendData(0xC4);
    eink154_sendCommand(_EINK154_CMD_MASTER_ACTIVATION);
    eink154_sendCommand(_EINK154_CMD_TERMINATE_FRAME_READ_WRITE);
    _waitUntilIdle();
}



void eink154_displayImage ( const uint8_t* image_buffer )
{
    uint16_t cnt;
    uint8_t tempbuf[210];
    uint8_t tbi = 0;

    eink154_setMemoryArea(0, 0, _EINK154_DISPLAY_WIDTH - 1, _EINK154_DISPLAY_HEIGHT - 1);
    eink154_setMemoryPointer(0, 0);

    eink154_sendCommand(_EINK154_CMD_WRITE_RAM);
    for ( cnt = 0; cnt < _EINK154_DISPLAY_RESOLUTIONS; cnt++ )
    {
        tempbuf[ tbi++ ]     = image_buffer[cnt];

        if ( tbi == 200 )
        {
            eink154_sendDataBurst( tempbuf, 200 );
            tbi = 0;
        }
     }

    DISPLAY_DELAY();
    eink154_updateDisplay();
}


void eink154_text( char *text, uint8_t nChar, uint16_t x, uint16_t y )
{
    uint16_t cnt;
    uint8_t tempbuf[210];
    uint8_t tbi = 0;

    if((x >= _EINK154_DISPLAY_WIDTH) || (y >= _EINK154_DISPLAY_HEIGHT))
    {
        return;
    }
    _cord.x = x;
    _cord.y = y;

    for (cnt = 0; cnt < nChar; cnt++)
    {
        _char_wr( text[cnt] );
    }

    eink154_setMemoryArea(0, 0, _EINK154_DISPLAY_WIDTH - 1, _EINK154_DISPLAY_HEIGHT - 1);
    eink154_setMemoryPointer(0, 0);

    eink154_sendCommand(_EINK154_CMD_WRITE_RAM);
    for ( cnt = 0; cnt < _EINK154_DISPLAY_RESOLUTIONS; cnt++ )
    {
        tempbuf[ tbi++ ]     = _frame[cnt];

        if ( tbi == 200 )
        {
            eink154_sendDataBurst( tempbuf, 200 );
            tbi = 0;
        }
     }

    DISPLAY_DELAY();
    eink154_updateDisplay();
}

void eink154_setFont( const uint8_t *font, uint16_t color, uint8_t orientation )
{
    _font.thisFont      = font;
    _font.first_char    = font[ 2 ] + ( font[ 3 ] << 8 );
    _font.last_char     = font[ 4 ] + ( font[ 5 ] << 8 );
    _font.height        = font[ 6 ];
    _font.color         = color;
    _font.orientation   = orientation ;
}


/* [] END OF FILE */
