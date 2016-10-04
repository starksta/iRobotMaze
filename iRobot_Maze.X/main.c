#include "main.h"
#include "motor.h"
#include "LCD.h"
#include "ADC.h"
#include "SPI.h"
#include "ser.h"



signed int distance = 0;
signed int angle = 0;



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
    DriveDirect(0,0);
}

//Turns 90 degrees, direction( 1 = CCW, 0 = CW)
void Turn90(char direction){
    angle = 0;
    if (direction == 1){
        
        if (orientation == 0)
                orientation = 3;
        else orientation--;
        
        Drive(0,150,0x00,0x01);
        while (angle < 90){
            angle = angle + getSensorData(20,2);
        }
        DriveDirect(0,0);
    }
    
    if (direction == 0){
        
        if (orientation == 3)
            orientation = 0;
        else orientation++;
        
        Drive(0,150,0xFF,0xFF);
        while (angle > -90){
            angle = angle + getSensorData(20,2);
        }
        DriveDirect(0,0);
    }
}

void PathTo(char x_target, char y_target){
    
    
    //mazeGrid = mazeGridDefault
    for (r = 0; r <= 3; r++){        
        for (c = 0; c <= 4; c++){
            mazeGrid[r][c] = mazeGridDefault[r][c];
        }
    }
    
    
    
    x_path = x;
    y_path = y;   
    orientation_path = orientation;
    pathCount = 0;
    
    if(IR_Wall == 1){
        switch (orientation){
            case 0: (mazeGrid[y][x]) = ((mazeGrid[y][x]) + 4);                     
                    break;
            case 1: (mazeGrid[y][x]) = ((mazeGrid[y][x]) + 2);
                    break;
            case 2: (mazeGrid[y][x]) = ((mazeGrid[y][x]) + 1); 
                    break;
            case 3: (mazeGrid[y][x]) = ((mazeGrid[y][x]) + 8);                            
                    break;
        }
    }
    
    lcdSetCursor(0b10000000);
    lcdWriteString("Working...");
    
    for (int loop = 0; loop <= 1000; loop++){
        
        reset_flag = 0;
        
        path[0][pathCount] = x_path;
        path[1][pathCount] = y_path;
                   
    //Gets wall details about current grid
        //mazeGrid[][] gives a number from 0-16 that corresponds with a particular wall pattern
        //squareWalls[][] takes that number and converts it into either a wall or no wall for each side.
        left    = gridWalls[0][(mazeGrid[y_path][x_path])];
        up      = gridWalls[1][(mazeGrid[y_path][x_path])];
        right   = gridWalls[2][(mazeGrid[y_path][x_path])];
        down    = gridWalls[3][(mazeGrid[y_path][x_path])];
         
    //Ignores the direction the robot came from using a fake wall
        //eg. if it entered the grid from the right, ie facing left (orientation = 3), then sets the right side as a wall
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
           //if sum is less than or equal to 2 then there is at most 2 walls, including the fake wall from above.
        char sum = (left + up + down + right);
        if(sum <= 2){
            x_int = x_path; //Stores the grid reference of the intersection
            y_int = y_path; //Will only ever be the most reset intersection because it overwrites.
            
            //When it moves to the next grid it prioritises left > up > right > down
            //Therefore, it will move into the first of those directions that isn't a wall
            //So it notes the direction it went(will go) at the intersection for 
            //when/if it comes back to put a fake wall if it was a dead end
            if (left == 0)
                intersection_orientation = 3;
            else if (up == 0)
                intersection_orientation = 0;
            else if (right == 0)
                intersection_orientation = 1;
            else if (down == 0)
                intersection_orientation = 2;
        }
            
            
        
        
        
    //Target Found      
        if ((x_path == x_target) && (y_path == y_target)){ 
            
            //If the pathCounts less than the previous count it replaces/stores the new path
            //Nested for loop because you cant set an array equal to another array
            //It needs to be done individually for each element
            if(pathCount < pathCountShortest){
                pathCountShortest = pathCount;
                for (r = 0; r <= 1; r++){        
                    for (c = 0; c <= 15; c++){
                        pathShortest[r][c] = path[r][c];
                    }
                }                       
            }    
     
        //Its found one path, now its going to fake wall that path so it wont repeat it when
        //it's looking for a shorter one
            //Heres where it checks that stored orientation, the number added to the end
            //is the same thing as setting the bit corresponding to that wall side.
            switch(intersection_orientation){
                case 0: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 4);                     
                    break;
                case 1: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 2);
                    break;
                case 2: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 1); 
                    break;
                case 3: (mazeGrid[y_int][x_int]) = ((mazeGrid[y_int][x_int]) + 8);                            
                    break;
            }
                             
            //Reset back to origin to start looking for a new path
            x_path = x;
            y_path = y;
            orientation_path = orientation;
            pathCount = 0;
            reset_flag = 1;
            
            //Clearing the path array
            for (r = 0; r <= 1; r++){        
                for (c = 0; c <= 15; c++){
                    path[r][c] = 0;
                }
            }         
        }
       

    
        
    //If dead end or too many steps, sum is from above where it added the wall sides
        if((sum == 4) || (pathCount >= 15)){      
            
            //Same thing as when it found the target, probably could put it all into one bit of code really.
            switch(intersection_orientation){
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
            x_path = x;
            y_path = y;
            orientation_path = orientation;
            pathCount = 0;
            reset_flag = 1; 
            
            for (r = 0; r <= 1; r++){        
                for (c = 0; c <= 15; c++){
                    path[r][c] = 0;
                }
            }                    
        }
        
        
        
        
        
    //Adjusts path coordinates to next adjacent grid
    //As mentioned above, it prioritises left > up > right > down
        
        if (reset_flag != 1){   //So it wont take a step if its resetting back to origin
            
            //These statements should be pretty self explanitory
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

void TravelPath(void){
                    
for (char n = 1; n <= pathCountShortest; n++){
        
    __delay_ms(1000);
    
        if (pathShortest[0][n] > x){
            switch (orientation){
                
                case 0: Turn90(0);
                break;
                
                case 2: Turn90(1);
                break;
                
                case 3: Turn90(1);
                __delay_ms(5);
                Turn90(1);
                break;
            }
            Drive1m();
        }
           
        
        else if (pathShortest[0][n] < x){
             switch (orientation){
                
                case 0: Turn90(1);
                break;
                
                case 2: Turn90(0);
                break;
                
                case 1: Turn90(1);
                __delay_ms(5);
                Turn90(1);
                break;
            }
            Drive1m();
        }
        
        else if(pathShortest[1][n] > y){
              switch(orientation){
                
                case 1: Turn90(0);
                break;
                
                case 3: Turn90(1);
                break;
                
                case 0: Turn90(1);
                __delay_ms(5);
                Turn90(1);
                break;
            }
            Drive1m;
        }
        
        
        else if (path[1][n] < y){
               switch (orientation){
                
                case 1: Turn90(1);
                break;
                
                case 3: Turn90(0);
                break;
                
                case 2: Turn90(1);
                __delay_ms(5);
                Turn90(1);
                break;
            }
            Drive1m;
        }       
    }    
}





void interrupt isr(void){
    if(TMR0IF){
        TMR0IF = 0;
        TMR0 = TMR0_VAL;
        time_count++;
        
        if(PB8)
            PB8Counter++;
        if(PB7)
            PB7Counter++;
        
            
        
        
    }
}


void main(void){
    
    __delay_ms(5000);
    
    ser_init();
    setupSPI();
    setupLCD();
    setupADC();
    
    TRISB = 0b00000011;
    PORTB = 0;
    
    unsigned char controlByte = 0b00001101;
    spi_transfer(controlByte);
   
    __delay_ms(1000);
    ser_putch(128);     //Startup
    __delay_ms(1000);
    ser_putch(132);     //Full mode
    __delay_ms(1000);
    
    
    x = 1;
    y = 0;
    orientation = 3;
    
        
    while(1){
        
        if(PB8Counter >= 10 && PB8 == 0){
            
            
            RB4 = 1;
            PathTo(2,1);
            __delay_ms(5000);
            TravelPath();
            RB4 = 0;
/*    
            lcdWriteControl(0b00000001);       
            lcdSetCursor(0b10000000);
            lcdWriteString("X:");
            lcdSetCursor(0b11000000);
            lcdWriteString("Y:");
            
            for (char n = 0; n <= pathCountShortest; n++){       
                lcdSetCursor(0b10000000 + n + 2);
                lcdWriteData(pathShortest[0][n] + 48);      
                lcdSetCursor(0b11000000 + n + 2);
                lcdWriteData(pathShortest[1][n] + 48);                
            }
            lcdSetCursor(0b10001110);
            lcdWriteToDigitBCD(pathCountShortest);
*/
        }
        
        
        
        
        
    }        
}
    

