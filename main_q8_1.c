#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>

TaskHandle_t T_Tache1;
TaskHandle_t T_Tache2;

void Tache1(void *parametres) {
    long int i;
    while(1) {
        vTaskSuspend(NULL);
        for (i = 0; i < 500000; i++) PORTA = 0x01;
        PORTA = 0x00;
    }
}

void Tache2(void *parametres) {
    long int i;
    while(1) {
        vTaskSuspend(NULL);
        for (i = 0; i < 500000; i++) PORTA = 0x80;
        PORTA = 0x00;
    }
}

ISR(INT0_vect){
    vTaskResume(T_Tache1);
}

ISR(INT1_vect){
    vTaskResume(T_Tache2);
}

int main(void){
    DDRA = 0xFF;
    DDRC = 0xF0;
    PORTC = 0x10;

    EIMSK = 0b00000011;
    EICRA = 0b00001111;
    sei();

    xTaskCreate(Tache1, "t1", 128, NULL, 1, &T_Tache1);
    xTaskCreate(Tache2, "t2", 128, NULL, 1, &T_Tache2);
    vTaskStartScheduler();

    return 0;
}
