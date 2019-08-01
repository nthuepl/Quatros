# Quatros
## Abstract
Hands-on experience is crucial to truly understanding the principles of operating systems (OS). This paper describes a preemptive,  multithreaded embedded operating system that was assigned as a five-part programming project in a junior-level OS course. It targets the popular 8051 instruction set architecture (ISA) and can run with as little as 128 bytes of RAM and 1 KB of program memory with preemption and synchronization primitives such as semaphores. It is written in C with embedded assembly language and compiled using the open-source Small Device C Compiler (SDCC). It runs on the free EdSim51 simulator, which interactively simulates common peripherals such as LCD, keypads, 7-segment LEDs, ADC, in addition to the built-in UART all in high fidelity and controllability. This course project has received positive feedback from students who took the course.
## Final Project
For final programming project , you are to (1) add a delay(n) function to your preemptive multithreading and semaphore code, (2) ensure your threads can terminate so its space can be recycled by another thread. (3) test your delay and thread recycling based on (but not identical) to the parking lot example you did earlier in the semester in Python.
## More Detail
Please reference this [link](https://docs.google.com/document/d/1He9Q3DOdXZlWhOoAbjGFRM9ZOgAOhHA14dM8W7DGvgM/edit).
