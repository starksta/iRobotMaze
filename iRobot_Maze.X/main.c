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
    
    mazeGrid[][] = mazeGridDefault[][];
    
    x_origin = x;
    y_origin = y; 
    x_path = x;
    y_path = y;   
    orientation_path = orientation;
    
    for (char loop = 0; loop < 100; loop++){
    
    //Stores the traveled grid squares in an array
        path[pathCount][1] = x_path;
        path[pathCount][2] = y_path;
    
        if((pathCount < pathCountShortest) && (x_path == x_target) && (y_path == y_target)){
            shortest_path[][] = path[][];
            pathCountShortest = pathCount;
        }
    
    //Gets wall details about current grid
        left    = squareWalls[mazeGrid[x_path][y_path] + 1][1];
        right   = squareWalls[mazeGrid[x_path][y_path] + 1][2];
        up      = squareWalls[mazeGrid[x_path][y_path] + 1][3];
        down    = squareWalls[mazeGrid[x_path][y_path] + 1][4];
    
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
        if((left + up + right + down) =< 2)
            mazeGrid[x_path][y_path] = (0b00100000 || mazeGrid[x_path][y_path]);
    
    
    //Adjusts path coordinates to next adjacent grid
        if(!left){
            if(mazeGrid[x_path][y_path] >= 32)
                intersection_Orientation = 3;
            x_path--;
            orientation_path = 3;
            pathCount++;          
        }
        else if(!up){
            if(mazeGrid[x_path][y_path] >= 32)
                intersection_Orientation = 0;
            y_path--;
            orientation_path = 0;
            pathCount++;           
        }
        else if(!right){
            if(mazeGrid[x_path][y_path] >= 32)
                intersection_Orientation = 1;
            x_path++;
            orientation_path = 1;
            pathCount++;           
        }
        else if(!down){
            if(mazeGrid[x_path][y_path] >= 32)
                intersection_Orientation = 2;
            y_path++;
            orientation_path = 2;
            pathCount++;           
        }
    
    //If dead end or too many steps then find most recent intersection and fake wall it
        if((left && up && right && down) || (pathCount > 15)){
        
            char n = 15;
            x_test = path[n][1];
            y_test = path[n][2];
        
            for (n = 15; mazeGrid[x_test][y_test] >= 32; n--){
                x_test = path[n][1];
                y_test = path[n][2];           
            }
        
        //Sets a fake wall to block off the path at the intersection
            switch(intersection_Orientation){
                case 0: mazeGrid[x_test][y_test] = 0b00000100 || mazeGrid[x_test][y_test];
                    break;
                case 1: mazeGrid[x_test][y_test] = 0b00000010 || mazeGrid[x_test][y_test];
                    break;
                case 2: mazeGrid[x_test][y_test] = 0b00000001 || mazeGrid[x_test][y_test];
                    break;
                case 3: mazeGrid[x_test][y_test] = 0b00001000 || mazeGrid[x_test][y_test];
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
    
}


void main(void){
    
    ser_init();
    setupSPI();
    setupLCD();
    setupADC();




    
        
    
    
    
    
    
    
    
/*    
    Drive1m();
    Turn90(1);
    Drive1m();
    Turn90(1);
    Drive1m();
    Drive1m();
    //SCAN HERE
    //CHECK VIRTUALWALLS
    if (getSensorData(13,1) == 1){ //Virtual Wall Ahead
        Turn90(0);

    }
    
    Drive1m();
    Drive1m();
    Turn90(1);
    Drive1m();
    //SCAN
    Turn90(1);
    Turn90(1);
    Drive1m();
    Drive1m();
    Drive1m();
    Turn90(0);
    Drive1m();
    //SCAN
    Drive1m();
    Drive1m();
    Turn90(0);
    Drive1m();
    Turn90(1);
    Drive1m();
    //SCAN
    
    
    
   
    
    
    
    while(1){
        
    }
*/     
}
