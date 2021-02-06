#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

unsigned char cpt = 0;

TaskHandle_t H_incr;
TaskHandle_t H_long;

void TacheAff(void *parametres){
    int u = 0;
    int d = 0;
    int c = 0;

    while (1) {
        u = cpt % 10;
        d = (cpt / 10) % 10;
        c = (cpt / 100) % 10;

        PORTC = 0b00100000;
        PORTA = codeAff[u];
        vTaskDelay(1);
        PORTC = 0b01000000;
        PORTA = codeAff[d];
        vTaskDelay(1);
        PORTC = 0b10000000;
        PORTA = codeAff[c];
        vTaskDelay(1);
    }
}

void TacheCompteur(void *parametres){
    while (1) {
        if (cpt == 999) cpt = 0;
        else cpt++;
        vTaskDelay(5);
    }
}

void TacheTraitementLong(void *parametres){
    while (1) {
        vTaskResume(H_incr);
        long int i;
        for(i=0; i<654321; i++){
            asm("NOP");
        }
        vTaskSuspend(H_incr);
        vTaskSuspend(H_long);
    }
}

ISR(INT0_vect){
    cpt = 0;
    vTaskResume(H_long);
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    EIMSK = 0b00000001; // Active INT0
    EICRA = 0b00000011; // Rising-edge sur INT0
    sei(); // Autorise les interruptions au MicroP

    xTaskCreate(TacheAff, "aff", 128, NULL, 1, NULL);
    xTaskCreate(TacheCompteur, "cpt", 128, NULL, 2, &H_incr);
    xTaskCreate(TacheTraitementLong, "long", 128, NULL, 3, &H_long);
    vTaskSuspend(H_incr);
    vTaskSuspend(H_long);
    vTaskStartScheduler();

    return 0;
}
