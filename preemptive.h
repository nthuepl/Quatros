/*
 * file: preemptive.h
 *
 * this is the include file for the preemptive multithreading
 * package.  It is to be compiled by SDCC and targets the EdSim51 as
 * the target architecture.
 *
 * CS 3423 Fall 2018
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4  /* not including the scheduler */
#define MAXSPOTS 2
#define NULLPTR 0x01
// Means wait 9
#define TIMELEN 8//8
/* the scheduler does not take up a thread of its own */

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadYield_Interrupt(void);
void ThreadExit(void);

void delay(unsigned char n);
unsigned char rand();
unsigned char now(void);

// preemptive.c
__idata __at (0x30) char savedSP[MAXTHREADS];
__idata __at (0x34) char thread_bitmap;
__idata __at (0x35) char current_thread;
__idata __at (0x36) char i;
__idata __at (0x37) char j;
__idata __at (0x38) char k;
// Limit to 4 threads.
__idata __at (0x39) signed char sem_thread;
__idata __at (0x3A) char* waiting_threads[MAXTHREADS];
__idata __at (0x3E) char* addr;
__idata __at (0x3F) char uart;

// test3threads.c
//__idata __at (0x39) char buf1_prev;
//__idata __at (0x3A) char buf2_prev;
//__idata __at (0x3B) char buf[3];
//__idata __at (0x3E) char buf_head;
//__idata __at (0x3F) char buf_tail;
__idata __at (0x20) signed char sem_parking_lot;
__idata __at (0x21) signed char sem_spots_sync;
__idata __at (0x22) char spots[MAXSPOTS];
// Queued threads (for semaphore signal wakeup).
__idata __at (0x24) unsigned char waiting_time[MAXTHREADS];
__idata __at (0x28) unsigned char count_down[MAXTHREADS];
__idata __at (0x2C) unsigned char waiting_time_global;
__idata __at (0x2D) unsigned char time;
__idata __at (0x2E) unsigned short seed;
//__idata __at (0x20) signed char mutex;
//__idata __at (0x21) signed char full;
//__idata __at (0x22) signed char empty;
__idata __at (0x02) char enter1;
__idata __at (0x03) char space1;
__idata __at (0x04) char exit1;
__idata __at (0x05) char enter2;
__idata __at (0x0A) char space2;
__idata __at (0x0B) char exit2;
__idata __at (0x0C) char enter3;
__idata __at (0x0D) char space3;
__idata __at (0x12) char exit3;
__idata __at (0x13) char enter4;
__idata __at (0x14) char space4;
__idata __at (0x15) char exit4;
__idata __at (0x1A) char enter5;
__idata __at (0x1B) char space5;
__idata __at (0x1C) char exit5;
__idata __at (0x1D) char tmp;


#define CNAME(s) _ ## s
#define CONCAT_NATIVE(a, b) a ## b
#define CONCAT(a, b) CONCAT_NATIVE(a, b)
#define DOLLAR $
#define SHARP_Z #0
#define SHARP_ONE #1
#define SHARP_MAX #0xFF
#define SHARP_NULL #NULLPTR
#define SHARP_TIMELEN #TIMELEN

//TODO: Critical?
// create a counting semaphore s initialized to n
#define SemaphoreCreate(s, n) \
{ __asm \
mov	CNAME(s), CONCAT(SHARP_Z, n) \
__endasm; \
}; \
EA = 0; \
/* Must assign to variable or it won't work... */ \
addr = &s; \
for (i = 0; i < MAXTHREADS; i++) { \
    if (waiting_threads[i] == addr) { \
        waiting_threads[i] = 0; \
    } \
}; \
EA = 1;

// signal a semaphore s
#define MACRO_WT #_waiting_time
#define SemaphoreSignal(s) \
{ __asm \
inc CNAME(s) \
__endasm; \
}; \
EA = 0; \
if (s <= 0) { \
    /* Must assign to variable or it won't work... */ \
    addr = &s; \
    j = MAXTHREADS; \
    for (i = 0; i < MAXTHREADS; i++) { \
        if (((thread_bitmap >> i) & 1) && \
            (waiting_threads[i] == addr)) { \
            if (j == MAXTHREADS) \
                j = i; \
            __asm \
            mov a, MACRO_WT \
            add a, _j \
            mov r0, a \
            mov a, @r0 \
            mov r0, a \
            mov a, MACRO_WT \
            add a, _i \
            mov r1, a \
            mov a, @r1 \
            mov r1, a \
            mov a, r0 \
            clr c \
            subb a, r1 \
            mov _k, c \
            __endasm ; \
            if (!k) \
            /*if (waiting_time[j] < waiting_time[i])*/ \
                j = i; \
        } \
    } \
    if (j < MAXTHREADS) \
        waiting_threads[j] = 0; \
} \
EA = 1;

// do busy-wait on semaphore s
#define SemaphoreWait(s) \
SemaphoreWaitBody(s, __COUNTER__)
#define SemaphoreWaitBody(s, c) \
{ __asm \
dec CNAME(s) \
__endasm; \
}; \
EA = 0; \
if (s < 0) { \
    waiting_threads[current_thread] = &s; \
    EA = 1; \
    ThreadYield(); \
} \
EA = 1;

#endif // __PREEMPTIVE_H__
