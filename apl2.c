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
#define NO_LINES 2
#define NO_COLUMNS 16
int delay_step=0;                                                               //var globala folosita ca sa avem un delay mai mic sau mai mare



LED_show(int step)                                                              //more/less LEDS are turned on depending on the delay step
{                                                                               
    if(step==8) LATD=0xFF;
    if(step==7) LATD=0b11111110;
    if(step==6) LATD=0b11111100;
    if(step==5) LATD=0b11111000;
    if(step==4) LATD=0b11110000;
    if(step==3) LATD=0b11100000;
    if(step==2) LATD=0b11000000;
    if(step==1) LATD=0b10000000;
    if(step==0) LATD=0x0;
}

void delay_modify(int dirrection)                                               //modifies delay_step variable for longer/shorter delay
{   //dirrection: -1 lower delay  (press RB0)
               // +1 increment delay (press RA5))
    
    if(dirrection==1){
        if(delay_step != 8) delay_step++;
        LED_show(delay_step);
    }
    if(dirrection==-1){
        if(delay_step != 0) delay_step--;
        LED_show(delay_step);
     }         
}



void delay()                                                                    //call shorter/longer delay depending on the delay step;
{
    if(delay_step==0) __delay_ms(140);                                       //25                      
    if(delay_step==1) __delay_ms(125);                                       //30
    if(delay_step==2) __delay_ms(110);                                       //35
    if(delay_step==3) __delay_ms(95);
    if(delay_step==4) __delay_ms(70);
    if(delay_step==5) __delay_ms(55);
    if(delay_step==6) __delay_ms(40);
    if(delay_step==7) __delay_ms(25);
    if(delay_step==8) __delay_ms(10);
    
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
    //__delay_ms(25);
    delay();
    GPIO_set (GPIOB_address,c);
   // __delay_ms(25);
    delay();
    GPIO_set (GPIOA_address,0x40);
   // __delay_ms(25);
    delay();
    GPIO_set (GPIOA_address,0x00);
    //__delay_ms(25);  
    delay();
}

void LCD_init()
{
    IODIR_set(IODIRA_address,0x00);
    IODIR_set(IODIRB_address,0x00);
    LCD_cmd(0b00111111);
    LCD_cmd(0b00001111);
    LCD_cmd(0b00000001);
    LCD_cmd(0b00000110);
}

void LCD_out(char litera, int coloana, int linie)
{
    if(coloana==1)
        LCD_cmd(coloana1_inc+linie);
    if(coloana==2)
        LCD_cmd(coloana2_inc+linie);
    GPIO_set(GPIOA_address,0x80);
    //__delay_ms(25);
    delay();
    GPIO_set(GPIOB_address,litera);
    //__delay_ms(25);
    delay();
    GPIO_set(GPIOA_address,0xc0);
    //__delay_ms(25);
    delay();
    GPIO_set(GPIOA_address,0x00);
    //__delay_ms(25);
    delay();
}

void port_set( int PORT_no, int PORT_value, int PORT_dir)                       //setarea unui bit din portul A,B,D ca input(PORT_dir=1) sau ca output(PORT_dir=0)
{
    // PORT_no= numele portului dorit: A(1) /B(2)/D(4)
    // PORT_value=  Bitul X (0<=x<=7) al portului      
    // PORT_dir= Port de intrare(1) sau iesire(o)
    if(PORT_no==4) if (PORT_dir==1) { TRISD=TRISD | (1<<PORT_value);}
                   else { TRISD=TRISD & ~(1<<PORT_value);}
    if(PORT_no==1) if (PORT_dir==1) { TRISA=TRISA | (1<<PORT_value);}
                   else { TRISA=TRISA & ~(1<<PORT_value);}
    if(PORT_no==2) if (PORT_dir==1) { TRISB=TRISB | (1<<PORT_value);}
                   else { TRISB=TRISB & ~(1<<PORT_value);} 
}

void port_reset(int PORT_no)                                                    //resetarea porturilor
{
    if(PORT_no == 1 ) {TRISA=0x00; LATA=0x0;}
    if(PORT_no == 2 ) {TRISB=0x00; LATB=0x0;}
    if(PORT_no == 4 ) {TRISD=0x00; LATD=0x0;}
}

void animation()
{   
    int column;   
    for(column=1; column <= NO_COLUMNS; column++)
    {
        LCD_out('>',1,column);
       // LED_show(delay_step);
        if(RA5==0) delay_modify(1);
        if(RB0==0) delay_modify(-1);
    }
    for(column=NO_COLUMNS; column >= 1; column--)
    {
        LCD_out('<',2,column);
        //LED_show(delay_step);
        if(RA5==0) delay_modify(1);
        if(RB0==0) delay_modify(-1);
    }
    LCD_cmd(LCD_clear);
    
    
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
    while (1){
       animation();
        
    }
    return;
}
