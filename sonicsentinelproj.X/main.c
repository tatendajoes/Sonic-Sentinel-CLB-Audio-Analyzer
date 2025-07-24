 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
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
#include "mcc_generated_files/system/system.h"
#include <stdint.h>
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"        // your fix_fft()
#include <stdbool.h>
#include "detect.h"


/*
    Main application
*/
// ======================================== ADC Declarations ===============================================================

#define NO_SAMPLES 64

volatile adc_result_t adcVal;
volatile uint16_t     indexData  = 0;
volatile bool         bufferFull = false;

int16_t realNumbers[NO_SAMPLES];
int16_t imaginaryNumbers[NO_SAMPLES];

// === ADC Callback: Samples audio data at each TMR1 tick ===
void adc_callback(void)
{
    adcVal = ADC_ConversionResultGet();
    realNumbers[indexData]      = (int16_t)adcVal - 388;  // DC offset correction
    imaginaryNumbers[indexData] = 0;
    indexData++;

    if (indexData >= NO_SAMPLES) {
        bufferFull = true;
        indexData  = 0;
        TMR1_Stop();  // Stop sampling timer until we process
    }
}

// === Integer square root calculation for FFT magnitude ===
void Square_Root(int16_t real[], int16_t imag[])
{
    for (uint8_t i = 0; i < NO_SAMPLES / 2; i++) {
        uint32_t val = (uint32_t)real[i] * real[i]
                     + (uint32_t)imag[i] * imag[i];
        uint16_t root = 0;
        uint32_t bit  = 1UL << 30;
        while (bit > val)       bit >>= 2;
        while (bit != 0) {
            if (val >= root + bit) {
                val  -= root + bit;
                root  = (root >> 1) + bit;
            } else {
                root >>= 1;
            }
            bit >>= 2;
        }
        real[i] = (int16_t)root;
    }
}

//===================================================================================================

// == Button Interrupt Handling =====================================================================
volatile uint8_t buttonPressed1 = 0;
//static const char msg[] = "Hello, world! \r\n";
void myButtonHandler1(void){
    if (buttonPressed1 == 1){
        buttonPressed1 = 0;
    }else{
        buttonPressed1 = 1;
    }
}

volatile uint8_t buttonPressed2 = 0;
void myButtonHandler2(void){
    if (buttonPressed2 == 1){
        buttonPressed2 = 0;
    }else{
        buttonPressed2 = 1;
    }
}

// ==========================================================================================================================

int main(void)
{
    SYSTEM_Initialize();
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts 
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts 
    // Use the following macros to:
    
    // 1) Hook up ADC done ? our callback
    ADC_ConversionDoneCallbackRegister(adc_callback);
    
    // initialize detect
    detect_init(); 

    // Enable the Global Interrupts 
    INTERRUPT_GlobalInterruptEnable(); 
    
    //Start Timer
    TMR2_Start();
     TMR1_Start();
    __delay_ms(20);    // let a few samples queue up (optional)


    // Disable the Global Interrupts 
    //INTERRUPT_GlobalInterruptDisable(); 

    // Enable the Peripheral Interrupts 
    INTERRUPT_PeripheralInterruptEnable(); 

    // Disable the Peripheral Interrupts 
    //INTERRUPT_PeripheralInterruptDisable(); 
    IO_RC2_SetInterruptHandler(&myButtonHandler1);
    IO_RC3_SetInterruptHandler(&myButtonHandler2);
    int clbState =1;
    CLB1_SWIN_Write8(clbState);
    while(1)
    {
        if (bufferFull) {
            // FFT transform (in-place)
            fix_fft(realNumbers, imaginaryNumbers, 6);
            // Compute magnitudes
            Square_Root(realNumbers, imaginaryNumbers);

            // Detect gun?shot via spectral metrics
            if (detect_shot(realNumbers)) {
                clbState = 0;  // alarm state
            }

            // Clear flag & restart sampling
            bufferFull = false;
            TMR1_Start();
        }

        // === Process emergency button ===
        if (buttonPressed1) {
            clbState = 0;
            buttonPressed1 = 0;
        }
        // === Process reset button ===
        if (buttonPressed2) {
            clbState = 1;
            buttonPressed2 = 0;
        }

        // Drive CLB with current state
        CLB1_SWIN_Write8(clbState);
        if (!clbState) {
            printf("Shot Detected \r\n");
        }else{
           printf("Safe \r\n");
        }
    }    
}