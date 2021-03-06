/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : FC321.c
**     Project     : SOLINDAR
**     Processor   : MC9S08QE128CLK
**     Component   : FreeCntr32
**     Version     : Component 02.070, Driver 01.22, CPU db: 3.00.067
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2019-11-11, 12:11, # CodeGen: 0
**     Abstract    :
**         This device "FreeCntr32" implements 32-bit Free Running Counter
**     Settings    :
**         Timer name                  : TPM1 (16-bit)
**         Compare name                : TPM11
**         Counter shared              : Yes
**
**         High speed mode
**             Prescaler               : divide-by-16
**             Clock                   : 1048576 Hz
**           Resolution of timer
**             microseconds            : 10
**             seconds (real)          : 0.000009536743
**             Hz                      : 104858
**             kHz                     : 105
**
**         Initialization:
**              Timer                  : Enabled
**
**         Timer registers
**              Counter                : TPM1CNT   [$0041]
**              Mode                   : TPM1SC    [$0040]
**              Run                    : TPM1SC    [$0040]
**              Prescaler              : TPM1SC    [$0040]
**
**         Compare registers
**              Compare                : TPM1C1V   [$0049]
**     Contents    :
**         Enable    - byte FC321_Enable(void);
**         Disable   - byte FC321_Disable(void);
**         Reset     - byte FC321_Reset(void);
**         GetTimeUS - byte FC321_GetTimeUS(word *Time);
**         GetTimeMS - byte FC321_GetTimeMS(word *Time);
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
** @file FC321.c
** @version 01.22
** @brief
**         This device "FreeCntr32" implements 32-bit Free Running Counter
*/         
/*!
**  @addtogroup FC321_module FC321 module documentation
**  @{
*/         


/* MODULE FC321. */

#include "PE_Error.h"
#include "FC321.h"

#pragma MESSAGE DISABLE C4002          /* Disable warning C4002 "Result not used" */


static bool EnUser;                    /* Enable/Disable device by user */
static dword TTicks;                   /* Counter of timer ticks */
static dword LTicks;                   /* Working copy of variable TTicks */
static bool TOvf;                      /* Counter overflow flag */
static bool LOvf;                      /* Working copy of variable TOvf */
static word CmpVal;                    /* Value added to compare register in ISR */

/*** Internal macros and method prototypes ***/

/*
** ===================================================================
**     Method      :  SetCV (component FreeCntr32)
**
**     Description :
**         The method computes and sets compare eventually modulo value 
**         for time measuring.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#define FC321_SetCV(_Val) ( \
  TPM1C1V = (CmpVal = (word)(_Val)) )

/*
** ===================================================================
**     Method      :  LoadTicks (component FreeCntr32)
**
**     Description :
**         The method loads actual number of timer ticks and actual state 
**         of overflow flag.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void LoadTicks(void);
/*
** ===================================================================
**     Method      :  HWEnDi (component FreeCntr32)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void);

/*** End of Internal methods declarations ***/

/*
** ===================================================================
**     Method      :  LoadTicks (component FreeCntr32)
**
**     Description :
**         The method loads actual number of timer ticks and actual state 
**         of overflow flag.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void LoadTicks(void)
{
  EnterCritical();                     /* Save the PS register */
  LTicks = TTicks;                     /* Loading actual number of timer ticks */
  LOvf = TOvf;                         /* Loading actual state of "overflow flag" */
  ExitCritical();                      /* Restore the PS register */
}

/*
** ===================================================================
**     Method      :  HWEnDi (component FreeCntr32)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the 
**         component. The method is called automatically as a part of the 
**         Enable and Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  word TmpCmpVal;                      /* Temporary variable for new compare value */

  if (EnUser) {
    TmpCmpVal = (word)(TPM1CNT + CmpVal); /* Count current value for the compare register */
    TPM1C1V = TmpCmpVal;               /* Set compare register */
    while (TPM1C1V != TmpCmpVal) {}    /* Wait for register update (because of Latching mechanism) */
    /* TPM1C1SC: CH1F=0 */
    clrReg8Bits(TPM1C1SC, 0x80U);      /* Reset request flag */ 
    /* TPM1C1SC: CH1IE=1 */
    setReg8Bits(TPM1C1SC, 0x40U);      /* Enable compare interrupt */ 
  } else {
    /* TPM1C1SC: CH1IE=0 */
    clrReg8Bits(TPM1C1SC, 0x40U);      /* Disable Compare interrupt */ 
  }
}

/*
** ===================================================================
**     Method      :  FC321_Enable (component FreeCntr32)
*/
/*!
**     @brief
**         This method enables the component - the internal clocks are
**         counted.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte FC321_Enable(void)
{
  if (!EnUser) {                       /* Is the device disabled by user? */
    EnUser = TRUE;                     /* If yes then set the flag "device enabled" */
    HWEnDi();                          /* Enable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  FC321_Disable (component FreeCntr32)
*/
/*!
**     @brief
**         This method disables the component - the internal clocks are
**         not counted.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte FC321_Disable(void)
{
  if (EnUser) {                        /* Is the device enabled by user? */
    EnUser = FALSE;                    /* If yes then set the flag "device disabled" */
    HWEnDi();                          /* Disable the device */
  }
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  FC321_Reset (component FreeCntr32)
*/
/*!
**     @brief
**         This method clears the counter.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
*/
/* ===================================================================*/
byte FC321_Reset(void)
{
  EnterCritical();
  HWEnDi();                            /* Reset compare settings */
  TTicks =  0x00U;                     /* Reset counter of timer ticks */
  TOvf = FALSE;                        /* Reset counter overflow flag */
  ExitCritical();                      /* Restore the PS register */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  FC321_GetTimeUS (component FreeCntr32)
**     Description :
**         Returns the time (as a 16-bit unsigned integer) in microseconds
**         since the last resetting after the last reset.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Time            - A pointer to the returned 16-bit value
**                           in microseconds
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_OVERFLOW - Software counter overflow
**                           ERR_MATH - Overflow during evaluation
** ===================================================================
*/
byte FC321_GetTimeUS(word *Time)
{
  dlong RtVal;                         /* Result of two 32-bit numbers multiplication */

  LoadTicks();                         /* Load actual state of counter */
  if (LOvf) {                          /* Testing counter overflow */
    return ERR_OVERFLOW;               /* If yes then error */
  }
  PE_Timer_LngMul((dword)LTicks, 0x09896800UL, &RtVal); /* Multiply timer ticks and High speed CPU mode coefficient */
  if (PE_Timer_LngHi3(RtVal[0], RtVal[1], Time)) { /* Get result value into word variable */
    return ERR_MATH;                   /* Overflow, value too big */
  } else {
    return ERR_OK;                     /* OK: Value calculated */
  }
}

/*
** ===================================================================
**     Method      :  FC321_GetTimeMS (component FreeCntr32)
**     Description :
**         Returns the time (as a 16-bit unsigned integer) in milliseconds
**         since the last resetting after the last reset.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Time            - A pointer to the returned 16-bit value
**                           in milliseconds
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_OVERFLOW - Software counter overflow
**                           ERR_MATH - Overflow during evaluation
** ===================================================================
*/
byte FC321_GetTimeMS(word *Time)
{
  dlong RtVal;                         /* Result of two 32-bit numbers multiplication */

  LoadTicks();                         /* Load actual state of counter */
  if (LOvf) {                          /* Testing counter overflow */
    return ERR_OVERFLOW;               /* If yes then error */
  }
  PE_Timer_LngMul((dword)LTicks, 0x02710000UL, &RtVal); /* Multiply timer ticks and High speed CPU mode coefficient */
  if (PE_Timer_LngHi4(RtVal[0], RtVal[1], Time)) { /* Get result value into word variable */
    return ERR_MATH;                   /* Overflow, value too big */
  } else {
    return ERR_OK;                     /* OK: Value calculated */
  }
}

/*
** ===================================================================
**     Method      :  FC321_Init (component FreeCntr32)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void FC321_Init(void)
{
  /* TPM1C1SC: CH1F=0,CH1IE=0,MS1B=0,MS1A=1,ELS1B=0,ELS1A=0,??=0,??=0 */
  setReg8(TPM1C1SC, 0x10U);            /* Set output compare mode and disable compare interrupt */ 
  EnUser = TRUE;                       /* Enable device */
  TTicks = 0U;                         /* Counter of timer ticks */
  TOvf = FALSE;                        /* Counter overflow flag */
  FC321_SetCV(0x0AU);                  /* Initialize appropriate value to the compare/modulo/reload register */
  HWEnDi();
}

/*
** ===================================================================
**     Method      :  FC321_Interrupt (component FreeCntr32)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes the component event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
ISR(FC321_Interrupt)
{
  /* TPM1C1SC: CH1F=0 */
  clrReg8Bits(TPM1C1SC, 0x80U);        /* Reset compare interrupt request flag */ 
  TPM1C1V += CmpVal;                   /* Count and save new value to the compare register (counter is shared)*/
  if (++TTicks == 0x00U) {             /* Increment #ticks, check overflow */
    TOvf = TRUE;                       /* If yes then set overflow flag */
  }
}



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
