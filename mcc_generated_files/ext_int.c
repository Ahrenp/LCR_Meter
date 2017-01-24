/**
   Section: Includes
 */
#include <xc.h>
#include <stdio.h>
#include "ext_int.h"
#include "tmr0.h"
#include "pin_manager.h"

#define MIN_COUNTS 1000
#define MIN_RANGE 2
#define MAX_RANGE 5

//***User Area Begin->code: Add External Interrupt handler specific headers

// Configuration variables
// R1_BIT and RSET_BIT define the individual range bits for each port
const uint8_t R1_BIT[] =    {0, 1, 2, 3, 4};
const uint8_t RSET_BIT[] =  {2, 3, 3, 4, 5};

// Current in microamps for each range
const float cal_current[] = {0.50, 7.8, 108.43, 4430.0, 112000.0};

// Functional variables
// Reading in integer form and result in float form
uint16_t reading;
float result;

// Initial range on boot up
uint8_t range = 2;

// Auto range on (1) / off (0)
uint8_t auto_range = 1;

// If auto range is turned off the last auto selected range is retained
uint8_t stored_range = 0;

// Calibration setting - when this variable is set to 1, the UC pin stays low
uint8_t cal_mode = 0;

// Sets a range given an integer valued 1 to 5
void SetRange(uint8_t new_range)
{
    //Reset all range bits
    LATC |= (1 << R1_BIT[0]) | (1 << R1_BIT[1]) | (1 << R1_BIT[2]) | (1 << R1_BIT[3]) | (1 << R1_BIT[4]);
    LATB |= (1 << RSET_BIT[0]) | (1 << RSET_BIT[1]) | (1 << RSET_BIT[3]) | (1 << RSET_BIT[4]);

    LATC &= ~(1 << R1_BIT[new_range - 1]);
    LATB &= ~(1 << RSET_BIT[new_range - 1]);
}

//***User Area End->code: Add External Interrupt handler specific headers

/**
  Interrupt Handler for EXT_INT1 - INT1
*/
void INT1_ISR(void)
{   
    //***User Area Begin->code***
    // If auto range is currently enabled, store the auto selected range
    if (auto_range == 1)
    {
        stored_range = range;
        auto_range = 0;
    }   
    
    // Select the next range based on the current range
    if (range < MAX_RANGE)
    {
        range += 1;
    }
    else
    // Go back MAX - MIN ranges
    {
        range -= (MAX_RANGE - MIN_RANGE);
    }
    
    // Check to see if auto ranging is off, and turn it back on if 
    // the the user has looped back to the last stored auto range
    if (auto_range == 0 && range == stored_range)
    {
        auto_range = 1;
    }
    
    // DEBUG: Print the range that has been changed to
    printf("\r\nSetting range = %d\r\n", range);
    
    SetRange(range);
    
    //***User Area End->code***
        
    EXT_INT1_InterruptFlagClear();  
}

/**
  Interrupt Handler for EXT_INT0 - INT0
*/
void INT0_ISR(void)
{
    // If cal mode is enabled, make sure UC is set low and exit the ISR
    if (cal_mode == 1)
    {
        UC_SetLow();
        return;
    }
    
    //Stop microsecond timer and get reading
    TMR0_StopTimer();
    reading = TMR0_ReadTimer();
    
    //Set UC high to drain capacitor
    UC_SetHigh();
    
    //Compute capacitance reading in pF
    //Current in uA
    result = cal_current[range - 1] * ((float)reading - 27) / 3.01004;
    
    //Print capacitance reading
    printf("Capacitance: %0.2f pF (Count: %u) - Range: %d (%d)\r\n", result, reading, range, auto_range);
    
    //Autorange decrease range
    if (auto_range == 1)
    {
        if (reading < MIN_COUNTS && range > MIN_RANGE)
        {
            range -= 1;
            SetRange(range);
        }
    }
    
    //Reload the timer with inital values for its specified (60ms) range
    TMR0_Reload();
    
    //Restart timer, set UC low to recharge capacitor
    UC_SetLow();
    TMR0_StartTimer();
    
    //Clear interrupt flag
    EXT_INT0_InterruptFlagClear();  
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
    EXT_INT1_InterruptEnable();      
    
    /*******
     * INT0
     * Clear the interrupt flag
     * Set the external interrupt edge detect
     * Enable the interrupt, if enabled in the UI. 
     ********/
    EXT_INT0_InterruptFlagClear();   
    EXT_INT0_fallingEdgeSet();    
    EXT_INT0_InterruptEnable();      
}

