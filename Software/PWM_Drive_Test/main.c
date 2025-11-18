 
#include "DSP2833x_Project.h"
#include <stdio.h>
#include <string.h>

#define DELAY 35.700L
#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6     // 150MHZ
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6		// 100MHZ
#endif
#define PWM_CLK   10e3                // PWM frequency 10KHZ
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, Timebase = SYSCLKOUT


void StartEPwm(void);
	 
Uint32 LoopCount;

void main(void)
{
   InitSysCtrl();
   asm(" RPT #8 || NOP");
   DINT;
 
   InitPieCtrl();
   DINT;

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;
   InitPieVectTable();
   asm(" RPT #8 || NOP");
   DINT;
   
   EALLOW;	// This is needed to write to EALLOW protected registers
  
   InitEPwm1Gpio();
   InitEPwm2Gpio();
   InitEPwm5Gpio();
  
   EDIS;
   DINT;  

   StartEPwm();

   
  while(1)
  { 
  
  }
}



//------------------------------------------------------------------------------------
void StartEPwm()
{  
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;   // Disable clock synchronization
   	EPwm1Regs.TBSTS.all=0;					// 
	EPwm1Regs.TBPHS.half.TBPHS=0;			// 
	EPwm1Regs.TBCTR=0;						// 

	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm1Regs.CMPA.half.CMPA =SP/2;	  // 
	EPwm1Regs.CMPB=0;					// 

	EPwm1Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLB.all=0;				// 
	EPwm1Regs.AQSFRC.all=0;				// 
	EPwm1Regs.AQCSFRC.all=0;			// 

	EPwm1Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm1Regs.DBRED=15;					// 
	EPwm1Regs.DBFED=15;					// 

	EPwm1Regs.TZSEL.all=0;				// 
	EPwm1Regs.TZCTL.all=0;				// 
	EPwm1Regs.TZEINT.all=0;				// 
	EPwm1Regs.TZFLG.all=0;				// 
	EPwm1Regs.TZCLR.all=0;				// 
	EPwm1Regs.TZFRC.all=0;				// 

	EPwm1Regs.ETSEL.all=0;              // 
	EPwm1Regs.ETFLG.all=0;				// 
	EPwm1Regs.ETCLR.all=0;				// 
	EPwm1Regs.ETFRC.all=0;				// 

	EPwm1Regs.PCCTL.all=0;				// 

	EPwm1Regs.TBCTL.all=0x0010+TBCTLVAL;			
	EPwm1Regs.TBPRD=SP;								
	
	// EPWM2 Configuration
	EPwm2Regs.TBSTS.all=0;
	EPwm2Regs.TBPHS.half.TBPHS=0;
	EPwm2Regs.TBCTR=0;

	EPwm2Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPA.half.CMPA =SP/2;
	EPwm2Regs.CMPB=0;

	EPwm2Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm2Regs.AQCTLB.all=0;
	EPwm2Regs.AQSFRC.all=0;
	EPwm2Regs.AQCSFRC.all=0;

	EPwm2Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm2Regs.DBRED=15;
	EPwm2Regs.DBFED=15;

	EPwm2Regs.TZSEL.all=0;
	EPwm2Regs.TZCTL.all=0;
	EPwm2Regs.TZEINT.all=0;
	EPwm2Regs.TZFLG.all=0;
	EPwm2Regs.TZCLR.all=0;
	EPwm2Regs.TZFRC.all=0;

	EPwm2Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm2Regs.ETFLG.all=0;
	EPwm2Regs.ETCLR.all=0;
	EPwm2Regs.ETFRC.all=0;

	EPwm2Regs.PCCTL.all=0;

	EPwm2Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm2Regs.TBPRD=SP; 

	// EPWM5 Configuration
	EPwm5Regs.TBSTS.all=0;
	EPwm5Regs.TBPHS.half.TBPHS=0;
	EPwm5Regs.TBCTR=0;

	EPwm5Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm5Regs.CMPA.half.CMPA =SP/2;
	EPwm5Regs.CMPB=0;

	EPwm5Regs.AQCTLA.all=0x60;        // EPWMxA = 1 when CTR=CMPA and counter inc
	                                  // EPWMxA = 0 when CTR=CMPA and counter dec
	EPwm5Regs.AQCTLB.all=0;
	EPwm5Regs.AQSFRC.all=0;
	EPwm5Regs.AQCSFRC.all=0;

	EPwm5Regs.DBCTL.all=0xb;          // EPWMxB is inverted
	EPwm5Regs.DBRED=15;
	EPwm5Regs.DBFED=15;

	EPwm5Regs.TZSEL.all=0;
	EPwm5Regs.TZCTL.all=0;
	EPwm5Regs.TZEINT.all=0;
	EPwm5Regs.TZFLG.all=0;
	EPwm5Regs.TZCLR.all=0;
	EPwm5Regs.TZFRC.all=0;

	EPwm5Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	EPwm5Regs.ETFLG.all=0;
	EPwm5Regs.ETCLR.all=0;
	EPwm5Regs.ETFRC.all=0;

	EPwm5Regs.PCCTL.all=0;

	EPwm5Regs.TBCTL.all=0x0010+TBCTLVAL;			// Enable Timer
	EPwm5Regs.TBPRD=SP; 

    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;	// Enable clock synchronization
	 EDIS;
}
 
//------------------------------------------------------------------------------------

 

