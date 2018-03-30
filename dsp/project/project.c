
#include "DSP2833x_Device.h"

// Prototype statements for functions found within this file.
void Gpio_select(void);
extern void InitSysCtrl(void);
interrupt void cpu_timer0_isr(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitCpuTimers(void);
extern void ConfigCpuTimer(struct CPUTIMER_VARS *, float, float);
extern void display_ADC(unsigned int);
extern void InitAdc(void);
interrupt void adc_isr(void);
void Rakam(int, int);

float lm35;
int temp;
int sum;
//###########################################################################
//                      main code
//###########################################################################
void main(void)
{

    // binary counter for digital output

    InitSysCtrl();      // Basic Core Initialization
    EALLOW;
    SysCtrlRegs.WDCR = 0x00AF;
    EDIS;


    DINT;               // Disable all interrupts

    Gpio_select();

    InitPieCtrl();

    InitPieVectTable();

    InitAdc();

    AdcRegs.ADCTRL1.all = 0;
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;    // Dual Sequencer Mode
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0;   // Single Run Mode
    AdcRegs.ADCTRL1.bit.ACQ_PS = 7;     // 8 x ADC-Clock
    AdcRegs.ADCTRL1.bit.CPS = 0;        // divide by 1

    AdcRegs.ADCTRL2.all = 0;
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;     // ePWM_SOCA trigger
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1; // enable ADC int for seq1
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0; // interrupt after every EOS

    AdcRegs.ADCTRL3.bit.ADCCLKPS = 3;       // set FCLK to 12.5 MHz

    AdcRegs.ADCMAXCONV.all = 0x0002; // 2 conversions

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0; // 1st channel ADCINA0
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 2;
    EPwm2Regs.TBCTL.all = 0xC030;   // Configure timer control register


    EPwm2Regs.TBPRD = 2999; // TPPRD +1  =  TPWM / (HSPCLKDIV * CLKDIV * TSYSCLK)
                            //           =  20 µs / 6.667 ns
    EPwm2Regs.ETPS.all = 0x0100;            // Configure ADC start by ePWM2

    EPwm2Regs.ETSEL.all = 0x0A00;           // Enable SOCA to ADC

    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;
    PieVectTable.ADCINT = &adc_isr;
    EDIS;
    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0, 150, 100000);

    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    IER |= 1;
    EINT;
    ERTM;
    CpuTimer0Regs.TCR.bit.TSS = 0;

    while(1)
    {
          while(CpuTimer0.InterruptCount <10)
          {
               EALLOW;
               SysCtrlRegs.WDKEY = 0x55;   // service WD #1
               SysCtrlRegs.WDKEY = 0xAA;   // service WD #2
               EDIS;
           }
          int i = 0;
          sum = 0;
          for( i=0; i<100; i++ ){
              sum += temp;
          }
           temp = sum/100;
           int a,b;
           a = temp/10;
           b = temp%10;
           Rakam(a, b);

           CpuTimer0.InterruptCount = 0;

}}


void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all = 0;       // GPIO15 ... GPIO0 = General Puropse I/O
    GpioCtrlRegs.GPAMUX2.all = 0;       // GPIO31 ... GPIO16 = General Purpose I/O
    GpioCtrlRegs.GPBMUX1.all = 0;       // GPIO47 ... GPIO32 = General Purpose I/O
    GpioCtrlRegs.GPBMUX2.all = 0;       // GPIO63 ... GPIO48 = General Purpose I/O
    GpioCtrlRegs.GPCMUX1.all = 0;       // GPIO79 ... GPIO64 = General Purpose I/O
    GpioCtrlRegs.GPCMUX2.all = 0;       // GPIO87 ... GPIO80 = General Purpose I/O

    GpioCtrlRegs.GPADIR.all = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO26= 1;
    GpioCtrlRegs.GPADIR.bit.GPIO27= 1;
    GpioCtrlRegs.GPADIR.bit.GPIO28= 1;
    GpioCtrlRegs.GPADIR.bit.GPIO29= 1;
    GpioCtrlRegs.GPADIR.bit.GPIO30= 1;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioCtrlRegs.GPBDIR.all = 0;        // GPIO63-32 as inputs

    GpioCtrlRegs.GPCDIR.all = 0;        // GPIO87-64 as inputs
    EDIS;
}
interrupt void cpu_timer0_isr(void){
    CpuTimer0.InterruptCount++;
    EALLOW;
    SysCtrlRegs.WDKEY = 0xAA;
    EDIS;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
interrupt void adc_isr(void){

    lm35 = AdcMirror.ADCRESULT0;
    lm35 = lm35*300/4095;
    temp = lm35-8;

    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

void Rakam(int onlar, int birler)
{
   switch(birler)
   {
   case 0 :
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPASET.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;

       break;
   case 1:
       GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
       break;
   case 2:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPASET.bit.GPIO5 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 3:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 4:
       GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 5:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 6:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPASET.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 7:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
       break;
   case 8:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPASET.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   case 9:
       GpioDataRegs.GPASET.bit.GPIO1 = 1;
       GpioDataRegs.GPASET.bit.GPIO2 = 1;
       GpioDataRegs.GPASET.bit.GPIO3 = 1;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
       GpioDataRegs.GPASET.bit.GPIO6 = 1;
       GpioDataRegs.GPASET.bit.GPIO7 = 1;
       break;
   }

   switch(onlar)
   {
   case 0 :
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPASET.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
       break;
   case 1:
       GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
       break;
   case 2:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPASET.bit.GPIO29 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 3:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 4:
       GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 5:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 6:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPASET.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 7:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
       break;
   case 8:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPASET.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   case 9:
       GpioDataRegs.GPASET.bit.GPIO25 = 1;
       GpioDataRegs.GPASET.bit.GPIO26 = 1;
       GpioDataRegs.GPASET.bit.GPIO27 = 1;
       GpioDataRegs.GPASET.bit.GPIO28 = 1;
       GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;
       GpioDataRegs.GPASET.bit.GPIO30 = 1;
       GpioDataRegs.GPASET.bit.GPIO31 = 1;
       break;
   }
}
//===========================================================================
// End of SourceCode.
//===========================================================================

