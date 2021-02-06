#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define portTICK_PERIOD_MS 2

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

/*
 * Défaut -> 0
 * R -> 1
 * B -> 2
 * V -> 3
 */
char boule = 0;

void TacheAffiche(void *parametres) {
    while (1) {
        PORTC = 0b01000000;
        PORTA = codeAff[boule];
        vTaskDelay(1);
    }
}

void cmd_trappes() {
    // Ecriture
    UDR0 = 'O'; // Commande 'Ouvrir trappe'
    while (!(UCSR0A & (1 << UDRE0))) {
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    UDR0 = 48 + boule; // Ouvre trappe courante (num trappe à ouvrir)
    while (!(UCSR0A & (1 << UDRE0))) {
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    UDR0 = 'F'; // Commande 'Fermer trappe'
    while (!(UCSR0A & (1 << UDRE0))) {
        vTaskDelay(2 / portTICK_PERIOD_MS);
    }
    if (boule == 1) { // Si boule courante = 1
        UDR0 = 48 + 2; // Ferme trappe 2
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
        UDR0 = 48 + 3; // Ferme trappe 3
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    } else if (boule == 2) { // Si boule courante = 2
        UDR0 = 48 + 1; // Ferme trappe 1
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
        UDR0 = 48 + 3; // Ferme trappe 3
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    } else if (boule == 3) { // Si boule courante = 3
        UDR0 = 48 + 1; // Ferme trappe 1
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
        UDR0 = 48 + 2; // Ferme trappe 2
        while (!(UCSR0A & (1 << UDRE0))) {
            vTaskDelay(2 / portTICK_PERIOD_MS);
        }
    }

}

void TacheCom(void *parametres) {
    while (1) {
        // Lecture
        if ((UCSR0A & 0x80) != 0) {
            unsigned char capt = UDR0; // Lecture boule
            if (capt == 'R') {
                boule = 1; // 1 = boule R
                cmd_trappes(); // Commande des trappes en fonction de la boule courante
            } else if (capt == 'B') {
                boule = 2; // 2 = boule B
                cmd_trappes(); // Commande des trappes en fonction de la boule courante
            } else if (capt == 'V') {
                boule = 3; // 3 = boule V
                cmd_trappes(); // Commande des trappes en fonction de la boule courante
            }
        }
    }
}

int main(void) {
    DDRA = 0xFF;
    DDRC = 0xF0;

    UBRR0 = 103;    // 9600 bauds
    UCSR0C = 6;     // 8 data, 1 stop
    UCSR0B = 0x18;  // RXEN et TXEN à 1
    UCSR0A = 0;

    xTaskCreate(TacheAffiche, "affiche", 128, NULL, 1, NULL);
    xTaskCreate(TacheCom, "com", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
