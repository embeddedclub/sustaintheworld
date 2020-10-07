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

#include "project.h"

#define T_EINK154_P    const uint8_t*

/** @defgroup EINK154_TYPES Types */                             /** @{ */

typedef struct
{
    const uint8_t*   thisFont;
    uint16_t         color;
    uint8_t          orientation;
    uint16_t         first_char;
    uint16_t         last_char;
    uint16_t         height;

}T_EINK154_FONT;

typedef struct
{
  uint16_t x;
  uint16_t y;

} T_EINK154_COORDINATE;

/* Fill screen color*/
extern const uint8_t _EINK154_SCREEN_COLOR_WHITE ;
extern const uint8_t _EINK154_SCREEN_COLOR_BLACK ;
extern const uint8_t _EINK154_SCREEN_COLOR_LIGHT_GREY ;
extern const uint8_t _EINK154_SCREEN_COLOR_DARK_GREY  ;

extern const uint8_t _EINK154_FO_HORIZONTAL      ;
extern const uint8_t _EINK154_FO_VERTICAL        ;
extern const uint8_t _EINK154_FO_VERTICAL_COLUMN ;  

void eink154_setLut();
void eink154_startConfig();
void eink154_displayImage ( const uint8_t* image_buffer );

void eink154_text( char *text, uint8_t nChar, uint16_t x, uint16_t y );
void eink154_setFont( const uint8_t *font, uint16_t color, uint8_t orientation );


/* [] END OF FILE */
