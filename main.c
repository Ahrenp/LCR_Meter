#include "mcc_generated_files/mcc.h"
#include "lcd.h"

extern uint16_t reading;
extern float result;
char units[2] = "pF";

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighEnable();

    // Enable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowEnable();

    // Disable high priority global interrupts
    //INTERRUPT_GlobalInterruptHighDisable();

    // Disable low priority global interrupts.
    //INTERRUPT_GlobalInterruptLowDisable();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    lcd_init();
    lcd_clear();
    
    //Initial range
    //All control bits high
    LATC |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
    LATB |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    
    //Desired range bits low
    LATC &= ~(1 << 1);
    LATB &= ~(1 << 3);
    
    while (1)
    {        
        //Update display
        char cap_string[16];
        
        //Units logic
        if (result >= 1000000.0)
        {
            sprintf(cap_string, "C=%12.6fuF", result / 1000000.0);
        }
        else if (result < 1000000 && result >= 1000.0)
        {
            sprintf(cap_string, "C=%12.3fnF", result / 1000.0);
        }
        else if (result < 1000 && result > 0)
        {
            sprintf(cap_string, "C=%12.0fpF", result);
        }

        lcd_sendString(cap_string);
    }
}