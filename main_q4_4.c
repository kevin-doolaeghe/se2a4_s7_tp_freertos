#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>
#include <stdbool.h>

const unsigned char codeAff[10] = { 0x00, 0x02, 0x01, 0x10 };

unsigned char dist_gauche = 0;
unsigned char dist_droite = 0;

bool sel_adc = 0; // Capteur distance droit par défaut

unsigned char conv(unsigned char dist){
    if (dist > 230) return codeAff[3]; // Inf. à 10cm == Segment bas
    else if (dist > 143 && dist < 230) return codeAff[2]; // Entre 10cm et 20cm == Segment milieu
    else if (dist > 82 && dist < 143) return codeAff[1]; // Entre 20cm et 30cm == Segment haut
    else if (dist < 82) return codeAff[0]; // Sup. à 30cm == Eteint
}

void TacheAffiche(void *parametres){
    while(1){
        PORTC = 0x20;
        PORTA = conv(dist_droite);
        vTaskDelay(5);
        PORTC = 0x80;
        PORTA = conv(dist_gauche);
        vTaskDelay(5);
    }
}

ISR(ADC_vect){ // Tâche d'acquisition des mesures
    if (sel_adc == 0){ //Capteur distance droit
        dist_droite = ADC; //dist_droite == valeur de conversion ADC
        ADMUX = 0b01000010; //Voie PF2 (Gauche) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 1;    //Sélection de l'afficheur gauche
    } else { //Capteur distance gauche
        dist_gauche = ADC;
        ADMUX = 0b01000001; //Voie PF1 (Droite) pas de facteur de conversion
        ADCSRB = 0b00000000;
        ADCSRA = 0b11011111;
        sel_adc = 0;    //Sélection de l'afficheur droit
    }
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    EIMSK = 0b00000000;
    EICRA = 0b00000000;
    sei(); // Autorise les interruptions sur le microP

    ADMUX = 0b01000001; //Voie PF1 (Droite) pas de facteur de conversion
    ADCSRB = 0b00000000;
    ADCSRA = 0b11011111;

    xTaskCreate(TacheAffiche, "aff", 128, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
