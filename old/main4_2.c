#include <freeRTOS.h>
#include <task.h>

const unsigned char codeAff[10] = { 0x7e, 0x0c, 0x37, 0x1f, 0x4d, 0x5b, 0x7b, 0x0e, 0x7f, 0x5f };

int val_compteur;
int val_decompteur;

void tacheAffichage(void *param) {
    while (1) {
        PORTC = 0x20; // Afficheur de droite
        PORTA = codeAff[i];
        vTaskDelay(1);
        PORTC = 0x80; // Afficheur de gauche
        PORTA = codeAff[i];
        vTaskDelay(1);
    }
}

void tacheCompteur(void *param) {
    unsigned char i;

    while (1) {
        for (i = 0; i < 10; i++) { // Compteur
            val_compteur = i;
        }
    }
}

void tacheDecompteur(void *param) {
    unsigned char i;

    while (1) {
        for (i = 9; i <= 0; i--) { // Décompteur
            val_decompteur = i;
        }
    }
}

int main() {
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(tacheCompteur, "compteur", 128, NULL, 2, NULL);
    xTaskCreate(tacheDecompteur, "decompteur", 128, NULL, 2, NULL);
    xTaskCreate(tacheAffichage, "affichage", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    
    return 0;
}