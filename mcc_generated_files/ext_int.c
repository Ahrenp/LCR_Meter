/**
   Section: Includes
 */
#include <xc.h>
#include <stdio.h>
#include "ext_int.h"
#include "tmr0.h"
#include "../lcd.h"
//***User Area Begin->code: Add External Interrupt handler specific headers 

//***User Area End->code: Add External Interrupt handler specific headers
uint16_t reading;
float result;
/**
   Section: External Interrupt Handlers
 */
// INTn Dynamic Interrupt Handlers 
void (*INT0_InterruptHandler)(void);

/**
  Interrupt Handler for EXT_INT0 - INT0
*/
void INT0_ISR(void)
{
    //Stop microsecond timer and get reading
    TMR0_StopTimer();
    reading = TMR0_ReadTimer();
    
    //Compute capacitance reading in pF
    result = 494.5 * ((float)reading - 27) / 3.01004;
    
    //Print capacitance reading
    printf("Capacitance: %0.2f pF (Count: %u)\r\n", result, reading);
    
    //Reload the timer with inital values for its specified (60ms) range
    TMR0_Reload();
    
    //Restart timer
    TMR0_StartTimer();
    
    //Clear interrupt flag
    EXT_INT0_InterruptFlagClear();  
}

/**
  Callback function for EXT_INT0 - INT0
*/
void INT0_CallBack(void)
{
    // Add your custom callback code here
    if(INT0_InterruptHandler)
    {
        INT0_InterruptHandler();
    }
}

/**
  Allows selecting an interrupt handler for EXT_INT0 - INT0 at application runtime
*/
void INT0_SetInterruptHandler(void* InterruptHandler){
    INT0_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for EXT_INT0 - INT0 
*/
void INT0_DefaultInterruptHandler(void){
    // add your INT0 interrupt custom code
    // or set custom function using INT0_SetInterruptHandler()
}
/**
    Section: External Interrupt Initializers
 */
/**
    void EXT_INT_Initialize(void)

    Initializer for the following external interrupts
    INT0
*/
void EXT_INT_Initialize(void)
{
    
    /*******
     * INT0
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EXT_INT0_InterruptFlagClear();   
    EXT_INT0_fallingEdgeSet();    
    // Set Default Interrupt Handler
    INT0_SetInterruptHandler(INT0_DefaultInterruptHandler);
    EXT_INT0_InterruptEnable();      



 
      
}

