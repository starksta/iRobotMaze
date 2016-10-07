#include "main.h"
#include "motor.h"
#include "LCD.h"
#include "ADC.h"
#include "SPI.h"
#include "ser.h"



signed int distance = 0;
signed int angle = 0;


void wallFollow(void){
            
    
    unsigned int setADCdist = 150;
                    
           
    ADCMain();                                               
    if(adcRAW < (setADCdist - 40)) {
        DriveDirect(CURRENT_SPEED_R,CURRENT_SPEED_L);
    }
    if(adcRAW > setADCdist){   
        while(adcRAW > setADCdist){   
            ADCMain();
            ADC_ADJUST = adcRAW - setADCdist;
            ADJUSTED_SPEED_L = CURRENT_SPEED_L + (7/10)*ADC_ADJUST + (CURRENT_SPEED_L)/(ADC_ADJUST+10);
            DriveDirect(CURRENT_SPEED_R,ADJUSTED_SPEED_L);       //make left faster  
        }      
    } 
    else if (adcRAW < setADCdist){
        while(adcRAW < setADCdist){ 
            ADCMain();
            ADC_ADJUST = setADCdist - adcRAW;
            ADJUSTED_SPEED_R = CURRENT_SPEED_R + (7/10)*ADC_ADJUST + (CURRENT_SPEED_R)/(ADC_ADJUST+10);
            DriveDirect(ADJUSTED_SPEED_R,CURRENT_SPEED_L);       //make right faster  
            __delay_ms(100);   
        }
    }
	//DriveDirect(CURRENT_SPEED_R,CURRENT_SPEED_L);
}
    


void PathTo(char x_target, char y_target){
    
    
    //mazeGrid = mazeGridDefault
    for (r = 0; r <= 3; r++){        
        for (c = 0; c <= 4; c++){
            mazeGrid[r][c] = mazeGridDefault[r][c];
        }
    }
    
    pathCountShortest = 20;
    for (r = 0; r <= 1; r++){        
        for (c = 0; c <= 15; c++){
            pathShortest[r][c] = 0;
        }
    }
    
    x_goto = x_target;
    y_goto = y_target;
    
    x_path = x;
    y_path = y;   
    orientation_path = orientation;
    
    pathCount = 0;
    
    
    lcdSetCursor(0b10000000);
    lcdWriteString("Working...");
    
    for (int loop = 0; loop <= 2000; loop++){
        
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
        if (pathCount != 0){
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
        if ((x_path == x_target)){ 
            if (y_path == y_target){
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
//------------------------------------------------------------------------------    
    lcdSetCursor(0b10000000);                                                   //Prints the Shortest Path on the LCD        
        for (c = 0; c <= 15; c++){
            lcdWriteData(pathShortest[0][c]+48);
        }
    lcdSetCursor(0x40);
        for (c = 0; c<=15; c++){
            lcdWriteData(pathShortest[1][c]+48);
    }
//------------------------------------------------------------------------------    
}


void Drive1m(void){   
    
    //Ride of the Valkyries
//    ser_putch(141);				//Play
//    ser_putch(3);				//Song 3         
//    ser_putch(141);				//Play
//    ser_putch(4);				//Song 4
    
    distance = 0;
    DriveDirect(250,250);
    RB2 = 1;
    RB3 = 1;
   // for (char m = 0; m <= 10; m++){
   
    while (distance < 1000){
        
       // wallFollow();
        distance = distance + getSensorData(19,2);
      
        if ((getSensorData(13,1)) == 1){  //If virtual wall or home-base force field sensor triggered. Maybe we could test in turn 90 as well.
                 
            if((getSensorData(17,1)) != 242){                                   //If not the force-field. Used to be <240
                IR_Wall = 1;   
                distance = 1001;
                RB3 = 0;
            }
            
            
            unsigned char omni_IR = (getSensorData(17,1));
            if (omni_IR == 250 || omni_IR == 246){                              // If forcefield and green or red buoy.   
             
                targets++;
            
                switch(targets){
                    case 1: ser_putch(141);
                        ser_putch(5);
                        break;
                    case 2: ser_putch(141);
                        ser_putch(3);
                        targets_found = 1;
                        break;                          
                }
            
            __delay_ms(2000);
                  
            }
             
        }
    }

     
    DriveDirect(0,0);
    
    if(IR_Wall == 0){
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
    __delay_ms(100);
    }
    
    // if (getSensorData(7,1) == (0b00000010 || 0b00000001)){
    //    DriveDirect(0,0);         
    //}
    if (targets_found == 1){
        char n = 20;
    }
}


//Turns 90 degrees, direction( 1 = CCW, 0 = CW)
void Turn90(char direction){
    
    angle = 0;
    
    if (direction == 1){
        
        if (orientation == 0)
                orientation = 3;
        else orientation--;
        
        Drive(0,150,0x00,0x01);
        while (angle < 86){
            angle = angle + getSensorData(20,2);
        }
    }
    
    if (direction == 0){
        
        if (orientation == 3)
            orientation = 0;
        else orientation++;
        
        Drive(0,150,0xFF,0xFF);
        while (angle > -86){
            angle = angle + getSensorData(20,2);
        }    
    }
    DriveDirect(0,0);
    __delay_ms(100);   
}

void TravelPath(void){
                    
for (char n = 1; n <= (pathCountShortest); n++){
        
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
            Drive1m();
        }
        
        else if (pathShortest[1][n] < y){     
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
            Drive1m();
        }
        
        
        if (IR_Wall == 1){
        n = 20;     //break from for loop
        }      
    }
}    

void re_route(void){
    switch(orientation){
            case 0: (mazeGridDefault[y][x]) = ((mazeGridDefault[y][x]) + 4);                             
                break;
            case 1: (mazeGridDefault[y][x]) = ((mazeGridDefault[y][x]) + 2);
                break;
            case 2: (mazeGridDefault[y][x]) = ((mazeGridDefault[y][x]) + 1);
                break;
            case 3: (mazeGridDefault[y][x]) = ((mazeGridDefault[y][x]) + 8);
                break;                  
        }
                
        Turn90(0);   //Turn Around
        Turn90(0);
        distance = 0;
        DriveDirect(250,250);   //Move back to centre of grid
        while (distance < 500){
            distance = distance + getSensorData(19,2); 
        }
        DriveDirect(0,0);
        IR_Wall = 0;
            
        __delay_ms(1000);
        PathTo(x_goto,y_goto);
        TravelPath();            
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
    PORTB = 0b11111100;
    
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
    
    
    //Song 2
    ser_putch(140);
    ser_putch(2);
    ser_putch(10);
        ser_putch(67);				//G
        ser_putch(32);				//1/2
        ser_putch(77);				//F
        ser_putch(16);				//1/4
        ser_putch(77);				//F
        ser_putch(16);				//1/4
        ser_putch(77);				//F
        ser_putch(16);				//1/4
        ser_putch(77);				//F
        ser_putch(16);				//1/4
        ser_putch(76);				//E
        ser_putch(16);				//1/4
        ser_putch(74);				//D
        ser_putch(16);				//1/4	
        ser_putch(72);				//C
        ser_putch(32);				//1/2	
        ser_putch(79);				//G
        ser_putch(16);				//1/4
        ser_putch(84);				//C
        ser_putch(48);				//3/4
        
     
    char first = 16;
    char second = 8;
    char third = 12;
    char fourth = 36;
    char fifth = 32;
    
    //Song 3 & 4 (Ride of the Valkyries)
    //Song 3
        ser_putch(140);
        ser_putch(3);
        ser_putch(16);
            ser_putch(62);  //D
            ser_putch(first);
            ser_putch(69);  //A
            ser_putch(second);
            ser_putch(62);  //D
            ser_putch(third);
            ser_putch(65);  //F
            ser_putch(fourth);
            ser_putch(62);  //D
            ser_putch(fifth);
            
            ser_putch(65);  //F
            ser_putch(first);
            ser_putch(62);  //D
            ser_putch(second);
            ser_putch(65);  //F
            ser_putch(third);
            ser_putch(69);  //A
            ser_putch(fourth);
            ser_putch(65);  //F
            ser_putch(fifth);
            
            ser_putch(69);  //A
            ser_putch(first);
            ser_putch(65);  //F
            ser_putch(second);
            ser_putch(69);  //A
            ser_putch(third);
            ser_putch(60);  //C
            ser_putch(fourth);
            ser_putch(48);  //C'
            ser_putch(fifth);
            
            ser_putch(65);  //F
            ser_putch(16);
                 
        //Song 4
        ser_putch(140);
        ser_putch(4);
        ser_putch(3);
            ser_putch(60);  //C
            ser_putch(12);
            ser_putch(65);  //F
            ser_putch(12);
            ser_putch(69);  //A
            ser_putch(96);
            
        //Song 5 & 6 (Mission Impossible)
        //Song 5 (high pitch rif)
        ser_putch(140);
        ser_putch(5);
        ser_putch(11);
            ser_putch(70);    //A#
            ser_putch(12);
            ser_putch(67);    //G
            ser_putch(12);
            ser_putch(62);    //D
            ser_putch(96);
            ser_putch(70);    //A#
            ser_putch(12);
            ser_putch(67);    //G
            ser_putch(12);
            ser_putch(61);    //C#
            ser_putch(96);
            ser_putch(70);    //A#
            ser_putch(12);
            ser_putch(67);    //G
            ser_putch(12);
            ser_putch(60);    //C
            ser_putch(96);
            ser_putch(70);    //A#
            ser_putch(12);
            ser_putch(60);    //C
            ser_putch(20);
        
       //Song 6 (base rif) 
        ser_putch(140);
        ser_putch(6);
        ser_putch(11);
            ser_putch(55);    //G
            ser_putch(22);
            ser_putch(55);    //G
            ser_putch(22);
            ser_putch(0);   //Rest
            ser_putch(2);
            ser_putch(58);    //A#
            ser_putch(18);
            ser_putch(48);    //C
            ser_putch(24);
            ser_putch(0);   //Rest
            ser_putch(10);
            ser_putch(55);    //G
            ser_putch(22);
            ser_putch(55);    //G
            ser_putch(22);  
            ser_putch(0);   //Rest
            ser_putch(2);
            ser_putch(53);    //F
            ser_putch(18);    
            ser_putch(54);    //F#
            ser_putch(24);    
            
            
            
            
                
            
            
    while(1){
        
        if(PB7Counter >= 10 && PB7== 0){
            
           //Mission Impossible
            ser_putch(141);				//Play
            ser_putch(6);				//Song 3 
            __delay_ms(3000);
            ser_putch(141);				//Play
            ser_putch(6);				//Song 3 
            __delay_ms(3000);
            ser_putch(141);				//Play
            ser_putch(5);				//Song 3 
            
        //    stepCW(200);
        //    __delay_ms(3);
        //    stepCCW(200);
            PB7Counter=0;
                   

            
        }
        
        
            
        
        if(PB8Counter >= 10 && PB8 == 0){
            
            
            PathTo(2,0);   
            TravelPath();           
            if (IR_Wall == 1)
                re_route();
            if (targets_found == 1){
                PathTo(1,0);
                TravelPath();
            }
  
        
            PathTo(3,0);         
            TravelPath();
            if (IR_Wall == 1)
                re_route();
            if (targets_found == 1){
                PathTo(1,0);
                TravelPath();
            }
            
            PathTo(0,3);
            TravelPath();
            if (IR_Wall == 1)
                re_route();
            if (targets_found == 1){
                PathTo(1,0);
                TravelPath();
            }
            
            PathTo(3,2);
            TravelPath();
            if (IR_Wall == 1)
                re_route();
            if (targets_found == 1){
                PathTo(1,0);
                TravelPath();
            }

         PB8Counter = 0;
        }
             
    }        
}
    
    

