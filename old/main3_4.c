#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7e, 0x0c, 0x37, 0x1f, 0x4d, 0x5b, 0x7b, 0x0e, 0x7f, 0x5f };

void tacheCompteur(void *param) {
    unsigned char i, j, k;

    while (1) {
        for (i = 0; i < 10; i++) {
            for (j = 0; j < 10; j++) {
                for (k = 0; k < 10; k++) {
                    PORTC = 0x80;
                    PORTA = codeAff[i];
                    vTaskDelay(1);
                    PORTC = 0x40;
                    PORTA = codeAff[j];
                    vTaskDelay(1);
                    PORTC = 0x20;
                    PORTA = codeAff[k];
                    vTaskDelay(1);
                }
            }
        }
    }
}

void main() {
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(tacheCompteur, "cpt", 128, NULL, 2, NULL);
    vTaskStartScheduler();
}