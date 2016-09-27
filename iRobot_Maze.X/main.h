/* 
 * File:   main.h
 * Author: Brendan
 *
 * Created on 24 September 2016, 1:14 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#pragma config BOREN = OFF, CPD = OFF, WRT = OFF, FOSC = HS, WDTE = OFF, CP = OFF, LVP = OFF, PWRTE = OFF
#define _XTAL_FREQ 20000000

#define     SELECT_NONE()	RC0 = 0; RC1 = 0;
#define     SELECT_EEPROM()	RC0 = 0; RC1 = 1;
#define     SELECT_RF()		RC0 = 1; RC1 = 0;
#define     SELECT_SM()		RC0 = 1; RC1 = 1;
#define     SM_STEP()		RC2 = 1; NOP(); RC2 = 0;

#define TMR0_VAL 100
#define PB8 !RB0

volatile unsigned int time_count;

unsigned char PB8Counter = 0;

unsigned char highByte = 0;
unsigned char lowByte = 0;
unsigned char left = 0;
unsigned char right = 0;
unsigned char up = 0;
unsigned char down = 0;
unsigned char x_origin = 0;
unsigned char y_origin = 0;
unsigned char x_path = 0;
unsigned char y_path = 0;
unsigned char x_target = 0;
unsigned char y_target = 0;
unsigned char orientation_path = 0;

unsigned char pathCount = 0;
unsigned char pathCountShortest = 20;

unsigned char x_test = 0;
unsigned char y_test = 0;
unsigned char intersection_Orientation = 0;

volatile char path[3][16];
volatile char shortest_path[3][16];
volatile char mazeGrid[4][5];
volatile char mazeGridDefault[4][5];


volatile char path[3][16] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

volatile char shortest_path[3][16] = {
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


volatile char mazeGrid[4][5] = {
    {0b00001100,0b00000111,0b00001110,0b00001101,0b00000110},
    {0b00000111,0b00000101,0b00000000,0b00000101,0b00000010},
    {0b00001100,0b00000110,0b00001010,0b00001110,0b00001010},
    {0b00001011,0b00001001,0b00000001,0b00000001,0b00000011}
};

volatile char mazeGridDefault[4][5] = {
    {0b00001100,0b00000111,0b00001110,0b00001101,0b00000110},
    {0b00000111,0b00000101,0b00000000,0b00000101,0b00000010},
    {0b00001100,0b00000110,0b00001010,0b00001110,0b00001010},
    {0b00001011,0b00001001,0b00000001,0b00000001,0b00000011}
};

//Square Identifier: left/up/right/down
volatile char squareWalls[5][16] = {
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
    {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},
    {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
};

#endif	/* MAIN_H */

