
#include "spi_drv.h"


#define SPI_CH XMC_SPI1_CH1

#define SCLK     P1_8
#define MOSI     P1_9
#define CS       P1_7
#define MISO     P0_0
#define EN		 P0_11

const XMC_SPI_CH_CONFIG_t spi_channel_config =
{
  .baudrate = 100000U,
  .bus_mode = XMC_SPI_CH_BUS_MODE_MASTER,
  .selo_inversion = XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS,
  .parity_mode = XMC_USIC_CH_PARITY_MODE_NONE
};


static void spi_write(uint8_t data)
{

	XMC_SPI_CH_ClearStatusFlag(SPI_CH, XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_SPI_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);
	XMC_SPI_CH_Transmit(SPI_CH, data, XMC_SPI_CH_MODE_STANDARD);
	//XMC_GPIO_SetOutputHigh(ADC_BUSY);
	while ((XMC_SPI_CH_GetStatusFlag(SPI_CH) & (XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_SPI_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) == 0);
	//XMC_GPIO_SetOutputLow(ADC_BUSY);
}

void spi_test_write()
{
	  XMC_SPI_CH_ClearStatusFlag(SPI_CH, XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_SPI_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION);
	  XMC_SPI_CH_Transmit(SPI_CH, 0x1234, XMC_SPI_CH_MODE_STANDARD);
	  XMC_GPIO_SetOutputHigh(EN);
	  while ((XMC_SPI_CH_GetStatusFlag(SPI_CH) & (XMC_SPI_CH_STATUS_FLAG_RECEIVE_INDICATION | XMC_SPI_CH_STATUS_FLAG_ALTERNATIVE_RECEIVE_INDICATION)) == 0);
	  XMC_GPIO_SetOutputLow(EN);
}

void spi_init()
{

	XMC_GPIO_SetMode(MISO, XMC_GPIO_MODE_INPUT_PULL_UP);

	XMC_SPI_CH_Init(SPI_CH, &spi_channel_config);

	XMC_SPI_CH_SetFrameLength(SPI_CH, 16U);
	XMC_SPI_CH_SetWordLength(SPI_CH, 8U);
	XMC_SPI_CH_SetBitOrderMsbFirst(SPI_CH);

	/*Enable transmission when TDV = 1 and the selected edge of the DX2 signal occurs*/
//	XMC_USIC_CH_SetInputTriggerCombinationMode(SPI_CH, XMC_USIC_CH_INPUT_DX2, XMC_USIC_CH_INPUT_COMBINATION_MODE_RISING_EDGE);
//	XMC_USIC_CH_EnableTBUFDataValidTrigger(SPI_CH);

	/* Set input source for input stage dx0 (receive pin) */
	XMC_SPI_CH_SetInputSource(SPI_CH, XMC_SPI_CH_INPUT_DIN0, USIC1_C1_DX0_P0_0);
	/* Set input source for input stage dx2 */
	//XMC_SPI_CH_SetInputSource(SPI_CH, XMC_SPI_CH_INPUT_SLAVE_SELIN, USIC1_C1_DX2_SELO0);

	/* Enable Frame End Mode */
	XMC_SPI_CH_EnableFEM(SPI_CH);
	/* Configure the clock polarity and clock delay */
	XMC_SPI_CH_ConfigureShiftClockOutput(SPI_CH, XMC_SPI_CH_BRG_SHIFT_CLOCK_PASSIVE_LEVEL_0_DELAY_ENABLED,
											   XMC_SPI_CH_BRG_SHIFT_CLOCK_OUTPUT_SCLK);

	/* Disable all Slave Select lines */
	XMC_SPI_CH_DisableSlaveSelect(SPI_CH);
	/* Establish Slave Select signal polarity */
	XMC_SPI_CH_SetSlaveSelectPolarity(SPI_CH, XMC_SPI_CH_SLAVE_SEL_INV_TO_MSLS);
	/* Set Select Control */;
	XMC_SPI_CH_DisableSlaveSelectCodedMode(SPI_CH);
	/* Configure the leading/trailing delay for the MSLS signal */
	XMC_SPI_CH_SetSlaveSelectDelay(SPI_CH, 2U);
	/* Enable the Slave Select 0 Signal */
	XMC_SPI_CH_EnableSlaveSelect(SPI_CH, XMC_SPI_CH_SLAVE_SELECT_2);

	XMC_SPI_CH_SetBitOrderLsbFirst(SPI_CH);

	//XMC_SPI_CH_DataLatchedInTrailingEdge(SPI_CH);

	XMC_SPI_CH_DataLatchedInLeadingEdge(SPI_CH);

	XMC_USIC_CH_TXFIFO_Configure(SPI_CH, 0U, XMC_USIC_CH_FIFO_SIZE_8WORDS, 0U);

	XMC_SPI_CH_Start(SPI_CH);

	XMC_GPIO_SetOutputHigh(CS);
	XMC_GPIO_SetMode(CS, XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P1_7_AF_U1C1_SELO2);
	XMC_GPIO_SetMode(SCLK, XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P1_8_AF_U1C1_SCLKOUT);
	XMC_GPIO_SetMode(MOSI, XMC_GPIO_MODE_OUTPUT_PUSH_PULL | P1_9_AF_U1C1_DOUT0);
}

uint16_t spi_write_buffer(uint8_t *data , uint16_t length)
{
 uint16_t i;

	for(i=0;i<length;i++)
	{
		spi_write(data[i]);
	}
}



