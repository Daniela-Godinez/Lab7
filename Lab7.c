/* 
 * File:   Lab7.c
 * Author: Daniela Godinez
 *
 * Created on 3 de abril de 2022, 22:29
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdint.h>

//Constantes
#define _t0_literal 246  //N para mi tiempo de desbordamiento de 5ms del TMR0

//Variables
unsigned char bandera; //Variable para cambiar de Display en el TMR0
unsigned char centena; //Variable para centenas en el Display
unsigned char decena; //Variable para decenas en el Display
unsigned char unidad; //Variable para unidades en el Display
unsigned char numero; //Variable para convertir los números binarios en el PORTA a decimales en el PORTC

/*
 * TABLA HEXADECIMAL
 */
char tabla[10] = {
    0B00111111, //0
    0B00000110, //1
    0B01011011, //2
    0B01001111, //3
    0B01100110, //4
    0B01101101, //5
    0B01111101, //6
    0B00000111, //7
    0B01111111, //8
    0B01101111, //9
    };

/*
 * INTERRUPCIONES
 */
void __interrupt() isr (void){
    if(INTCONbits.T0IF){
        TMR0 = _t0_literal;     //TMR0 es igual a _t0_literal
        switch(bandera){
            case 0:
                PORTD = 0;
                PORTDbits.RD0 = 1;
                PORTC = tabla[unidad];
                bandera++;
                break;
            case 1:
                PORTD = 0;
                PORTDbits.RD1 = 1;
                PORTC = tabla[decena];
                bandera++;
                break;
            case 2:
                PORTD = 0;
                PORTDbits.RD2 = 1;
                PORTC = tabla[centena];
                bandera = 0;
                break;
        }
        INTCONbits.T0IF = 0;    //Limpiamos bandera de TMR0
    }
    return;
}

/*
 * ENTRADAS Y SALIDAS
 */
void setup(void){
    ANSEL = 0;
    ANSELH = 0; //I-O Digitales
    
    TRISA = 0; //PORTA como salida
    TRISC = 0; //PORTC como salida
    TRISDbits.TRISD0 = 0; //RD0 salida para transistor
    TRISDbits.TRISD1 = 0; //RD1 salida para transistor
    TRISDbits.TRISD2 = 0; //RD2 salida para transistor
    
    TRISBbits.TRISB0 = 1; //RB0 entrada incremento
    TRISBbits.TRISB1 = 1; //RB1 entrada decremento
    
    OPTION_REGbits.nRBPU = 0; //Habilitación de PORTB pull-ups
    WPUBbits.WPUB0 = 1; //Habilitamos Pull-Up en RB0
    WPUBbits.WPUB1 = 1; //Habilitamos Pull-Up en RB1
    
    PORTA = 0; //Limpiamos PORTA
    PORTB = 0; //Limpiamos PORTB
    PORTC = 0; //Limpiamos PORTC
    PORTD = 0; //Limpiamos PORTD
    
    //CONFIGURACIÓN DE OSCILADOR
    OSCCONbits.IRCF = 0b0101; //2MHz
    OSCCONbits.SCS = 1;
    
    //CONFIGURACIÓN TMR0
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0; //Se asigno el prescaler para el TMR0
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1; //1:256
    
    TMR0 = _t0_literal;
    
    //HABILITACIÓN DE BANDERAS
    INTCONbits.GIE = 1;     //Se habilita las banderas globales
    INTCONbits.T0IE = 1;    //Habilita la interrupción del TMR0
    INTCONbits.T0IF = 0;    //Limpiamos la bandera
    
    return;
    
}

/*
 * FUNCIÓN DE CONVERSIÓN
 */
void conversion(void){
    numero = PORTA;
    centena = (numero/100);
    decena = ((numero-(centena*100))/10);
    unidad = (numero-((centena*100)+(decena*10)));
    return;
}

/*
 * MAIN
 */
int main(void) { //Función que empezará a ejecutar nuestro PIC al iniciar
    setup();    //Llamamos al setup ya que es necesario al inicializar mi PIC
    
    while(1){
        if(!PORTBbits.RB0){
            while(!RB0);
            PORTA++;
        }
        if(!PORTBbits.RB1){
            while(!RB1);
            PORTA--;
        }
        conversion();
    }
    
    return 1;
}

