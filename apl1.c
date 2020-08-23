#include <spi.h>
#pragma config WDT=OFF
#pragma config OSC=HS
#pragma config FCMEN=OFF
#pragma config XINST=OFF
#include <xc.h>
#include<string.h>

#define IODIRA_address 0x00
#define IODIRB_address 0x01
#define GPIOA_address 0x12
#define GPIOB_address 0x13
#define CS PORTAbits.RA2
#define _XTAL_FREQ 4000000
#define LCD_clear 0b00000001
#define LCD_cursor_off 0b0001111
#define LCD_set 0b00111011
#define LCD_entry 0b00000110
#define coloana1_inc 128
#define coloana2_inc 192

#define word_left "STANGA"                                                      //word that will be used for the "left" animation
#define left_offset 3                                                          //column offset for word_left

#define word_right "DREAPTA"                                                    //word that will be used for the "right" animation
#define right_offset 3                                                          //column offset for word_right


#define delay 20


int pressed=0;                                                                  //variable used to check if any of the 2 buttons (RB0/RA5) were pressed 
void right_animation(); 
void left_animation();
void LCD_out(char,int,int);
void LCD_cmd(char);

void left_sequence(int sequence_no)                                           //pt a activa urmatorul led necesar pt animatie
{  
    if(sequence_no == 1) LATD0=1;
    if(sequence_no == 2) LATD1=1;
    if(sequence_no == 3) LATD2=1;
    if(sequence_no == 4) LATD3=1;
    if(sequence_no == 5) LATD4=1;
    if(sequence_no == 6) LATD5=1;
    if(sequence_no == 7) LATD6=1;
    if(sequence_no == 8) { LATD7=1; LATD0=0;}
    if(sequence_no == 9) LATD1=0;
    if(sequence_no == 10) LATD2=0;
    if(sequence_no == 11) LATD3=0;
    if(sequence_no == 12) LATD4=0;
    if(sequence_no == 13) LATD5=0;
    if(sequence_no == 14) LATD6=0;
    if(sequence_no == 15) LATD7=0;
}

void right_sequence(int sequence_no)                                          //idem secventa_stanga
{   
    if(sequence_no == 1) LATD7=1;
    if(sequence_no == 2) LATD6=1;
    if(sequence_no == 3) LATD5=1;
    if(sequence_no == 4) LATD4=1;
    if(sequence_no == 5) LATD3=1;
    if(sequence_no == 6) LATD2=1;
    if(sequence_no == 7) LATD1=1;
    if(sequence_no == 8) {LATD0=1; LATD7=0;}
    if(sequence_no == 9) LATD6=0;
    if(sequence_no == 10) LATD5=0;
    if(sequence_no == 11) LATD4=0;
    if(sequence_no == 12) LATD3=0;
    if(sequence_no == 13) LATD2=0;
    if(sequence_no == 14) LATD1=0;
    if(sequence_no == 15) LATD0=0;       
}

void left_animation()
{
    
    LCD_cmd(LCD_clear);                                                         //reset LCD
    LATD=0x00;                                                                  //Reset LEDs
   // char c[10]="STANGA";                                                        //cuvantul de afisat
    int word_length=strlen(word_left);                                          //word_length-the length of the char array used for the left animation
   // int offset_max = 16 - word_length + 1;  
    int offset = 16 - word_length + 1;                                          //the position(column) from where the word will begin be written on LCD
    int linie=1;                                                                //word_left will be written on the first line
    int column = offset + word_length -2;                                      //column where the characters of word_left will be written (first character is the last letter of the word )
    int i = word_length - 1;                                                    //i-th character of word_left ( goes backwards from n-1, n-2 .... 1, 0 ) 
    char letter;                                                                //letter that will be written on LCD
    int written=0;                                                              //condition to clear the display and re-write word_left with a new offset
    int sequence_no=1;                                                          //which LEDs will be turned on
    while(pressed)
    {                
        if(sequence_no==16) { sequence_no=1;                                    //if the LED animation sequence has finished, the animation will start again from the first sequence
                          //  __delay_ms(150); __delay_ms(150);                   //300ms delay
                            }
        left_sequence(sequence_no);                                             //Activate LEDs based on the current sequence
        sequence_no++;                                                          //increment the sequence number
       // if(RB0==0) { pressed=0;   break;  }
        if(RA5==0) {LCD_cmd(LCD_clear);  right_animation(); break; }            //if we press the other button we will start the right animation
        if(i==-1) written=1;                                                    //if we have written the whole word we will set the condition to re-initialize the offset for word_left when it will be written again
        if(offset<=0) {written=0; offset=16 - word_length + 1;   i=word_length - 1;   column=offset+word_length - 1;}         //if the offset got the its max value, we reinitialize parameters
        if (!written) {                                                         //if word_right hasn't been fully written on LCD 
                        letter=word_left[i];                                    //letter = i-th character of word_right 
                        LCD_out(letter,linie,column);                           //write on LCD the i-th character of word_right
                        
                        //if you need faster LED animation- add another left_sequence function call here
                        
                        
                        
                         if (i==0) LCD_out('<',2,column);                       //will display under the word an arrow like         |   LEFT_WORD    |
                         else LCD_out('-',2,column);                            //                                                  |   <--------    |
                        i--; column--;                                                                
                    }
        else {                                                                  //if word_right has been fully written on the LCD-reinitialize
            LCD_cmd(LCD_clear);                                                 //clear LCD
            offset-=left_offset;                                                //decrement the offset value
            written=0;                                                          //word_left is not written
            i = word_length - 1;                                                                //reinitialize i-th character for a new writing
            column = offset + word_length - 1;
            __delay_ms(2*delay);
        //    if(RB0==0) { pressed=0; break;LCD_cmd(0x00000001);  }
            if(RA5==0) { LCD_cmd(LCD_clear); right_animation(); break; }
            __delay_ms(2*delay);
          //  if(RB0==0) { pressed=0; break;  }
            if(RA5==0) { LCD_cmd(LCD_clear); right_animation(); break; }
            LCD_cmd(LCD_clear);                                                 //clear display(100ms)
        }
    }  
}

void right_animation()
{   
    LCD_cmd(LCD_clear);                                                         //clear the display
    LATD=0x00;                                                                  //clear the LEDs
    int word_length=strlen(word_right);                                         //word_length-the length of the char array used for the right animation
    int offset_max = 16 - word_length + 1;                                      //the max value that offset can get(so that the word can be written in the LCD)
    // char c[10]="DREAPTA";                                                       //cuvantul de afisat pe a 2-a linie a LCD-ului
    int offset=1;                                                               //the position(column) from where the word will begin be written on LCD
    int line=2;                                                                 //the word will be written on the second line of the LCD
    int column=offset;                                                          //column no. where a particular letter will be written
    int i=0;                                                                    //i-th character of word_right
    char letter;                                                                //character from word_right that will be written
    int written=0;                                                              //condition to clear the display and re-write word_right with a new offset
    int sequence_no=1;                                                          //which LED will be turned on
   
    while(pressed)
    {   
        if(sequence_no==16) { sequence_no=1;                                    //if the LED animation sequence has finished, the animation will start again from the first sequence                          
                          //  __delay_ms(150); __delay_ms(150);                   //300ms delay
                            }
        right_sequence(sequence_no);                                            //Activate LEDs based on the current sequence
        sequence_no++;                                                          //increment the sequence number
  //      if(RA5==0) { pressed=0; break; LCD_cmd(0x00000001);  }                  // daca apasam iar butonul pt animatiile de dreapta, resetam displayul si ledurile se opresc
        if(RB0==0) { LCD_cmd(LCD_clear);  left_animation(); break; }                    //if we press the other button we will start the left animation
        if(i==word_length) written=1;                                         //if we have written the whole word we will set the condition to re-initialize the offset for word-right when it will be written again
        if(offset>=offset_max) { written =0; offset =1; i=0; column=offset; }   //if the offset got the its max value, we reinitialize parameters       
        if(!written) {                                                          //if word_right hasn't been fully written on LCD
            letter=word_right[i];                                               //letter = i-th character of word_right 
            LCD_out(letter,line,column);                                        //write on LCD the i-th character of word_right
            
            
            //if you need faster LED animation- add another right_sequence function call here
            
            if (i==word_length-1) LCD_out('>',1,column);                        //will display above the word an arrow like         |     --------->    |
            else LCD_out('-',1,column);                                         //                                                  |     RIGHT_WORD    |
            i++;                                                                 
            column++;
        }
        else                                                                    //if word_right has been fully written on the LCD-reinitialize
        {
            LCD_cmd(LCD_clear);                                                 //clear LCD
            offset+=right_offset;                                               //increment the offset value
            written=0;                                                          //word_right is not written
            i=0;                                                                //i-th character of word_right
            column=offset;                                                      //column where the i-th character of word_right will be written
            __delay_ms(2*delay);     
        //    if(RA5==0) { pressed=0; break; LCD_cmd(0x00000001);  }                   // daca apasam iar butonul pt animatiile de dreapta, resetam displayul si ledurile se opresc
            if(RB0==0) { LCD_cmd(LCD_clear); left_animation(); break;  }                //if we press the other button we will start the left animation
            __delay_ms(2*delay);
        //    if(RA5==0) { pressed=0; break; LCD_cmd(0x00000001);  }                   // daca apasam iar butonul pt animatiile de dreapta, resetam displayul si ledurile se opresc
            if(RB0==0) { LCD_cmd(LCD_clear);left_animation(); break; }                  //if we press the other button we will start the left animation
           LCD_cmd(LCD_clear);
        }
        
    }
}

void GPIO_set(char address,char value)
{
    CS=0;
    WriteSPI1(0x40);
    WriteSPI1(address);
    WriteSPI1(value);
    CS=1;
}

void IODIR_set(char address,char dir)
{
    CS=0;
    WriteSPI1(0x40);
    WriteSPI1(address);
    WriteSPI1(dir);
    CS=1;
}

void LCD_cmd(char c)
{
    GPIO_set(GPIOA_address,0x00);
    __delay_ms(delay);
    GPIO_set (GPIOB_address,c);
    __delay_ms(delay);
    GPIO_set (GPIOA_address,0x40);
    __delay_ms(delay);
    GPIO_set (GPIOA_address,0x00);
    __delay_ms(delay);  
}

void LCD_init()
{
    IODIR_set(IODIRA_address,0x00);
    IODIR_set(IODIRB_address,0x00);
    LCD_cmd(0b00111111);                                                        //8bit data; 2 line display; 5x10 dots
    LCD_cmd(0b00001111);                                                        //display on cursor on blink on
    LCD_cmd(0b00000001);                                                        //clear display
    LCD_cmd(0b00000110);                                                        //entry mode
}

void LCD_out(char litera, int coloana, int linie)
{
    if(coloana==1)
        LCD_cmd(coloana1_inc+linie);
    if(coloana==2)
        LCD_cmd(coloana2_inc+linie);
    GPIO_set(GPIOA_address,0x80);
    __delay_ms(delay);
    GPIO_set(GPIOB_address,litera);
    __delay_ms(delay);
    GPIO_set(GPIOA_address,0xc0);
    __delay_ms(delay);
    GPIO_set(GPIOA_address,0x00);
    __delay_ms(delay);
}

void port_set( int PORT_no, int PORT_value, int PORT_dir)                       //set a bit from A,B,D port as input(PORT_dir=1) or as output(PORT_dir=0)
{
    // PORT_no=  A(1) /B(2)/D(4)
    // PORT_value=  X bit (0<=x<=7) of the port     
    // PORT_dir= input(1) or output (0) port
    if(PORT_no==4) if (PORT_dir==1) { TRISD=TRISD | (1<<PORT_value);}
                   else { TRISD=TRISD & ~(1<<PORT_value);}
    if(PORT_no==1) if (PORT_dir==1) { TRISA=TRISA | (1<<PORT_value);}
                   else { TRISA=TRISA & ~(1<<PORT_value);}
    if(PORT_no==2) if (PORT_dir==1) { TRISB=TRISB | (1<<PORT_value);}
                   else { TRISB=TRISB & ~(1<<PORT_value);} 
}

void port_reset(int PORT_no)                                                    //reset a port
{
    if(PORT_no == 1 ) {TRISA=0x00; LATA=0x0;}
    if(PORT_no == 2 ) {TRISB=0x00; LATB=0x0;}
    if(PORT_no == 4 ) {TRISD=0x00; LATD=0x0;}
}

void main(void) {
    TRISAbits.RA2=0;
    CS=1;
    OpenSPI1(SPI_FOSC_64,MODE_10,SMPEND);
    LCD_init();
    port_reset(1);                                                              // same as TRISA=0x00; LATA=0x00;
    port_reset(2);                                                              // same as TRISB=0x00; LATB=0x00;
    port_reset(4);                                                              // same as TRISD=0x00; LATD=0x00;
    port_set(2,0,1);                                                            // same as TRISB=0x01; 
    port_set(1,5,1);                                                            // same as TRISA=0x20;
    ADCON1bits.PCFG3=1;
    ADCON1bits.PCFG2=0;
    ADCON1bits.PCFG1=1;
    ADCON1bits.PCFG0=1;
    while(1){
    if(RB0==0 && pressed==0)
        { pressed=1; left_animation(); } 
        else { LATD=0x0; }
        if (RA5==0 && pressed==0)
        { pressed=1; right_animation(); } 
        else {LATD=0x0; }
    }   
    return;
}
