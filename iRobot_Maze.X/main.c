#include "main.h"
#include "motor.h"
#include "LCD.h"
#include "ADC.h"
#include "SPI.h"
#include "ser.h"


volatile char x = 1;
volatile char y = 0;
volatile char orientation = 0;

signed int distance = 0;
signed int angle = 0;

char r = 0;
char c = 0;

void Drive1m(void){   
   
    //Add virtual wall check

    switch(orientation){
        case 0: y--;
        break;
        case 1: x++;
        break;
        case 2: y++;
        break;
        case 3: x--;
        break;
    }
    
    distance = 0;
    DriveDirect(250,250);
    while (distance < 1000){
        distance = distance + getSensorData(19,2);
    }
}

//Turns 90 degrees, direction( 1 = CCW, 0 = CW)
void Turn90(char direction){
    angle = 0;
    if (direction == 1){
        
        if (orientation == 0)
                orientation = 3;
        else orientation--;
        
        DriveDirect(150,0);
        while (angle < 90){
            angle = angle + getSensorData(20,2);
        }
    }
    
    if (direction == 0){
        
        if (orientation == 3)
            orientation = 0;
        else orientation++;
        
        DriveDirect(0,150);
        while (angle > -90){
            angle = angle + getSensorData(20,2);
        }
    }
}

void PathTo(char x_target, char y_target){
    
    
    //mazeGrid = mazeGridDefault
    for (r = 0; r <= 3; r++){        
        for (c = 0; c <= 4; c++){
            mazeGrid[r][c] = mazeGridDefault[r][c];
        }
    }
    
    x_origin = x;
    y_origin = y; 
    x_path = x;
    y_path = y;   
    orientation_path = orientation;
    pathCount = 0;
    
    for (int loop = 0; loop <= 1000; loop++){
    
    //Stores the traveled grid squares in an array
        path[1][pathCount] = x_path;
        path[2][pathCount] = y_path;
          
        if ((x_path == x_target) && (y_path == y_target)){
        
            if(pathCount < pathCountShortest){
                pathCountShortest = pathCount;
                for (r = 0; r <= 2; r++){        
                    for (c = 0; c <= 15; c++){
                        shortest_path[r][c] = path[r][c];
                    }
                }                       
            }    
            //Sets fake wall at last intersection and resets to origin 
            char n = 15;
            x_test = path[1][n];
            y_test = path[2][n];
        
            for (n = 15; mazeGrid[y_test][x_test] < 32; n--){
                x_test = path[1][n];
                y_test = path[2][n];           
            }
        
        //Sets a fake wall to block off the path at the intersection
            switch(intersection_Orientation){
                case 0: mazeGrid[y_test][x_test] = 0b00000100 || mazeGrid[y_test][x_test];
                    break;
                case 1: mazeGrid[y_test][x_test] = 0b00000010 || mazeGrid[y_test][x_test];
                    break;
                case 2: mazeGrid[y_test][x_test] = 0b00000001 || mazeGrid[y_test][x_test];
                    break;
                case 3: mazeGrid[y_test][x_test] = 0b00001000 || mazeGrid[y_test][x_test];
                    break;
            }    
            
            //Rest back to origin
            x_path = x_origin;
            y_path = y_origin;
            orientation_path = orientation;
            pathCount = 0;                       
            }
    
    //Gets wall details about current grid
        left    = squareWalls[1][((mazeGrid[y_path][x_path]) + 1)];
        right   = squareWalls[2][((mazeGrid[y_path][x_path]) + 1)];
        up      = squareWalls[3][((mazeGrid[y_path][x_path]) + 1)];
        down    = squareWalls[4][((mazeGrid[y_path][x_path]) + 1)];
    
    //Ignores the direction the robot came from using a fake wall
        switch(orientation_path){
            case 0: down = 1;
                break;
            case 1: left = 1;
                break;
            case 2: up = 1;
                break;
            case 3: right = 1;
                break;       
        }
    
    //Sets bit 5 as intersection
        char sum = (left + up + down + right);
        if(sum <= 2)
            (mazeGrid[y_path][x_path]) = ((0b00100000) || (mazeGrid[y_path][x_path]));
    
    
    //Adjusts path coordinates to next adjacent grid
        if(left == 0){
            if(mazeGrid[y_path][x_path] >= 32)
                intersection_Orientation = 3;
            x_path--;
            orientation_path = 3;
            pathCount++;          
        }
        else if(up == 0){
            if(mazeGrid[y_path][x_path] >= 32)
                intersection_Orientation = 0;
            y_path--;
            orientation_path = 0;
            pathCount++;           
        }
        else if(right == 0){
            if(mazeGrid[y_path][x_path] >= 32)
                intersection_Orientation = 1;
            x_path++;
            orientation_path = 1;
            pathCount++;           
        }
        else if(down == 0){
            if(mazeGrid[y_path][x_path] >= 32)
                intersection_Orientation = 2;
            y_path++;
            orientation_path = 2;
            pathCount++;           
        }
    
    //If dead end or too many steps then find most recent intersection and fake wall it
        if((left == up == right == down == 1) || (pathCount > 50)){
        
            char n = 15;
            x_test = path[1][n];
            y_test = path[2][n];
        
            for (n = 15; mazeGrid[y_test][x_test] < 32; n--){
                x_test = path[1][n];
                y_test = path[2][n];           
            }
        
        //Sets a fake wall to block off the path at the intersection
            switch(intersection_Orientation){
                case 0: mazeGrid[y_test][x_test] = 0b00000100 || mazeGrid[y_test][x_test];
                    break;
                case 1: mazeGrid[y_test][x_test] = 0b00000010 || mazeGrid[y_test][x_test];
                    break;
                case 2: mazeGrid[y_test][x_test] = 0b00000001 || mazeGrid[y_test][x_test];
                    break;
                case 3: mazeGrid[y_test][x_test] = 0b00001000 || mazeGrid[y_test][x_test];
                    break;
            }    
            
            //Rest back to origin
            x_path = x_origin;
            y_path = y_origin;
            orientation_path = orientation;
            pathCount = 0;
            
        }
    }
}




void interrupt isr(void){
    if(TMR0IF){
        TMR0IF = 0;
        TMR0 = TMR0_VAL;
        time_count++;
        
        if (PB8 == 1)
            PB8Counter++;
        
            
        
        
    }
}


void main(void){
    
    ser_init();
    setupSPI();
    setupLCD();
    setupADC();
    
    x = 1;
    y = 0;
    
    //while(1){
    char a = 1;
    while (a == 1){
        //if(PB8Counter == 10 && PB8 == 0){
        PathTo(3,2);
        
        lcdSetCursor(0b10000000);
        for (char n = 0; n < 10; n++){       
            lcdWriteData((shortest_path[1][n]) + 48);
            __delay_ms(15);
        }
        lcdWriteString(" ");
        lcdWriteToDigitBCD(pathCount);
        
        lcdSetCursor(0b11000000);
        for (char n = 0; n < 10; n++){           
            lcdWriteData((shortest_path[2][n]) + 48);
            __delay_ms(15);
        }
        lcdWriteString(" ");
        lcdWriteToDigitBCD(pathCountShortest);
        
        //PB8Counter = 0;
        //}
        a = 0;
    }
        
    //}
    
}
