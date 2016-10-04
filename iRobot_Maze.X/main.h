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
#define PB7 !RB1

volatile unsigned int time_count;

unsigned char PB8Counter = 0;
unsigned char PB7Counter = 0;

unsigned char highByte = 0;
unsigned char lowByte = 0;

unsigned char r = 0;
unsigned char c = 0;

unsigned char x = 1;    //Bots actual coordinates
unsigned char y = 0;
unsigned char orientation = 3;

unsigned char x_target = 0; //Target coordinates for PathTo()
unsigned char y_target = 0;

unsigned char x_path = 0;   //Tracking coordinates for the PathTo()
unsigned char y_path = 0;
unsigned char orientation_path = 0; //Tracking orientation for PathTo()

unsigned char x_int = 0;    //Coordinates of most recent intersection
unsigned char y_int = 0;
unsigned char intersection_orientation = 0; //Orientation at most recent intersection

unsigned char pathCount = 0;
unsigned char pathCountShortest = 20;

bit IR_Wall = 0;

bit left = 0;
bit right = 0;
bit up = 0;
bit down = 0;
bit reset_flag = 0;


   

//Array storing current path
char path[2][16] = {    
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

//Array storing shortest path
char pathShortest[2][16] = {   
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


//Dictates the wall configuration for each grid, subject to change with fake walls etc
char mazeGrid[4][5] = {
    {12,7,14,13,6},
    {9,5,0,5,2},
    {12,6,10,14,10},
    {11,9,1,1,3}
};

//Same thing, always the same for resetting
char mazeGridDefault[4][5] = {
    {12,7,14,13,6},
    {9,5,0,5,2},
    {12,6,10,14,10},
    {11,9,1,1,3}
};

//Wall configuration: left/up/right/down
char gridWalls[4][16] = {    
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
    {0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},
    {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}
};

#endif	/* MAIN_H */

