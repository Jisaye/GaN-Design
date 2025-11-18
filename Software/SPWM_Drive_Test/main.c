 
#include "DSP2833x_Project.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DELAY 35.700L
#if (CPU_FRQ_150MHZ)
  #define CPU_CLK   150e6     // 150MHZ
#endif
#if (CPU_FRQ_100MHZ)
  #define CPU_CLK   100e6		// 100MHZ
#endif
#define PWM_CLK   100e3                // PWM frequency 10KHZ
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, Timebase = SYSCLKOUT

interrupt void epwm1_isr(void);
interrupt void epwm2_isr(void);
void StartEPwm(void);
	 
Uint32 LoopCount;
int i,k=0;
float M=0.67;               //璋冨埗姣�
int N=2000;                 //閲囨牱鐐规暟
float sina[];        //姝ｅ鸡娉㈤噰鏍峰�兼暟缁�
float sinb[];        //姝ｅ鸡娉㈤噰鏍峰�兼暟缁�

void main(void)
{
   InitSysCtrl();
   asm(" RPT #8 || NOP");
   DINT;

   EALLOW;	// This is needed to write to EALLOW protected registers
   InitEPwm1Gpio();
   InitEPwm2Gpio();
   EDIS;

 
   for(k=0;k<N;k++)
   {
	   sina[k]=sin(2.0*3.1416*(float)k/(float)N);  //璁＄畻256涓噰鏍风偣鐨勬寮﹀��
	   sinb[k]=sin(2.0*3.1416*(float)k/(float)N); //璁＄畻256涓噰鏍风偣鐨勬寮﹀�硷紝绉荤浉90搴�
   }

   DINT;
  InitPieCtrl();
// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;
   InitPieVectTable();


   EALLOW;
   PieVectTable.EPWM1_INT = &epwm1_isr;
   PieVectTable.EPWM2_INT = &epwm2_isr;
   EDIS;

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;   // Disable clock synchronization
   EDIS;

   StartEPwm();
//閰嶇疆EPWM鏃惰鎶婂悓姝ユ椂閽熷厛鍏抽棴
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;	// Enable clock synchronization
   EDIS;
   // Enable CPU INT3 which is connected to EPWM1-3 INT:
   IER |= M_INT3;
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;   // Enable EPWM1 INT in the PIE: Group 3 interrupt 1
   PieCtrlRegs.PIEIER3.bit.INTx2 = 1;   // Enable EPWM2 INT in the PIE: Group 3 interrupt 1
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM
   for(;;)
   {
	  __asm(" NOP");
   }

}



//------------------------------------------------------------------------------------
void StartEPwm()
{  
    EALLOW;
	//EPWM1 SET
   	EPwm1Regs.TBSTS.all=0;					// 娓呴櫎鏃跺熀鐘舵�佸瘎瀛樺櫒锛堝璁℃暟鏂瑰悜銆佹孩鍑烘爣蹇楃瓑锛�
	EPwm1Regs.TBPHS.half.TBPHS=0;			// 鏃跺熀鐩镐綅瀵勫瓨鍣ㄦ竻闆讹紙鏃犵浉浣嶅亸绉伙級
	EPwm1Regs.TBCTR=0;						// 鏃跺熀璁℃暟鍣ㄦ竻闆讹紙浠�0寮�濮嬭鏁帮級

	EPwm1Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm1Regs.CMPA.half.CMPA =0;	  
	EPwm1Regs.CMPB=0;					

	EPwm1Regs.AQCTLA.bit.CAU=1;          // EPWMxA = 1 when CTR=CMPA and counter inc锛孍PWMxA = 0 when CTR=CMPA and counter dec
	EPwm1Regs.AQCTLA.bit.CAD=2;				
	EPwm1Regs.AQCTLB.bit.CAU=1;          // EPWMxB = 1 when CTR=CMPB and counter inc
	EPwm1Regs.AQCTLB.bit.CAD=2;          // EPWMxB = 0 when CTR=CMPB and counter dec

	EPwm1Regs.DBCTL.bit.OUT_MODE=3;
	EPwm1Regs.DBCTL.bit.POLSEL=2;			// EPWMxA and EPWMxB are complementary          
	EPwm1Regs.DBCTL.bit.IN_MODE=0;
	EPwm1Regs.DBRED=15;					
	EPwm1Regs.DBFED=15;					



	EPwm1Regs.ETSEL.bit.INTSEL=1;      // Select INT on Time Base Counter equal to zero 
	EPwm1Regs.ETSEL.bit.INTEN=1;       // Enable INT  
	EPwm1Regs.ETPS.bit.INTPRD=1;       // Generate INT on first event

	EPwm1Regs.PCCTL.all=0;				// 绂佺敤澶栬鎺у埗鍔熻兘

	EPwm1Regs.TBCTL.bit.CTRMODE=2;			// Enable Timer
	EPwm1Regs.TBCTL.bit.PHSEN=0;			// 绂佺敤鐩镐綅鍔犺浇
	EPwm1Regs.TBCTL.bit.HSPCLKDIV=0;		// 楂橀�熸椂闂撮鍒嗛鍣ㄨ缃�
	EPwm1Regs.TBCTL.bit.CLKDIV=0;			// 鏃堕棿鍩烘椂閽熼鍒嗛鍣ㄨ缃�
	EPwm1Regs.TBPRD=SP;								// 璁剧疆鏃跺熀鍛ㄦ湡瀵勫瓨鍣ㄨ鏁板�间负SP锛孴BPRD=SP
	// Setup shadowing
	EPwm1Regs.CMPCTL.bit.SHDWAMODE=0;		// CMPA瀵勫瓨鍣ㄧ珛鍗冲姞杞�
	EPwm1Regs.CMPCTL.bit.SHDWBMODE=0;		// CMPB瀵勫瓨鍣ㄧ珛鍗冲姞杞�
	EPwm1Regs.CMPCTL.bit.LOADAMODE=2;		
	EPwm1Regs.CMPCTL.bit.LOADBMODE=2;		

	//EPWM2 SET
	EPwm2Regs.TBSTS.all=0;					// 娓呴櫎鏃跺熀鐘舵�佸瘎瀛樺櫒锛堝璁℃暟鏂瑰悜銆佹孩鍑烘爣蹇楃瓑锛�
	EPwm2Regs.TBPHS.half.TBPHS=0;			// 鏃跺熀鐩镐綅瀵勫瓨鍣ㄦ竻闆讹紙鏃犵浉浣嶅亸绉伙級
	EPwm2Regs.TBCTR=0;						// 鏃跺熀璁℃暟鍣ㄦ竻闆讹紙浠�0寮�濮嬭鏁帮級

	EPwm2Regs.CMPCTL.all=0x50;        // Immediate mode for CMPA and CMPB
	EPwm2Regs.CMPA.half.CMPA =0;	  
	EPwm2Regs.CMPB=0;					

	EPwm2Regs.AQCTLA.bit.CAU=2;          // EPWMxA = 1 when CTR=CMPA and counter inc锛孍PWMxA = 0 when CTR=CMPA and counter dec
	EPwm2Regs.AQCTLA.bit.CAD=1;
	EPwm2Regs.AQCTLB.bit.CAU=2;          // EPWMxB = 1 when CTR=CMPB and counter inc
	EPwm2Regs.AQCTLB.bit.CAD=1;          // EPWMxB = 0 when CTR=CMPB and counter dec

	EPwm2Regs.DBCTL.bit.OUT_MODE=3;
	EPwm2Regs.DBCTL.bit.POLSEL=2;			// EPWMxA and EPWMxB are complementary
	EPwm2Regs.DBCTL.bit.IN_MODE=0;
	EPwm2Regs.DBRED=15;
	EPwm2Regs.DBFED=15;



	EPwm2Regs.ETSEL.bit.INTSEL=1;      // Select INT on Time Base Counter equal to zero 
	EPwm2Regs.ETSEL.bit.INTEN=1;       // Enable INT  
	EPwm2Regs.ETPS.bit.INTPRD=1;       // Generate INT on first event

	EPwm2Regs.PCCTL.all=0;				// 绂佺敤澶栬鎺у埗鍔熻兘

	EPwm2Regs.TBCTL.bit.CTRMODE=2;			// Enable Timer
	EPwm2Regs.TBCTL.bit.PHSEN=0;			// 绂佺敤鐩镐綅鍔犺浇
	EPwm2Regs.TBCTL.bit.HSPCLKDIV=0;		// 楂橀�熸椂闂撮鍒嗛鍣ㄨ缃�
	EPwm2Regs.TBCTL.bit.CLKDIV=0;			// 鏃堕棿鍩烘椂閽熼鍒嗛鍣ㄨ缃�
	EPwm2Regs.TBPRD=SP;								// 璁剧疆鏃跺熀鍛ㄦ湡瀵勫瓨鍣ㄨ鏁板�间负SP锛孴BPRD=SP
	// Setup shadowing
	EPwm2Regs.CMPCTL.bit.SHDWAMODE=0;		// CMPA瀵勫瓨鍣ㄧ珛鍗冲姞杞�
	EPwm2Regs.CMPCTL.bit.SHDWBMODE=0;		// CMPB瀵勫瓨鍣ㄧ珛鍗冲姞杞�
	EPwm2Regs.CMPCTL.bit.LOADAMODE=2;		
	EPwm2Regs.CMPCTL.bit.LOADBMODE=2;	
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;	// Enable clock synchronization
	EDIS;

}
 
//------------------------------------------------------------------------------------
interrupt void epwm1_isr(void)
{
	EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD*((1.0+M*sina[i])/2.0); //鏇存柊CMPA瀵勫瓨鍣ㄥ��
	EPwm1Regs.CMPB = EPwm1Regs.TBPRD*((1.0+M*sinb[i])/2.0);   //鏇存柊CMPB瀵勫瓨鍣ㄥ��
	i++;
	if(i>=N)
	{
		i=0;
	}
   // Clear INT flag for this timer
   EPwm1Regs.ETCLR.bit.INT=1;
   // Acknowledge this interrupt to receive more interrupts from group 3
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
//------------------------------------------------------------------------------------
interrupt void epwm2_isr(void)
{
	EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.TBPRD*((1.0+M*sina[i])/2.0); //鏇存柊CMPA瀵勫瓨鍣ㄥ��
	EPwm2Regs.CMPB = EPwm2Regs.TBPRD*((1.0+M*sinb[i])/2.0);   //鏇存柊CMPB瀵勫瓨鍣ㄥ��
	i++;
	if(i>=N)
	{
		i=0;
	}
   // Clear INT flag for this timer
   EPwm2Regs.ETCLR.bit.INT=1;
   // Acknowledge this interrupt to receive more interrupts from group 3
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}
