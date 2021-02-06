#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define portTICK_PERIOD_MS 2

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

bool sel_adc = 0;	// Capteur distance droit par d�faut

bool prev_dst1 = 0; // Etat pr�c�dent du capteur droit
bool prev_dst2 = 0; // Etat pr�c�dent du capteur gauche

unsigned int cpt = 0;
char delta = 20;

bool sens = 0;

TaskHandle_t TH_Chenillard;
TaskHandle_t TH_Chenille;

void detection(unsigned char dist) {
    if (dist > 82 + delta) {
        if (sel_adc == 0 && prev_dst1 == 0) {   //D�tection capteur droite
			if (cpt != 999) cpt++;  //Incr�mentation
			prev_dst1 = 1;  //Changement d'�tat : proximit� inf � 30cm
			if (prev_dst2 == 0) { // Si l'autre capteur (gauche) ne d�tecte rien
                sens = 0; // Set le sens droite � gauche (param�tre TacheChenille)
                vTaskResume(TH_Chenille); // Alors animation chenillard
			}
		} else if (sel_adc == 1 && prev_dst2 == 0) {    //D�tection capteur gauche
			if (cpt != 0) cpt--; // d�cr�mentation
			prev_dst2 = 1; //Changement d'�tat : proximit� inf � 30 cm
			if (prev_dst1 == 0) {   //Si l'autre capteur (droite) ne d�tecte rien
                sens = 1; // Set le sens gauche � droite (param�tre TacheChenille)
                vTaskResume(TH_Chenille); // Alors animation chenillard
			}
		}
    } else if (dist < 82 - delta) {
        if (sel_adc == 0) {
			prev_dst1 = 0;
		} else if (sel_adc == 1) {
			prev_dst2 = 0;
		}
    }
}

void TacheAffiche(void *parametres) {
    char u = 0;
    char d = 0;
    char c = 0;

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

void TacheChenillard(void *parametres) {
    unsigned char i;
    while(1) {
        vTaskSuspend(TH_Chenillard);
        for (i = 0; i < 8; i++) {
            PORTC = 0x10;
            PORTA = 1 << i;
            vTaskDelay(16);
        }
    }
}

void TacheCom(void *parametres) {
    while (1) {
        if ((UCSR0A & (1 << RXC0)) != 0) {
            unsigned char capt = UDR0;
            if (capt == 't') {
                while (!(UCSR0A & (1 << UDRE0))) {
                    vTaskDelay(5 / portTICK_PERIOD_MS);
                }
                UDR0 = 'T'; // Emet 'T' si 't' est re�u
            }
        }
    }
}


void TacheChenille(void *parametres) {
    unsigned char i;
    while(1) {
        vTaskSuspend(TH_Chenille);
        for (i = 0; i < 8; i++) {
            PORTC = 0x10;
            if (sens) { // Param�tre set � l'appel de la fonction --> Gauche vers droite
                PORTA = 0x01 << i;  // D�calage vers la gauche de i 0b0000 0001
            } else { // Param�tre set � l'appel de la fonction --> Droite vers gauche
                PORTA = 0x80 >> i;  // D�calage vers la droite de i 0b1000 0000
            }
            vTaskDelay(16);
        }
    }
}

ISR(INT0_vect) {
    vTaskResume(TH_Chenillard);
}

ISR(ADC_vect) { 			// T�che d�tection E/S
    detection(ADC);

    if (sel_adc == 0) { 	// Capteur distance droit
        ADMUX = 0b01000010; // Voie PF2 (Gauche) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 1;    	// S�lection de l'afficheur gauche
    } else { 				// Capteur distance gauche
        ADMUX = 0b01000001; // Voie PF1 (Droite) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 0;    	// S�lection de l'afficheur droit
    }
}

int main(void) {
    DDRA = 0xFF;
    DDRC = 0xF0;

    EIMSK = 0b00000001;
    EICRA = 0b00000001;
    sei();

    ADMUX = 0b01000001;
    ADCSRB = 0b00000000;
    ADCSRA = 0b11011111;

    UBRR0 = 103;    // 9600 bauds
    UCSR0C = 6;     // 8 data, 1 stop
    UCSR0B = 0x18;  // RXEN et TXEN � 1
    UCSR0A = 0;

    xTaskCreate(TacheAffiche, "affiche", 128, NULL, 1, NULL);
    xTaskCreate(TacheChenillard, "chenillard", 128, NULL, 1, &TH_Chenillard);
    xTaskCreate(TacheChenille, "chenille", 128, NULL, 1, &TH_Chenille);
    xTaskCreate(TacheCom, "com", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
