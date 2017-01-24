/**
  Section: Included Files
*/

#include <xc.h>
#include <stdio.h>
#include "tmr1.h"
#include "../lcd.h"
#include "tmr0.h"
#include "ext_int.h"
#include "pin_manager.h"

/**
  Section: Global Variable Definitions
*/
volatile uint16_t timer1ReloadVal;
void (*TMR1_InterruptHandler)(void);

extern uint16_t reading;
extern float result;
char units[2] = "pF";

extern uint8_t range, auto_range;

/**
  Section: TMR1 APIs
*/

void TMR1_Initialize(void)
{
    //Set the Timer to the options selected in the GUI

    //T1CKPS 1:8; T1OSCEN disabled; T1SYNC synchronize; TMR1CS FOSC/4; TMR1ON off; T1RD16 disabled; 
    T1CON = 0x30;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO done; T1GSPM disabled; 
    T1GCON = 0x00;

    //TMR1H 2; 
    TMR1H = 0x02;

    //TMR1L 24; 
    TMR1L = 0x18;

    // Load the TMR value to reload variable
    timer1ReloadVal=TMR1;

    // Clearing IF flag before enabling the interrupt.
    PIR1bits.TMR1IF = 0;

    // Enabling TMR1 interrupt.
    PIE1bits.TMR1IE = 1;

    // Set Default Interrupt Handler
    TMR1_SetInterruptHandler(TMR1_DefaultInterruptHandler);

    // Start TMR1
    TMR1_StartTimer();
}

void TMR1_StartTimer(void)
{
    // Start the Timer by writing to TMRxON bit
    T1CONbits.TMR1ON = 1;
}

void TMR1_StopTimer(void)
{
    // Stop the Timer by writing to TMRxON bit
    T1CONbits.TMR1ON = 0;
}

uint16_t TMR1_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValHigh;
    uint8_t readValLow;
    
    readValLow = TMR1L;
    readValHigh = TMR1H;
    
    readVal = ((uint16_t)readValHigh << 8) | readValLow;

    return readVal;
}

void TMR1_WriteTimer(uint16_t timerVal)
{
    if (T1CONbits.T1SYNC == 1)
    {
        // Stop the Timer by writing to TMRxON bit
        T1CONbits.TMR1ON = 0;

        // Write to the Timer1 register
        TMR1H = (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;

        // Start the Timer after writing to the register
        T1CONbits.TMR1ON =1;
    }
    else
    {
        // Write to the Timer1 register
        TMR1H = (timerVal >> 8);
        TMR1L = (uint8_t) timerVal;
    }
}

void TMR1_Reload(void)
{
    // Write to the Timer1 register
    TMR1H = (timer1ReloadVal >> 8);
    TMR1L = (uint8_t) timer1ReloadVal;
}

void TMR1_StartSinglePulseAcquisition(void)
{
    T1GCONbits.T1GGO = 1;
}

uint8_t TMR1_CheckGateValueStatus(void)
{
    return T1GCONbits.T1GVAL;
}

void TMR1_ISR(void)
{

    // Clear the TMR1 interrupt flag
    PIR1bits.TMR1IF = 0;
    
    // Write to the Timer1 register
    TMR1H = (timer1ReloadVal >> 8);
    TMR1L = (uint8_t) timer1ReloadVal;

    if(TMR1_InterruptHandler)
    {
        TMR1_InterruptHandler();
    }
}


void TMR1_SetInterruptHandler(void* InterruptHandler){
    TMR1_InterruptHandler = InterruptHandler;
}

void TMR1_DefaultInterruptHandler(void){
    
    //Stop measurements and drain cap
    TMR0_StopTimer();
    TMR0_Reload();
    EXT_INT0_InterruptDisable();
    UC_SetHigh();
    
    //Update display
    char cap_string[16];
    char range_string[16];

    //Units logic
    if (result >= 1000000.0)
    {
        sprintf(cap_string, "C=%12.6fuF", result / 1000000.0);
    }
    else if (result < 1000000 && result >= 1000.0)
    {
        sprintf(cap_string, "C=%12.3fnF", result / 1000.0);
    }
    else if (result < 0)
    {
        sprintf(cap_string, "C=ERROR         ");
    }
    else
    {
        sprintf(cap_string, "C=%12.0fpF", result);
    }

    sprintf(range_string, "Range %d (%d)   ", range, auto_range);
    
    lcd_sendStringToPos(1, 1, cap_string);
    lcd_sendStringToPos(1, 2, range_string);
    
    //Restart measurements
    EXT_INT0_InterruptEnable();
    TMR0_StartTimer();
    UC_SetLow();  
}


/**
 End of File
*/
