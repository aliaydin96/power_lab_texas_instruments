#include "DSP2833x_Device.h"

void display_ADC(int result)
/* show the 12-bit "result" of the AD-conversion at 4 LEDs (GPIO49,34,11 and 9)	*/
/* the result will be show as light-beam								*/
{
	switch((result>>8)/3) 
	{
		case 4:
		{
			GpioDataRegs.GPBSET.bit.GPIO49 = 1;
			GpioDataRegs.GPBSET.bit.GPIO34 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			break;
		}
		case 3:
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
			GpioDataRegs.GPBSET.bit.GPIO34 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			break;
		}
		case 2:
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
			GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			break;
		}
		case 1:
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
			GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			break;
		}
		case 0:
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
			GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			break;
		}
		default:
		{
			GpioDataRegs.GPBSET.bit.GPIO49 = 1;
			GpioDataRegs.GPBSET.bit.GPIO34 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
		}
	}
}




