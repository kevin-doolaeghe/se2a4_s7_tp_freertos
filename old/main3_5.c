#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7e, 0x0c, 0x37, 0x1f, 0x4d, 0x5b, 0x7b, 0x0e, 0x7f, 0x5f };

const unsigned char codeDigi[10] = { 0x41, 0x88, 0x48, 0x28, 0x84, 0x44, 0x24, 0x82, 0x42, 0x22 };

void codeOk() {
    int i;
    PORTC = 0x10;

    for (i = 0; i < 47; i++) { // 200ms / 2.13ms / 2
        PORTA = 0b10101010;
        vTaskDelay(1);
        PORTA = 0b01010101;
        vTaskDelay(1);
    }
}

void codeErr() {
    PORTC = 0x10;
    PORTA = 0xFF;
    vTaskDelay(1408); // 3s / 2.13ms
}

void tacheDigi(void *param) {
    unsigned char i, j;

    while (1) {
        PORTC = 0b11100000; // 3 afficheurs
        PORTA = 0x01; // Affiche -

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 10 ; j++) {
                PORTC = 0x80 >> i;
                if (PINC == codeDigi[j]) {
                    if (PINC == codeDigi[5]) {
                        codeOk();
                    } else {
                        codeErr();
                    }
                }
            }
        }
    }
}

void main() {
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(tacheDigi, "digi", 128, NULL, 2, NULL);
    vTaskStartScheduler();
}