#include "main.h"
#include "ADC.h"
#include "LCD.h"

void setupADC(void){
    PORTA = 0;              //Clears portA pins
    TRISA = 0b11111111;     //Set Port A to input

    //Set control registers
    ADCON0 = 0b10001001;	//Fosc/32, Channel 0, ADC on
    ADCON1 = 0b00000010;	//left justified, PortE Digital, PortA Analogue
    
    __delay_us(50);         // Delay for ADC aquisition
}

void ADCMain(void){
               
        getADC();                   //Get rawADC value from sensor
        lcdSetCursor(0b10000000);   //First line, first position
        lcdWriteToDigitBCD(adcRAW); //Write the raw ADC value
        
}

//get an adc result from the selected channel
unsigned int getADC(void){

    ADRESH	= 0; 	 	// Reset the ADRES value register
    ADRESL = 0;

    GO = 1;                 // Staring the ADC process
    while(GO) continue;     // Wait for conversion complete

    //Calculates the 10bit ADC output
    
    adcRAW = (4*ADRESH);    //Converts ADRESH to the top 8 bits of the 10bit value
    //Adds 1, 2 or 3 to the total depending on the last 2 bits of the 10bit value (ADRESL)
    switch(ADRESL){         
        case 0b01000000: adcRAW = adcRAW + 1;
        break;
        case 0b10000000: adcRAW = adcRAW + 2;
        break;
        case 0b11000000: adcRAW = adcRAW + 3;
        break;
        default: adcRAW = adcRAW;
    }

    return(adcRAW); // Return the value of the ADC process
}

