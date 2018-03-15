//
//      Lab5_1: TMS320F28335
//      (c) Frank Bormann
//
//###########################################################################
//
// FILE:    Lab5_1.c
//
// TITLE:   DSP28335ControlCARD; Digital Output
//          4 - bit - counter at 4 LEDs LD1(GPIO9), LD2(GPIO11), LD3(GPIO34)
//          and LD4 (GPIO49)
//          software delay loop; watchdog disabled
//          template file for Lab5_1
//###########################################################################
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  3.0 | 02 May 2008 | F.B. | Lab5_1 for F28335;
//  3.1 | 06 Nov 2009 | F.B  | Lab5_1 for F28335 and PE revision5
//###########################################################################
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


int voltage_vr1;
int counter=0;
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

    Gpio_select();      // GPIO9,GPIO11,GPIO34 and GPIO49 as output (LEDs @ peripheral explorer)

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

    AdcRegs.ADCMAXCONV.all = 0x0001; // 2 conversions

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0; // 1st channel ADCINA0
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1; // 2nd channel ADCINA1

    EPwm2Regs.TBCTL.all = 0xC030;   // Configure timer control register
    /*
     bit 15-14     11:     FREE/SOFT, 11 = ignore emulation suspend
     bit 13        0:      PHSDIR, 0 = count down after sync event
     bit 12-10     000:    CLKDIV, 000 => TBCLK = HSPCLK/1
     bit 9-7       000:    HSPCLKDIV, 000 => HSPCLK = SYSCLKOUT/1
     bit 6         0:      SWFSYNC, 0 = no software sync produced
     bit 5-4       11:     SYNCOSEL, 11 = sync-out disabled
     bit 3         0:      PRDLD, 0 = reload PRD on counter=0
     bit 2         0:      PHSEN, 0 = phase control disabled
     bit 1-0       00:     CTRMODE, 00 = count up mode
    */

    EPwm2Regs.TBPRD = 2999; // TPPRD +1  =  TPWM / (HSPCLKDIV * CLKDIV * TSYSCLK)
                            //           =  20 µs / 6.667 ns

    EPwm2Regs.ETPS.all = 0x0100;            // Configure ADC start by ePWM2
    /*
     bit 15-14     00:     EPWMxSOCB, read-only
     bit 13-12     00:     SOCBPRD, don't care
     bit 11-10     00:     EPWMxSOCA, read-only
     bit 9-8       01:     SOCAPRD, 01 = generate SOCA on first event
     bit 7-4       0000:   reserved
     bit 3-2       00:     INTCNT, don't care
     bit 1-0       00:     INTPRD, don't care
    */

    EPwm2Regs.ETSEL.all = 0x0A00;           // Enable SOCA to ADC
    /*
     bit 15        0:      SOCBEN, 0 = disable SOCB
     bit 14-12     000:    SOCBSEL, don't care
     bit 11        1:      SOCAEN, 1 = enable SOCA
     bit 10-8      010:    SOCASEL, 010 = SOCA on PRD event
     bit 7-4       0000:   reserved
     bit 3         0:      INTEN, 0 = disable interrupt
     bit 2-0       000:    INTSEL, don't care
    */


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
          while(CpuTimer0.InterruptCount == 0)
           {
               EALLOW;
               SysCtrlRegs.WDKEY = 0x55;   // service WD #1
               SysCtrlRegs.WDKEY = 0xAA;   // service WD #2
               EDIS;
           }
           CpuTimer0.InterruptCount = 0;
           ConfigCpuTimer(&CpuTimer0,100,20000 + voltage_vr1 * 239.32);
           CpuTimer0Regs.TCR.bit.TSS = 0;  //  restart timer0
           counter++;
           // place your code to analyze counter here
           // if bit 0 of counter = 1, set GPIO9 to 1
           // if bit 1 of counter = 1, set GPIO11 to 1
           // if bit 2 of counter = 1, set GPIO34 to 1
           // if bit 3 of counter = 1, set GPIO49 to 1
           if(counter&1) GpioDataRegs.GPASET.bit.GPIO9 = 1;
           else GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
           if(counter&2) GpioDataRegs.GPASET.bit.GPIO11 = 1;
           else GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
           if(counter&4) GpioDataRegs.GPBSET.bit.GPIO34 = 1;
           else GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
           if(counter&8) GpioDataRegs.GPBSET.bit.GPIO49 = 1;
           else GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

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
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;  // peripheral explorer: LED LD1 at GPIO9
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1; // peripheral explorer: LED LD2 at GPIO11

    GpioCtrlRegs.GPBDIR.all = 0;        // GPIO63-32 as inputs
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1; // peripheral explorer: LED LD3 at GPIO34
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; // peripheral explorer: LED LD4 at GPIO49
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
    voltage_vr1 = AdcMirror.ADCRESULT0;

    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}
//===========================================================================
// End of SourceCode.
//===========================================================================
