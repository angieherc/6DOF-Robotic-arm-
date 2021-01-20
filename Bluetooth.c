/* 
 * File:   main_uart.c
 *
 * Created on 20 de octubre de 2015, 09:47 AM
 */

//Demonstration of a loopback with UART
//Simulation can be driven by a text file injected as a message to RCREG or
//as an asynchronous direct message selecting RCREG as a pin and providing the
//hexadecimal ascii code of the character to be injected

#include <xc.h>
#include <plib.h>
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define _XTAL_FREQ 4000000ul
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

void InitializeBoard(void);
void ConfigUART(void);
void ConfigurePWM(void);

unsigned char userFlag = 0;
unsigned char dataUSART;


void interrupt myISR(void)
{
    if(PIE1bits.RCIE && PIR1bits.RCIF)
    {
        //PIR1bits.RCIF = 0;
        dataUSART = ReadUSART();
        userFlag = 1;
        Nop();
    }
    
}

void main (void)
{
    InitializeBoard();
    //welcome message
    // If USART is busy sending bits, program will hold. 
    // 
    while(BusyUSART());
    //putrsUSART( "\r\nHello World!" );

    while(1)
    {
        if(userFlag == 1)
        {
            //clear user's flag and return received character
            userFlag = 0;
            while(BusyUSART());      
            WriteUSART(dataUSART);
            __delay_ms(1);  
            PORTDbits.RD2 = 1;
            PORTDbits.RD3 = 1;
                 
        }
        PORTDbits.RD2 = 0;
        PORTDbits.RD3 = 0;
        
    }
}

void InitializeBoard(void)
{
    //internal clock set to 4MHz
    OSCCONbits.IRCF0=0;
    OSCCONbits.IRCF1=1;
    OSCCONbits.IRCF2=1;

    TRISDbits.RD2 = 0;
    TRISDbits.RD3 = 0;
  
    
    PORTDbits.RD2 = 0;
    PORTDbits.RD3 = 0;
    
    
    ConfigUART();
    ConfigurePWM();
    INTCONbits.PEIE = 1;
    ei();
}

void ConfigUART(void)
{
    unsigned char configUSART = USART_TX_INT_OFF &
                                USART_RX_INT_ON &
                                USART_ASYNCH_MODE &
                                USART_EIGHT_BIT &
                                //USART_SYNC_SLAVE &
                                USART_CONT_RX &
                                USART_BRGH_HIGH;
                                //USART_ADDEN_OFF;
    unsigned int spbrg = 25; //9600 bauds in high-speed mode
    OpenUSART(configUSART, spbrg);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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