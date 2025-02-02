#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

unsigned char cpt = 0;
unsigned char dpt = 0;

void TacheAff(void *parametres){
    while (1) {
        PORTC = 0x20;
        PORTA = codeAff[cpt];
        vTaskDelay(5);
        PORTC = 0x80;
        PORTA = codeAff[dpt];
        vTaskDelay(5);
    }
}

void TacheCompteur(void *parametres){
    unsigned char i = 0;
    while (1) {
        for (i = 0; i < 10; i++) {
            cpt = i;
            vTaskDelay(128);
        }
    }
}

ISR(INT0_vect){ //Interruption INT0 == Décompteur
    dpt == 0 ? dpt = 9 : dpt--;
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    EIMSK = 0b00000001; // Active INT0
    EICRA = 0b00000011; // Rising-edge sur INT0
    sei(); // Autorise les interruptions au MicroP

    xTaskCreate(TacheAff, "aff", 128, NULL, 1, NULL);
    xTaskCreate(TacheCompteur, "cpt", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
