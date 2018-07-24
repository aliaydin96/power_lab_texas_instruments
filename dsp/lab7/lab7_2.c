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
extern void InitSysCtrl(void);
interrupt void cpu_timer0_isr(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitCpuTimers(void);
extern void ConfigCpuTimer(struct CPUTIMER_VARS *, float, float);

void Setup_ePWM(void);
//###########################################################################
//                      main code
//###########################################################################
void main(void)
{


    int counter=0;      // binary counter for digital output

        InitSysCtrl();      // Basic Core Initialization
        EALLOW;
        SysCtrlRegs.WDCR = 0x00AF;
        EDIS;


        DINT;               // Disable all interrupts

        Gpio_select();      // GPIO9,GPIO11,GPIO34 and GPIO49 as output (LEDs @ peripheral explorer)
        Setup_ePWM();
        InitPieCtrl();

        InitPieVectTable();

        EALLOW;
        PieVectTable.TINT0 = &cpu_timer0_isr;
        EDIS;
        InitCpuTimers();

        ConfigCpuTimer(&CpuTimer0, 150, 100000);

        PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
        IER |= 1;
        EINT;
        ERTM;
        CpuTimer0Regs.TCR.bit.TSS = 0;

        while(1)
        {

                while(CpuTimer0.InterruptCount == 0);
                CpuTimer0.InterruptCount = 0;

                EALLOW;
                SysCtrlRegs.WDKEY = 0x55;   // service WD #1
                EDIS;
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



        }
}


void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all = 0;       // GPIO15 ... GPIO0 = General Puropse I/O
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // ePWM1A active
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; // ePWM2A active
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1; // ePWM3A active
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

void Setup_ePWM(void){
    EPwm1Regs.TBCTL.all = 0;
    EPwm1Regs.TBCTL.bit.CLKDIV = 7; // CLKDIV = 128
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 7;  //HPSCLKDIV = 14
    EPwm1Regs.TBCTL.bit.CTRMODE = 2;  // Count up and down operation (10) = 2
    EPwm1Regs.AQCTLA.all = 0x0006;
    EPwm1Regs.TBPRD = 41853;
    /*  TBPRD = 0.5*fcpu/(fpwm*CLKDIV*HSPCLKDIV)
     * fcpu = 150 MHz , fpwm = 1 KHz

     */
    EPwm2Regs.TBCTL.all = 0;
    EPwm2Regs.TBCTL.bit.CLKDIV = 7;   // CLKDIV = 128
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 7; //HPSCLKDIV = 14
    EPwm2Regs.TBCTL.bit.CTRMODE = 2;  // Count up and down operation (10) = 2
    EPwm2Regs.AQCTLA.all = 0x0006;
    EPwm2Regs.TBPRD = 41853;


    EPwm3Regs.TBCTL.all = 0;
    EPwm3Regs.TBCTL.bit.CLKDIV = 7;
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 7;
    EPwm3Regs.TBCTL.bit.CTRMODE = 2;  // Count up and down operation (10) = 2
    EPwm3Regs.AQCTLA.all = 0x0006;
    EPwm3Regs.TBPRD = 41853;

    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;   // GENERATE A SIGNAL IF CTR =0

    EPwm2Regs.TBCTL.bit.PHSEN = 1; // PHASE ENABLE FOR EPWM2A
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;  // SYNIN = SYNCOUT
    EPwm2Regs.TBPHS.half.TBPHS = 10000; // 1/3 PHASE SHIFT

    EPwm3Regs.TBCTL.bit.PHSEN = 1; // PHASE ENABLE FOR EPWM3
    EPwm3Regs.TBPHS.half.TBPHS = 20000;  // 2/3 PHASE SHIFT
    // I observed phase shift by using three leds

}
//===========================================================================
// End of SourceCode.
//===========================================================================
