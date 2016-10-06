#include "main.h"
#include "motor.h"
#include "SPI.h"

void stepCCW(int steps){
    
    __delay_ms(100);       
    spi_transfer(0b00001111);//11001000
    __delay_ms(100);
     spi_transfer(0b00001111);//11001000
    __delay_ms(100);
     spi_transfer(0b00001111);//11001000
    __delay_ms(100);
    
    for (loop=steps; loop > 0; loop--){
        SM_STEP();
        __delay_ms(3);
    }
}

void stepCW(int steps){
    
    __delay_ms(100);
      spi_transfer(0b00001101); 
    __delay_ms(100);     
      spi_transfer(0b00001101); 
    __delay_ms(100);
      spi_transfer(0b00001101); 
    __delay_ms(100);    
    
       for (loop=steps; loop > 0; loop--){
        SM_STEP();
        __delay_ms(3);
    }
}
