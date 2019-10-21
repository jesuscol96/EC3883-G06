/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : TI2.c
**     Project     : SOLINDAR
**     Processor   : MC9S08QE128CLK
**     Component   : TimerInt
**     Version     : Component 02.161, Driver 01.23, CPU db: 3.00.067
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2019-10-21, 15:08, # CodeGen: 68
**     Abstract    :
**         This component "TimerInt" implements a periodic interrupt.
**         When the component and its events are enabled, the "OnInterrupt"
**         event is called periodically with the period that you specify.
**         TimerInt supports also changing the period in runtime.
**         The source of periodic interrupt can be timer compare or reload
**         register or timer-overflow interrupt (of free running counter).
**     Settings    :
**         Timer name                  : TPM1 (16-bit)
**         Compare name                : TPM12
**         Counter shared              : Yes
**
**         High speed mode
**             Prescaler               : divide-by-16
**             Clock                   : 1048576 Hz
**           Initial period/frequency
**             Xtal ticks              : 1638
**             microseconds            : 50000
**             milliseconds            : 50
**             seconds (real)          : 0.050000190735
**             Hz                      : 20
**
**         Runtime setting             : none
**
**         Initialization:
**              Timer                  : Enabled
**              Events                 : Enabled
**
**         Timer registers
**              Counter                : TPM1CNT   [$0041]
**              Mode                   : TPM1SC    [$0040]
**              Run                    : TPM1SC    [$0040]
**              Prescaler              : TPM1SC    [$0040]
**
**         Compare registers
**              Compare                : TPM1C2V   [$004C]
**
**         Flip-flop registers
**              Mode                   : TPM1C2SC  [$004B]
**     Contents    :
**         No public methods
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file TI2.c
** @version 01.23
** @brief
**         This component "TimerInt" implements a periodic interrupt.
**         When the component and its events are enabled, the "OnInterrupt"
**         event is called periodically with the period that you specify.
**         TimerInt supports also changing the period in runtime.
**         The source of periodic interrupt can be timer compare or reload
**         register or timer-overflow interrupt (of free running counter).
*/         
/*!
**  @addtogroup TI2_module TI2 module documentation
**  @{
*/         

/* MODULE TI2. */

#include "Events.h"
#include "PE_Error.h"
#include "TI2.h"
#pragma MESSAGE DISABLE C2705          /* WARNING C2705: Possible loss of data */
#pragma MESSAGE DISABLE C5919          /* WARNING C5919: Conversion of floating to unsigned integral */
#pragma MESSAGE DISABLE C5703          /* WARNING C5703: Parameter X declared in function F but not referenced */
#pragma MESSAGE DISABLE C4002          /* Disable warning C4002 "Result not used" */

static word CmpVal;                    /* Value added to compare register in ISR */
/*** Internal macros and method prototypes ***/

/*
** ===================================================================
**     Method      :  SetCV (component TimerInt)
**
**     Description :
**         The method computes and sets compare eventually modulo value 
**         for time measuring.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#define TI2_SetCV(_Val) ( \
  ((TPM1C2V = (word)(TPM1CNT + (_Val)),((CmpVal = (_Val))))))


/*** End of internal method prototypes ***/

/*
** ===================================================================
**     Method      :  TI2_Init (component TimerInt)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void TI2_Init(void)
{
  /* TPM1SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=0,PS2=0,PS1=0,PS0=0 */
  setReg8(TPM1SC, 0x00U);              /* Stop HW; disable overflow interrupt and set prescaler to 0 */ 
  /* TPM1MOD: BIT15=0,BIT14=0,BIT13=0,BIT12=0,BIT11=0,BIT10=0,BIT9=0,BIT8=0,BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  setReg16(TPM1MOD, 0x00U);            /* Clear modulo register: e.g. set free-running mode */ 
  /* TPM1C2SC: CH2F=0,CH2IE=1,MS2B=0,MS2A=1,ELS2B=0,ELS2A=0,??=0,??=0 */
  setReg8(TPM1C2SC, 0x50U);            /* Set output compare mode and enable compare interrupt */ 
  TI2_SetCV(0xCCCDU);                  /* Initialize appropriate value to the compare/modulo/reload register */
  /* TPM1CNTH: BIT15=0,BIT14=0,BIT13=0,BIT12=0,BIT11=0,BIT10=0,BIT9=0,BIT8=0 */
  setReg8(TPM1CNTH, 0x00U);            /* Reset HW Counter */ 
  /* TPM1SC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=1,PS1=0,PS0=0 */
  setReg8(TPM1SC, 0x0CU);              /* Set prescaler */ 
}


/*
** ===================================================================
**     Method      :  TI2_Interrupt (component TimerInt)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes the component event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
ISR(TI2_Interrupt)
{
  /* TPM1C2SC: CH2F=0 */
  clrReg8Bits(TPM1C2SC, 0x80U);        /* Reset compare interrupt request flag */ 
  TPM1C2V += CmpVal;                   /* Set new value to the compare register */
  TI2_OnInterrupt();                   /* Invoke user event */
}



/* END TI2. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
