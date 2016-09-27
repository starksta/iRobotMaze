/* 
 * File:   ADC.h
 * Author: Brendan
 *
 * Created on 24 September 2016, 1:24 PM
 */

#ifndef ADC_H
#define	ADC_H

void ADCMain(void);
void setupADC(void);
unsigned int getADC(void);
unsigned int adcConvertMM(unsigned int);

volatile unsigned int adcRAW = 0;

#endif	/* ADC_H */

