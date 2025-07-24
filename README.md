# Sonic Sentinel Gunshot Detector

## Demo Video
[![Watch the SonicSentinel demo](https://img.youtube.com/vi/ycA4_98_dxw/hqdefault.jpg)](https://www.youtube.com/watch?v=ycA4_98_dxw)

# Introduction

Active shooter incidents often go undetected until after multiple rounds have been fired, leading to delayed responses and tragic outcomes. **SonicSentinel** addresses this challenge by providing a low-cost, battery-powered acoustic detection node built around the **PIC16F13145 Curiosity Nano**.

The system:

- Continuously monitors ambient sound through a **Max4466 microphone amplifier**  
- Processes incoming audio with a real-time **FFT** on the microcontroller  
- Uses simple visual feedback—**green** for safety, **red** for potential danger  

By leveraging optimized signal processing and the **Configurable Logic Block (CLB)**, SonicSentinel can detect the characteristic frequency signature of gunshots within milliseconds and notify nearby responders or trigger additional alerts.  

## Methodology

Four **PIC16F13145** peripherals and their drivers were used:

- **EUSART1**
- **Interrupts**
- **Configurable Logic Block** (clocked on Timer2)
- **ADC** (clocked on Timer1)

![PIC16F13145 Design Tree](/asserts/designtree.png)

*Figure 2.1: PIC16F13145 Design Tree.*  

## CLB Design

![CLB Design Flow](/asserts/clbdesign.png)

*Figure 2.2: CLB Design Flow*

The CLB takes input from the software-driven **CLBSWIN0**. When **CLBSWIN0** is true, the Safety LED indicators are turned on, and the simulation for the Danger Alert LED indicator array remains off. When **CLBSWIN0** is false, a Danger Alert is triggered and the Verilog simulation runs until **CLBSWIN0** returns to true by triggering the reset. Below is a State Transition Table for the State Machine Architecture on which the Verilog simulation code is built.  

![CLB Design Flow](/asserts/statemachine.png)

## Hardware Setup

- **Microphone:** A Max4466 amplifier module is connected to the PIC’s analog input (RA0) to capture sound pressure levels.  
- **Analog-to-Digital Conversion:** The built‑in ADC on the PIC16F13145 samples the audio signal.  
- **LED Indicator Arrays:** 10 GPIO pins drive 6 red and 4 green LEDs. Under normal conditions, the green LED remains lit. A detected gunshot or emergency button press triggers the red LED array simulation.  

![Schematic Diagram of the Hardware Setup Connection](/asserts/hardwaredesign.png)

*Figure 2.3: Schematic Diagram of the Hardware Setup Connection.*  

## FFT Implementation

Blocks of 64 audio samples were collected, and a standard Cooley–Tukey FFT run was performed to turn each time-domain frame into its frequency-domain representation. Because the input is real, only the first 32 bins actually carry unique information (the remaining bins mirror them), so the redundant half was discarded. These 32 bins gave an evenly spaced spectrum, from DC up to Nyquist. Lastly, a simple magnitude for each bin was computed, which became the basis for the detection logic.  

## Detection Algorithm

To reliably spot gunshots “brief, broadband impulses,” a two-metric test was used, tuned, and developed against test audio samples.

### Spectral Flux

Measures how much the spectrum jumps from one FFT frame to the next. Gunshots show a large, sudden spike; ordinary sounds (speech, music, ambient noise) tend to change more gradually. Using this formula:

$$
\text{Flux} = \sum_{k=0}^{31} \max\bigl(0,\,|X_k| - |X_k[n-1]|\bigr)
$$

The spectrum flux formula compares consecutive audio sample frames “X” to compute the sum of the difference between individual frequency bins, given that a jump is detected.

### Active‑Bin Count

Counts how many of the 32 bins exceed their “ambient-only” noise floor. Utilizing collected background data, a per‑bin threshold was set so that everyday noises rarely light up more than a handful of bins, whereas a gunshot lights up many at once.  

## Results

![Example Collected Ambient Noise Frequency Bins Data Samples](/asserts/ambient.png)

*Figure 3.1: Example Collected Ambient Noise Frequency Bins Data Samples* 

![Example Collected Gun Shot Detected Frequency Bins Data Samples](/asserts/gunshot.png)

*Figure 3.2: Example Collected Gun Shot Detected Frequency Bins Data Samples* 

![Example Collected Loud Music Frequency Bins Data Samples](/asserts/music.png)

*Figure 3.3: Example Collected Loud Music Frequency Bins Data Samples* 

## Resource Utilization

- **Program Memory:** 5,715 of 8,192 words used (~70%)  
- **Data Memory:** 503 of 1,024 bytes used (~49%)  

## Discussion

The decision for the detection algorithm used was mainly data-driven. For the purpose of demonstrating this project, a pistol sound from the Upbeat.io website was used as the gunshot sample—no actual firearm was used in the calibration or interpretation of the sampling data. However, the code structure was organized to allow easy recalibration for any alternative set of variables.

One of the major challenges during this project was finding the right documentation and learning resources, which slightly slowed progress. To overcome this, I relied heavily on the provided documentation and reviewed related project examples.

## Conclusion

SonicSentinel showcases how dynamic, real-time signal processing on a low‑cost microcontroller can deliver rapid, reliable gunshot detection with minimal hardware. The integration of a Max4466 microphone, optimized 64‑point FFT, clear LED indicators, and a UART peripheral provides an effective proof of concept.  
