/* 
 * File:   ahorasihay.c
 *
 * Created on 23 de octubre de 2015, 05:50 PM
 */
#define _XTAL_FREQ 8000000

#include <xc.h>

#include <plib.h>
#include <delays.h>

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSC_XT // Oscillator Selection bits (Internal oscillator, XT used by USB (INTXT))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

unsigned int a;
void InitializeBoard(void);
void lecturaSensor(void);
void reiniciaLectura(void);
void ConfigurePWM(void);

void interrupt myISR()
{
  if(INTCONbits.RBIF == 1)                 //Revisar si la interrupcion es de RB
  {
        INTCONbits.RBIF = 0;                     //Limpia bandera
        INTCONbits.RBIE = 0;                   //Desactivar la interrupcion de timer0 
    
    if(PORTBbits.RB4 == 1)                  // Si echo esta en altas
        {
        T1CONbits.TMR1ON = 1;                    //Start Timer
        }
         
    if(PORTBbits.RB4 == 0)                  //Si Echo es bajo
        {
        T1CONbits.TMR1ON = 0;                    // Detener el timer
        a = ((TMR1L | (TMR1H<<8))/58.81);  // Calcular distancia 
        }
  }
         INTCONbits.RBIE = 1;                     
}


void main()
{
    InitializeBoard();
    //// Introduccion al proyecto nombres y nombre del proyecto   
    // Se limpia el lcd y se comienza la lectura 
    while(1)
    {
      reiniciaLectura();
      lecturaSensor();
     Delay10KTCYx(100);

          
     
      if(a>=2 && a<=400)          //
      {
          if(a<= 10)
          {
                OpenPWM1(100);
                SetDCPWM1(50);
                PORTDbits.RD0 = 1;
          }
          else 
          {    
           ClosePWM1();
          PORTDbits.RD0 = 0;
          }
          
      }
   

         Delay10KTCYx(10);
    }
}

///////////////////// Activa la inicializacion de trigger y el apagado despues de un delay 
void lecturaSensor()
{
    PORTBbits.RB0 = 1;               //TRIGGER HIGH
    Delay10KTCYx(1);
    PORTBbits.RB0 = 0;               //TRIGGER LOW
}

//////////////////////// Reinicia la lectura del timer al inicializar 
void reiniciaLectura()
{       
      TMR1H = 0;                  //Reiniciar el valor leído del timer 
      TMR1L = 0;                  //Reiniciar el valor leído del timer
}

void InitializeBoard()
{
    //internal clock set to 8MHz
    OSCCONbits.IRCF0=1;
    OSCCONbits.IRCF1=1;
    OSCCONbits.IRCF2=1;   
    TRISB = 0b00010000;           //RB4 as Input PIN (ECHO)
    TRISD = 0x00;                      // LCD Pins as Output
    PORTD = 0x00;
    INTCONbits.GIE = 1;                      //Global Interrupt Enable
    INTCONbits.PEIE = 1;
    INTCONbits.RBIF = 0;                     //Clear PORTB On-Change Interrupt Flag
    INTCONbits.RBIE = 1;                     //Enable PORTB On-Change Interrupt
    ConfigurePWM();
    ClosePWM1();
    //Configuration of Timer1 interrupt
    OpenTimer1(TIMER_INT_ON & T1_16BIT_RW & T1_SOURCE_INT & T1_PS_1_8 & T1_OSC1EN_OFF & T1_SYNC_EXT_ON);
    WriteTimer1(3036);
    
    T1CON = 0x10;                 //Initialize Timer Module
}


void ConfigurePWM(void)
{
    unsigned char configTimer2 = TIMER_INT_OFF &
                                 T2_PS_1_4 &
                                 T2_POST_1_1;
    OpenTimer2(configTimer2);
    //define 150u and 75u for period and
    //duty cycle with a 4-MHz clock
    OpenPWM1(100);
    SetDCPWM1(50);
    //PWM output pin is RC2
    TRISCbits.TRISC2 = 0;
}