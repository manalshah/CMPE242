CMPE242
=======

SJSU CMPE 242: Embedded Linux Hardward/Software

Collection of code written for class. There were 3 labs to be completed. The abstracts for the 3 labs are,

1. The purpose of this project is to write a device driver to control several of the general purpose I/O ports of the ARM11 processor on the the Tiny6410 board. The simple I/O is to turn on and off an LED on a separate prototype board and to read in the state of a switch on the same board as the LED.
2. The purpose of this project is to validate that the ADC for the ARM11 processor works properly and to find the conversion for the input voltages. A Fast Fourier Transform is used to validate the ADC by looking at the power spectrum of the values. The ADC is determined to be working and a manual inspection of the values shows a linear relationship
3. This paper describes a methodology for developing and implementing a PID controller for the ARM 11 Processor. A positional error is read via the ADC, the data is ran through a PID controller, and the output is via the PWM of the ARM 11. Furthermore, the positional error is processed by a FFT to verify the data from the ADC is valid. The PID controller is shown to work when the positional error reaches a minimum.

This code is provided AS-IS.
