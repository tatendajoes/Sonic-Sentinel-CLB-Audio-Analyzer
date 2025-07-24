/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 3.0.0
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "../pins.h"

void (*IO_RC2_InterruptHandler)(void);
void (*IO_RC3_InterruptHandler)(void);

void PIN_MANAGER_Initialize(void)
{
   /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x0;
    LATC = 0x0;

    /**
    TRISx registers
    */
    TRISA = 0x7;
    TRISB = 0x0;
    TRISC = 0x2C;

    /**
    ANSELx registers
    */
    ANSELA = 0x7;
    ANSELB = 0x0;
    ANSELC = 0x0;

    /**
    WPUx registers
    */
    WPUA = 0x0;
    WPUB = 0x0;
    WPUC = 0xC;
  
    /**
    ODx registers
    */
   
    ODCONA = 0x0;
    ODCONB = 0x0;
    ODCONC = 0x0;
    /**
    SLRCONx registers
    */
    SLRCONA = 0x37;
    SLRCONB = 0xF0;
    SLRCONC = 0xFF;
    /**
    INLVLx registers
    */
    INLVLA = 0xE;
    INLVLB = 0x0;
    INLVLC = 0x0;

    /**
    PPS registers
    */
    RX1PPS = 0x15; //RC5->EUSART1:RX1;
    RA4PPS = 0x24;  //RA4->CLB1:CLBPPSOUT0;
    RC1PPS = 0x24;  //RC1->CLB1:CLBPPSOUT0;
    RA5PPS = 0x25;  //RA5->CLB1:CLBPPSOUT1;
    RC0PPS = 0x25;  //RC0->CLB1:CLBPPSOUT1;
    RB5PPS = 0x26;  //RB5->CLB1:CLBPPSOUT2;
    RB7PPS = 0x27;  //RB7->CLB1:CLBPPSOUT3;
    RC7PPS = 0x28;  //RC7->CLB1:CLBPPSOUT4;
    RC6PPS = 0x29;  //RC6->CLB1:CLBPPSOUT5;
    RB6PPS = 0x2A;  //RB6->CLB1:CLBPPSOUT6;
    RB4PPS = 0x2B;  //RB4->CLB1:CLBPPSOUT7;
    RC4PPS = 0x13;  //RC4->EUSART1:TX1;

    /**
    APFCON registers
    */

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x0;
    IOCAF = 0x0;
    IOCBP = 0x0;
    IOCBN = 0x0;
    IOCBF = 0x0;
    IOCCP = 0x0;
    IOCCN = 0xC;
    IOCCF = 0x0;

    IO_RC2_SetInterruptHandler(IO_RC2_DefaultInterruptHandler);
    IO_RC3_SetInterruptHandler(IO_RC3_DefaultInterruptHandler);

    // Enable PIE0bits.IOCIE interrupt 
    PIE0bits.IOCIE = 1; 
}
  
void PIN_MANAGER_IOC(void)
{
    // interrupt on change for pin IO_RC2}
    if(IOCCFbits.IOCCF2 == 1)
    {
        IO_RC2_ISR();  
    }
    // interrupt on change for pin IO_RC3}
    if(IOCCFbits.IOCCF3 == 1)
    {
        IO_RC3_ISR();  
    }
}
   
/**
   IO_RC2 Interrupt Service Routine
*/
void IO_RC2_ISR(void) {

    // Add custom IOCCF2 code

    // Call the interrupt handler for the callback registered at runtime
    if(IO_RC2_InterruptHandler)
    {
        IO_RC2_InterruptHandler();
    }
    IOCCFbits.IOCCF2 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF2 at application runtime
*/
void IO_RC2_SetInterruptHandler(void (* InterruptHandler)(void)){
    IO_RC2_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF2
*/
void IO_RC2_DefaultInterruptHandler(void){
    // add your IO_RC2 interrupt custom code
    // or set custom function using IO_RC2_SetInterruptHandler()
}
   
/**
   IO_RC3 Interrupt Service Routine
*/
void IO_RC3_ISR(void) {

    // Add custom IOCCF3 code

    // Call the interrupt handler for the callback registered at runtime
    if(IO_RC3_InterruptHandler)
    {
        IO_RC3_InterruptHandler();
    }
    IOCCFbits.IOCCF3 = 0;
}

/**
  Allows selecting an interrupt handler for IOCCF3 at application runtime
*/
void IO_RC3_SetInterruptHandler(void (* InterruptHandler)(void)){
    IO_RC3_InterruptHandler = InterruptHandler;
}

/**
  Default interrupt handler for IOCCF3
*/
void IO_RC3_DefaultInterruptHandler(void){
    // add your IO_RC3 interrupt custom code
    // or set custom function using IO_RC3_SetInterruptHandler()
}
/**
 End of File
*/