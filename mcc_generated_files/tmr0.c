#include <xc.h>
#include "tmr0.h"
#include "pin_manager.h"

extern uint8_t range;
extern uint16_t reading;
extern uint8_t auto_range;
extern void SetRange(uint8_t new_range);

/**
  Section: Global Variables Definitions
*/

void (*TMR0_InterruptHandler)(void);

volatile uint16_t timer0ReloadVal;


void TMR0_Initialize(void)
{
    // Set TMR0 to the options selected in the User Interface

    // T0PS 1:4; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS FOSC/4; TMR0ON off; PSA assigned; 
    T0CON = 0x11;

    // TMR0H 21; 
    TMR0H = 0x15;

    // TMR0L 159; 
    TMR0L = 0x9F;

    // Load TMR0 value to the 16-bit reload variable
    timer0ReloadVal = (TMR0H << 8) | TMR0L;

    // Clear Interrupt flag before enabling the interrupt
    INTCONbits.TMR0IF = 0;

    // Enabling TMR0 interrupt.
    INTCONbits.TMR0IE = 1;

    // Set Default Interrupt Handler
    TMR0_SetInterruptHandler(TMR0_DefaultInterruptHandler);

    // Start TMR0
    TMR0_StartTimer();
}

void TMR0_StartTimer(void)
{
    // Start the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 1;
}

void TMR0_StopTimer(void)
{
    // Stop the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 0;
}

uint16_t TMR0_ReadTimer(void)
{
    uint16_t readVal;
    uint8_t readValLow;
    uint8_t readValHigh;

    readValLow  = TMR0L;
    readValHigh = TMR0H;
    
    //Subtract reload value to get absolute timing
    readVal  = ((uint16_t)readValHigh << 8) + readValLow - (timer0ReloadVal);

    return readVal;
}

void TMR0_WriteTimer(uint16_t timerVal)
{
    // Write to the Timer0 register
    TMR0H = timerVal >> 8;
    TMR0L = (uint8_t) timerVal;
}

void TMR0_Reload(void)
{
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;
}

void TMR0_ISR(void)
{
    // clear the TMR0 interrupt flag
    INTCONbits.TMR0IF = 0;

    // reload TMR0
    // Write to the Timer0 register
    TMR0H = timer0ReloadVal >> 8;
    TMR0L = (uint8_t) timer0ReloadVal;

    if(TMR0_InterruptHandler)
    {
        TMR0_InterruptHandler();
    }
}


void TMR0_SetInterruptHandler(void* InterruptHandler){
    TMR0_InterruptHandler = InterruptHandler;
}

void TMR0_DefaultInterruptHandler(void)
{    
    //Timer has overflowed, toggle UC
    UC_Toggle();
    
    //Autorange increase range
    if (auto_range == 1)
    {
        if (range < 5)
        {
            range += 1;
            SetRange(range);
        }
    }
}

/**
  End of File
*/