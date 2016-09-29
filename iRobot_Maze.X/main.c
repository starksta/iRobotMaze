#include "main.h"
#include "motor.h"
#include "LCD.h"
#include "ADC.h"
#include "SPI.h"
#include "ser.h"

char a = 1;
char reset_flag = 0;

char x = 1;
char y = 0;

char intFound = 0;



char orientation = 3;

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
        
        reset_flag = 0;
        
        path[1][pathCount] = x_path;
        path[2][pathCount] = y_path;
        

        
        lcdWriteControl(0b00000001);
        lcdSetCursor(0b10001110);
        lcdWriteToDigitBCD(loop);
/*        
        lcdSetCursor(0b11001100);
        char gridWalls = (mazeGrid[1][2]);
        left    = squareWalls[1][gridWalls];
        up      = squareWalls[2][gridWalls];
        right   = squareWalls[3][gridWalls];
        down    = squareWalls[4][gridWalls];
        
        lcdWriteData(left + 48);
        lcdWriteData(up + 48);
        lcdWriteData(right + 48);
        lcdWriteData(down + 48);
        
       
        for (char n = 0; n <= pathCount; n++){       
            lcdSetCursor(0b10000000 + n);
            lcdWriteData(path[1][n] + 48);      
            lcdSetCursor(0b11000000 + n);
            lcdWriteData(path[2][n] + 48);          
        }
*/               
    //Gets wall details about current grid
       
        left    = squareWalls[1][(mazeGrid[y_path][x_path])];
        up      = squareWalls[2][(mazeGrid[y_path][x_path])];
        right   = squareWalls[3][(mazeGrid[y_path][x_path])];
        down    = squareWalls[4][(mazeGrid[y_path][x_path])];
         
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
        
        //Check if intersection
        char sum = (left + up + down + right);
        if(sum <= 2){
            x_int = x_path;
            y_int = y_path;
            
            if (left == 0)
                intersection_Orientation = 3;
            else if (up == 0)
                intersection_Orientation = 0;
            else if (right == 0)
                intersection_Orientation = 1;
            else if (down == 0)
                intersection_Orientation = 2;
        }
            
            
        
        
        
    //Target Found      
        if ((x_path == x_target) && (y_path == y_target)){
        
/*            for (char n = 0; n <= pathCount; n++){       
                lcdSetCursor(0b10000000 + n);
                lcdWriteData(path[1][n] + 48);      
                lcdSetCursor(0b11000000 + n);
                lcdWriteData(path[2][n] + 48);
            }
*/   
                      
            if(pathCount < pathCountShortest){
                pathCountShortest = pathCount;
                for (r = 1; r <= 2; r++){        
                    for (c = 0; c <= 15; c++){
                        shortest_path[r][c] = path[r][c];
                    }
                }                       
            }    
     
        //Sets a fake wall to block off the path at the intersection

            switch(intersection_Orientation){
                case 0: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 4);                     
                    break;
                case 1: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 2);
                    break;
                case 2: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 1); 
                    break;
                case 3: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 8);                            
                    break;
            }
                             
            //Reset back to origin
            x_path = x_origin;
            y_path = y_origin;
            orientation_path = orientation;
            pathCount = 0;
            reset_flag = 1;
            
            for (r = 1; r <= 2; r++){        
                for (c = 0; c <= 15; c++){
                    path[r][c] = 0;
                }
            }         
        }
       

    
    //If dead end or too many steps then find most recent intersection and fake wall it
        if((sum == 4) || (pathCount >= 15)){      
            
            //Sets a fake wall to block off the path at the intersection
            switch(intersection_Orientation){
                case 0: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 4);                             
                    break;
                case 1: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 2);
                    break;
                case 2: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 1);
                    break;
                case 3: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 8);
                    break;
            }
                                
            //Rest back to origin
            x_path = x_origin;
            y_path = y_origin;
            orientation_path = orientation;
            pathCount = 0;
            reset_flag = 1; 
            
            for (r = 1; r <= 2; r++){        
                for (c = 0; c <= 15; c++){
                    path[r][c] = 0;
                }
            }                    
        }
        
        
    //Adjusts path coordinates to next adjacent grid
        
        if (reset_flag != 1){
            
            if(left == 0){
                x_path--;
                orientation_path = 3;
                pathCount++;          
            }
            else if(up == 0){
                y_path--;
                orientation_path = 0;
                pathCount++;           
            }
            else if(right == 0){
                x_path++;
                orientation_path = 1;
                pathCount++;           
            }
            else if(down == 0){
                y_path++;
                orientation_path = 2;
                pathCount++;           
            }           
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
    TRISB = 0b00000000;
   
    
    x = 1;
    y = 0;
    orientation = 3;
    
    
    
        
    
       
        PathTo(0,3);
        __delay_ms(3000);
        lcdWriteControl(0b00000001);
        
        lcdSetCursor(0b10000000);
        lcdWriteString("X:");
        lcdSetCursor(0b11000000);
        lcdWriteString("Y:");
        
        for (char n = 0; n <= pathCountShortest; n++){       
                lcdSetCursor(0b10000000 + n + 2);
                lcdWriteData(shortest_path[1][n] + 48);      
                lcdSetCursor(0b11000000 + n + 2);
                lcdWriteData(shortest_path[2][n] + 48);                
        }
        lcdSetCursor(0b10001110);
        lcdWriteToDigitBCD(pathCountShortest);
        
        while(1){
            RB0 = 1;
            __delay_ms(500);
            RB0 = 0;
            __delay_ms(500);
        }        
}
    

