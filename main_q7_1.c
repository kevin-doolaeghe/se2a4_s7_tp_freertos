#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>
#include <timers.h>

TimerHandle_t timer1, timer2;

void fin_timer(TimerHandle_t x) {
    if (x == timer1) {
        PORTA &= 0b01111111; // Eteint la LED de droite
    }
    if (x == timer2) {
        PORTA &= 0b11111110; // Eteint la LED de gauche
    }
}

ISR(INT0_vect) { // INT0
    PORTA |= 0x80; // Allume la LED de droite
    xTimerStartFromISR(timer1, 0);
}

ISR(INT1_vect) { // INT1
    PORTA |= 0x01; // Allume la LED de gauche
    xTimerStartFromISR(timer2, 0);
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;
    PORTC = 0x10;

    EIMSK = 0b00000011;
    EICRA = 0b00000101;
    sei();

    timer1 = xTimerCreate("Timer1", 500, 0, NULL, fin_timer);
    timer2 = xTimerCreate("Timer2", 500, 0, NULL, fin_timer);
    vTaskStartScheduler();

    return 0;
}
