/*
 * BlinkingLED.c
 *
 * Created: 10/7/2019 10:44:15 AM
 * Author: MSI
 */

#include <io.h>
#include <delay.h>
 
void main(void){
    DDRC.0 = 1;
    DDRA.0 = 0;
    DDRA.1 = 0;
    DDRA.2 = 0;
    DDRA.3 = 0;
    PORTC.0 = 0;
    while (1){
        PORTC.0 = ! PORTC.0;
        delay_ms(100 - 40*PINA.0 - 80*PINA.1 + 40*PINA.2 + 80*PINA.3);
    }
}
