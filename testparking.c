/*
* file: testpreempt.c
*/
#include <8051.h>
#include "preemptive.h"
/*
* @@@ [2pt]
* declare your global variables here, for the shared buffer
* between the producer and consumer.
* Hint: you may want to manually designate the location for the
* variable. you can use
* __idata __at (0x30) type var;
* to declare a variable var of the type
*/

/*#define EnqueueBuf(s) \
buf[buf_tail] = s; \
buf_tail++; \
if (buf_tail == 3) \
    buf_tail = 0;

#define DequeueBuf(s) \
s = buf[buf_head]; \
buf_head++; \
if (buf_head == 3) \
    buf_head = 0;*/

/* [8 pts] for this function
* the producer in this test program generates one characters at a
* time from 'A' to 'Z' and starts from 'A' again. The shared
buffer
* must be empty in order for the Producer to write.
*/
//void Producer1( void ) {
    ///*
    //* @@@ [2 pt]
    //* initialize producer data structure, and then enter
    //* an infinite loop (does not return)
    //*/
    //buf1_prev = 'A' - 1;
    //while (1) {
        ///* @@@ [6 pt]
        //* wait for the buffer to be available,
        //* and then write the new data into the buffer */
        //SemaphoreWait(empty)
        //SemaphoreWait(mutex)
        //__critical {
            //buf1_prev++;
            //if (buf1_prev > 'Z')
                //buf1_prev = 'A';
            //EnqueueBuf(buf1_prev)
        //}
        //SemaphoreSignal(mutex)
        //SemaphoreSignal(full)
    //}
//}

//void Producer2( void ) {
    ///*
    //* @@@ [2 pt]
    //* initialize producer data structure, and then enter
    //* an infinite loop (does not return)
    //*/
    //buf2_prev = '0' - 1;
    //while (1) {
        ///* @@@ [6 pt]
        //* wait for the buffer to be available,
        //* and then write the new data into the buffer */
        //SemaphoreWait(empty)
        //SemaphoreWait(mutex)
        //__critical {
            //buf2_prev++;
            //if (buf2_prev > '9')
                //buf2_prev = '0';
            //EnqueueBuf(buf2_prev)
        //}
        //SemaphoreSignal(mutex)
        //SemaphoreSignal(full)
    //}
//}
/* [10 pts for this function]
* the consumer in this test program gets the next item from
* the queue and consume it and writes it to the serial port.
* The Consumer also does not return.
*/
//void Consumer( void ) {
    //while (1) {
        ///* @@@ [2 pt] wait for new data from producer
        //* @@@ [6 pt] write data to serial port Tx,
        //* poll for Tx to finish writing (TI),
        //* then clear the flag
        //*/
        //SemaphoreWait(full)
        //SemaphoreWait(mutex)
        //__critical {
            //DequeueBuf(SBUF)
        //}
        //SemaphoreSignal(mutex)
        //SemaphoreSignal(empty)
        //while (!TI) {}
        //TI = 0;
    //}
//}
/* [5 pts for this function]
* main() is started by the thread bootstrapper as thread-0.
* It can create more thread(s) as needed:
* one thread can acts as producer and another as consumer.
*/
void Park1() {
    SemaphoreWait(sem_parking_lot)
    // There are empty spots
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (spots[0] == 0) {
        spots[0] = 1;
        //space1 = 0;
    } else if (spots[1] == 0) {
        spots[1] = 1;
        //space1 = 1;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    //enter1 = now();
    delay(rand());
    exit1 = now();
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (space1 == 0) {
        spots[0] = 0;
    } else if (space1 == 1) {
        spots[1] = 0;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    SemaphoreSignal(sem_parking_lot)
}
void Park2() {
    SemaphoreWait(sem_parking_lot)
    // There are empty spots
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (spots[0] == 0) {
        spots[0] = 1;
        space2 = 0;
    } else if (spots[1] == 0) {
        spots[1] = 1;
        space2 = 1;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    enter2 = now();
    delay(rand());
    exit2 = now();
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (space2 == 0) {
        spots[0] = 0;
    } else if (space2 == 1) {
        spots[1] = 0;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    SemaphoreSignal(sem_parking_lot)
}
void Park3() {
    SemaphoreWait(sem_parking_lot)
    // There are empty spots
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (spots[0] == 0) {
        spots[0] = 1;
        space3 = 0;
    } else if (spots[1] == 0) {
        spots[1] = 1;
        space3 = 1;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    enter3 = now();
    delay(rand());
    exit3 = now();
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (space3 == 0) {
        spots[0] = 0;
    } else if (space3 == 1) {
        spots[1] = 0;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    SemaphoreSignal(sem_parking_lot)
}
void Park4() {
    SemaphoreWait(sem_parking_lot)
    // There are empty spots
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (spots[0] == 0) {
        spots[0] = 1;
        space4 = 0;
    } else if (spots[1] == 0) {
        spots[1] = 1;
        space4 = 1;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    enter4 = now();
    delay(rand());
    exit4 = now();
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (space4 == 0) {
        spots[0] = 0;
    } else if (space4 == 1) {
        spots[1] = 0;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    SemaphoreSignal(sem_parking_lot)
}
void Park5() {
    SemaphoreWait(sem_parking_lot)
    // There are empty spots
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (spots[0] == 0) {
        spots[0] = 1;
        space5 = 0;
    } else if (spots[1] == 0) {
        spots[1] = 1;
        space5 = 1;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    enter5 = now();
    delay(rand());
    exit5 = now();
    SemaphoreWait(sem_spots_sync)
    // Modify spots
    if (space5 == 0) {
        spots[0] = 0;
    } else if (space5 == 1) {
        spots[1] = 0;
    } else {
        while(1);
    }
    SemaphoreSignal(sem_spots_sync)
    SemaphoreSignal(sem_parking_lot)
}

#define print(c) \
SBUF = c; \
while (!TI) {} \
TI = 0;

#define log_3digits(num) \
if (num >= 200) uart = '2'; \
else if (num >= 100) uart = '1'; \
else uart = '0'; \
print(uart) \
uart = num; \
while (uart >= 100) uart -= 100; \
if (uart >= 90) uart = '9'; \
else if (uart >= 80) uart = '8'; \
else if (uart >= 70) uart = '7'; \
else if (uart >= 60) uart = '6'; \
else if (uart >= 50) uart = '5'; \
else if (uart >= 40) uart = '4'; \
else if (uart >= 30) uart = '3'; \
else if (uart >= 20) uart = '2'; \
else if (uart >= 10) uart = '1'; \
else uart = '0'; \
print(uart) \
uart = num; \
while (uart >= 10) uart -= 10; \
uart += '0'; \
print(uart)

#define log(num, space, from, to) \
        EA = 0; \
        print('C') \
        print('a') \
        print('r') \
        uart = num + '0'; \
        print(uart) \
        print(' ') \
        print('a') \
        print('t') \
        print(' ') \
        log_3digits(from) \
        print(' ') \
        print('o') \
        print('c') \
        print('c') \
        print('u') \
        print('p') \
        print('y') \
        print(' ') \
        uart = space + '0'; \
        print(uart) \
        print(' ') \
        print('u') \
        print('n') \
        print('t') \
        print('i') \
        print('l') \
        print(' ') \
        log_3digits(to) \
        print('\n') \
        EA = 1;

void UART_output() {
    /*for (ccc = 0; ccc < 20; ccc++) {
        tmp = rand();
        uart = tmp / 100 + '0'; \
        print(uart) \
        uart = tmp / 10 % 10 + '0'; \
        print(uart) \
        uart = tmp % 10 + '0'; \
        print(uart) \
        print('\n')
    }*/
    log(1, space1, enter1, exit1)
    log(2, space2, enter2, exit2)
    log(3, space3, enter3, exit3)
    log(4, space4, enter4, exit4)
    log(5, space5, enter5, exit5)
}

//#define WW0 while (waiting_time_global > 0);
//#define WW0 while (waiting_time_global < TIMELEN);
#define WW0 while (waiting_time_global != 3);

void WW1() {
    WW0
    enter1 = now();
    delay(1);
    space1 = now();
    while(1);
}

void WW2() {
    WW0
    exit1 = now();
    delay(1);
    enter2 = now();
    while(1);
}

void WW3() {
    WW0
    exit3 = now();
    delay(1);
    enter4 = now();
    while(1);
}

void WW4() {
    WW0
    space4 = now();
    delay(1);
    exit4 = now();
    while(1);
}

void main( void ) {
    /*
    * @@@ [1 pt] initialize globals
    * @@@ [4 pt] set up Producer and Consumer.
    * Because both are infinite loops, there is no loop
    * in this function and no return.
    */
    /* @@@ [2 pt] initialize Tx for polling */
    TMOD |= 0x20; // to send
    TH1 = (char)-6;    // 4800 baud
    SCON = 0x50; // 8-bit 1 stop REN
    TR1 = 1;     // start timer 1

    //buf[0] = buf[1] = buf[2] = 0;
    //buf_head = 0;
    //buf_tail = 0;
    //SemaphoreCreate(mutex, 1);
    //SemaphoreCreate(full, 0);
    //SemaphoreCreate(empty, 1);
    //buf_ready = 1;

    //ThreadCreate(&Producer);
    //Consumer();

    //ThreadCreate(&Producer1);
    //ThreadCreate(&Producer2);
    //Consumer();

    // Init logs
    enter1 = 0;
    space1 = 0;
    exit1 = 0;
    enter2 = 0;
    space2 = 0;
    exit2 = 0;
    enter3 = 0;
    space3 = 0;
    exit3 = 0;
    enter4 = 0;
    space4 = 0;
    exit4 = 0;
    enter5 = 0;
    space5 = 0;
    exit5 = 0;

    SemaphoreCreate(sem_parking_lot, 2)
    SemaphoreCreate(sem_spots_sync, 1)
    spots[0] = 0;
    spots[1] = 0;

    /*ThreadCreate(&WW1);
    ThreadCreate(&WW2);
    ThreadCreate(&WW3);
    WW4();
    while(1);*/

    // Car1
    tmp = 0;
    ThreadCreate(&Park1);
    tmp = 1;
    ThreadCreate(&Park2);
    tmp = 2;
    ThreadCreate(&Park3);
    tmp = 3;
    ThreadCreate(&Park4);
    tmp = 4;
    ThreadCreate(&Park5);
    tmp = 5;
    // Wait till all thread is zero.
    while (thread_bitmap != 1);
    tmp = 6;
    UART_output();
    tmp = 7;
}

void myTimer0Handler( void ) {
    EA = 0;
    ThreadYield_Interrupt();
    EA = 1;
    __asm
    RETI
    __endasm ;
}

void _sdcc_gsinit_startup( void ) {
    __asm
    ljmp _Bootstrap
    __endasm ;
}
void _mcs51_genRAMCLEAR( void ) {}
void _mcs51_genXINIT( void ) {}
void _mcs51_genXRAMCLEAR( void ) {}
void timer0_ISR(void) __interrupt(1) {
    __asm
    ljmp _myTimer0Handler
    __endasm ;
}