#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };
const unsigned char codeDigicode[10] = { 0x41, 0x88, 0x48, 0x28, 0x84, 0x44, 0x24, 0x82, 0x42, 0x22 };

void code_bon(){
    int i;
    for (i = 0; i < 10; i++) {
        PORTC = 0x10;   //LEDS en sortie
        PORTA = 0x55;   //Clignotement 01010101
        vTaskDelay(100);  //tempo
        PORTA = 0xAA;   //Clignotement 10101010
        vTaskDelay(100);  //tempo
    }
}

void code_faux(){
    PORTC = 0x10;   //LEDS en sortie
    PORTA = 0xFF;   //Allume toutes les leds
    vTaskDelay(1500);   //3/2e-3=1500 ticks
    PORTA = 0x00;      //Allume toutes les leds
}

void TacheDigicode(void *parametres){
    TickType_t dernierReveil = 0;

    // Init
    PORTC = 0xE0;
    PORTA = 0x01;

    while(1) {
        PORTC = 0x80; //Active colonne gauche (1,4,7)
        switch (PINC & 0xFF) {
        case 0x88: //Si 1 appuyé
        case 0x84: //Ou 4 appuyé
        case 0x82: //Ou 7 appuyé
            code_faux(); // Alors affichage quand code faux (!=5)
            break;
        }
        PORTC = 0x40; //Active colonne milieu (2,5,8,0)
        switch (PINC & 0xFF) {
        case 0x48: // Code faux
        case 0x42:
        case 0x41:
            code_faux();
            break;
        case 0x44: // Code bon
            code_bon();
            break;
        }
        PORTC = 0x20; //Active colonne droite (3,6,9)
        switch (PINC & 0xFF) {
        case 0x28: // Code faux
        case 0x24:
        case 0x22:
            code_faux();
            break;
        }

        // Retour aux tirets
        PORTC = 0xE0;
        PORTA = 0x01;
    }
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(TacheDigicode, "digi", 128, NULL, 2, NULL);
    vTaskStartScheduler();

    return 0;
}
