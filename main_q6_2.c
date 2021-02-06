#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>
#include <stdbool.h>

const unsigned char codeAff[10] = { 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F };

bool sel_adc = 0;	// Capteur distance droit par défaut

bool prev_dst1 = 0; // Etat précédent du capteur droit
bool prev_dst2 = 0; // Etat précédent du capteur gauche

unsigned int cpt = 0;
char delta = 20;

TaskHandle_t TH_Chenillard;

void detection(unsigned char dist) {
    if (dist > 82 + delta) {
        if (sel_adc == 0 && prev_dst1 == 0) {
			if (cpt != 999) cpt++;
			prev_dst1 = 1;
		} else if (sel_adc == 1 && prev_dst2 == 0) {
			if (cpt != 0) cpt--;
			prev_dst2 = 1;
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
        for (i = 0; i < 8; i++) {
            PORTC = 0x10;
            PORTA = 1 << i;
            vTaskDelay(16);
        }
        vTaskSuspend(TH_Chenillard);
    }
}

ISR(INT0_vect) {
    vTaskResume(TH_Chenillard);
}

ISR(ADC_vect) { 			// Tâche détection E/S
    detection(ADC);

    if (sel_adc == 0) { 	// Capteur distance droit
        ADMUX = 0b01000010; // Voie PF2 (Gauche) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 1;    	// Sélection de l'afficheur gauche
    } else { 				// Capteur distance gauche
        ADMUX = 0b01000001; // Voie PF1 (Droite) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 0;    	// Sélection de l'afficheur droit
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

    xTaskCreate(TacheAffiche, "affiche", 128, NULL, 1, NULL);
    xTaskCreate(TacheChenillard, "chenillard", 128, NULL, 1, &TH_Chenillard);
    vTaskStartScheduler();

    return 0;
}
