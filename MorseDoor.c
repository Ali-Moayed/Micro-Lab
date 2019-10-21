/*
 * MorseDoor.c
 *
 * Created: 10/9/2019 6:00:02 PM
 * Author: MSI
 */

#include <io.h>
#include <stdbool.h>

static unsigned int time_count;
bool hold = false;

bool isOpening = false;

bool isResetting = false;
bool inputForReset = false;

char code = 0b00001010;
char resetInput = 0b00000000;
char input = 0b00000000;

interrupt [EXT_INT0] void ext_int0_isr (void) 
{ 
    if (isOpening == false)
    {
        TIMSK=0x01;
        hold = false;
        time_count = 0;
        if (PIND.0 == 1)
        {
            isResetting = true;
            inputForReset = true;
            input = 0b00000000;
        }
        else if (isResetting == true)
        {
            isResetting = false;
        }
        
        if (PIND.0 == 0)
            PORTC.1 = 0;
    }
}

interrupt [EXT_INT1] void ext_int1_isr (void) 
{
    if (isOpening == false)
    {   
        if(inputForReset)
        {  
            if (PORTC.1 == 1)
            {
                if(hold == true)
                {  
                    code = code<<1;
                    code |= 1UL << 0; //Enqueue 1
                }
                else
                {               
                    code = code<<1;
                    code &= ~(1UL << 0);  //Enqueue 0
                }
            }
            else
            {   
                if(hold == true)
                { 
                    resetInput = resetInput<<1;
                    resetInput |= 1UL << 0; //Enqueue 1
                }
                else
                {                       
                    resetInput = resetInput<<1;
                    resetInput &= ~(1UL << 0);  //Enqueue 0
                }
                
                if (resetInput == code)
                {
                    PORTC.1 = 1;
                    resetInput = 0b00000000;
                    code = 0b00000000;
                }
                
                PORTB = resetInput;
            }
            
            inputForReset = false;
        }
        else
        {
            if(hold == true)
            {  
                input = input<<1;
                input |= 1UL << 0; //Enqueue 1
            }
            else
            {               
                input = input<<1;
                input &= ~(1UL << 0);  //Enqueue 0
            }
            
            if(code == input)
            {
                PORTC.0 = 1;
                
                isOpening = true;
                
                TIMSK=0x01;
                hold = false;
                time_count = 0;
            }
            else
            {
                PORTC.0 = 0;
            }
                
            PORTB = input;
        }
        
        PORTA = code; 
    }
}  

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCNT0 = 31;
    ++time_count;
    if ((time_count == 200)&(PORTC.0 == 0))
    {    
        TIMSK=0x00;  
        hold = true;
    }
    if ((time_count == 1200)&(PORTC.0 == 1))
    {    
        TIMSK=0x00;
        isOpening = false;
        PORTC.0 = 0;
        input = 0b00000000;
        PORTB = 0b00000000;
    }
}


void main(void)
{    
    TCCR0=0x02;
    TCNT0=0x31;
    
    WDTCR = 0x0f;
    
    GICR=0xc0;
    MCUCR=0x0b;
    #asm("sei");      

    DDRC.0 = 1;
    DDRC.1 = 1;
    DDRA = 0xff;
    DDRB = 0xff;
               
    DDRD.0 = 0;
    DDRD.2 = 0;
    DDRD.3 = 0; 
    
    PORTA = code;
    PORTC.1 = 0;
            
    while (1)
    {   
        #asm("wdr");
        
        if (PIND.0 == 0)
            PORTC.1 = 0; 
    }
}
