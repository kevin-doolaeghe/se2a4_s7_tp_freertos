#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

void TacheCompteur(void *parametres){
    unsigned int i;
    TickType_t dernierReveil = 0;

    int u = 0;
    int d = 0;
    int c = 0;
    int led = 0x01;

    while(1){
        for(i = 0; i < 1000; i++) {
            u = i % 10;
            d = (i / 10) % 10;
            c = (i / 100) % 10;

            PORTC = 0b00100000;
            PORTA = codeAff[u];
            vTaskDelay(1);
            PORTC = 0b01000000;
            PORTA = codeAff[d];
            vTaskDelay(1);
            PORTC = 0b10000000;
            PORTA = codeAff[c];
            vTaskDelay(1);
            PORTC = 0b00010000;
            PORTA = 0x01 << d;
            // led = (led << 1) & 0xFF;
            // if (led==0) led=0x01;
            vTaskDelay(1);
        }
    }
}


int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(TacheCompteur, "cpt", 128, NULL, 2, NULL);
    vTaskStartScheduler();

    return 0;
}
