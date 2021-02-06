#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7e, 0x0c, 0x37, 0x1f, 0x4d, 0x5b, 0x7b, 0x0e, 0x7f, 0x5f };

void tacheCompteur(void *param) {
    unsigned char i;
    TickType_t dernierReveil = 0;

    PORTC = 0x40;

    while (1) {
        for (i = 0; i < 10; i++) {
            PORTA = codeAff[i];
            vTaskDelayUntil(&dernierReveil, 100);
        }
        vTaskDelay(500);
    }
}

void main() {
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(tacheCompteur, "cpt", 128, NULL, 2, NULL);
    vTaskStartScheduler();
}