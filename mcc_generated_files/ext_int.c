/**
   Section: Includes
 */
#include <xc.h>
#include <stdio.h>
#include "ext_int.h"
#include "tmr0.h"

#define MIN_COUNTS 1000
#define MAX_COUNTS 60000

const uint8_t R1_BIT[] = {0, 1, 2, 3, 4};
const uint8_t RSET_BIT[] = {2, 3, 3, 4, 5};
const float cal_current[] = {0.50, 2.98, 108.43, 4525.0, 112000.0};

//***User Area Begin->code: Add External Interrupt handler specific headers 

//***User Area End->code: Add External Interrupt handler specific headers
uint16_t reading;
float result;

int8_t delta_range = 0;
uint8_t range = 2;

/**
   Section: External Interrupt Handlers
 */
// INTn Dynamic Interrupt Handlers 
void (*INT1_InterruptHandler)(void);
void (*INT0_InterruptHandler)(void);
/**
  Interrupt Handler for EXT_INT1 - INT1
*/
void INT1_ISR(void)
{
    //***User Area Begin->code***
    if (range < 5)
    {
        range += 1;
    }
    else
    {
        range -= 3;
    }
    
    //Reset all range bits
    LATC |= (1 << R1_BIT[0]) | (1 << R1_BIT[1]) | (1 << R1_BIT[2]) | (1 << R1_BIT[3]) | (1 << R1_BIT[4]);
    LATB |= (1 << RSET_BIT[0]) | (1 << RSET_BIT[1]) | (1 << RSET_BIT[3]) | (1 << RSET_BIT[4]);

    LATC &= ~(1 << R1_BIT[range - 1]);
    LATB &= ~(1 << RSET_BIT[range - 1]);
    
    //***User Area End->code***
        
    EXT_INT1_InterruptFlagClear();  
}
        
/**
  Callback function for EXT_INT1 - INT1
*/
void INT1_CallBack(void)
{
    // Add your custom callback code here
    if(INT1_InterruptHandler)
    {
        //INT1_InterruptHandler();
    }
}

/**
  Allows selecting an interrupt handler for EXT_INT1 - INT1 at application runtime
*/
void INT1_SetInterruptHandler(void* InterruptHandler){
    INT1_InterruptHandler = InterruptHandler;
}
    
/**
  Default interrupt handler for EXT_INT1 - INT1 
*/
void INT1_DefaultInterruptHandler(void){
    // add your INT1 interrupt custom code
    // or set custom function using INT1_SetInterruptHandler()
}
/**
  Interrupt Handler for EXT_INT0 - INT0
*/
void INT0_ISR(void)
{
    //Stop microsecond timer and get reading
    TMR0_StopTimer();
    reading = TMR0_ReadTimer();

    //Compute capacitance reading in pF
    //Current in uA
    result = cal_current[range - 1] * ((float)reading - 27) / 3.01004;
    
    //Print capacitance reading
    printf("Capacitance: %0.2f pF (Count: %u) - Range: %d\r\n", result, reading, range);
    
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
    INT1
    INT0
*/
void EXT_INT_Initialize(void)
{
    
    /*******
     * INT1
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EXT_INT1_InterruptFlagClear();   
    EXT_INT1_fallingEdgeSet();    
    // Set Default Interrupt Handler
    INT1_SetInterruptHandler(INT1_DefaultInterruptHandler);
    EXT_INT1_InterruptEnable();      



    
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

