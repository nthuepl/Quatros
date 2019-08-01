#include <8051.h>
#include "preemptive.h"
/*
* @@@ [2 pts] declare the static globals here using
* __idata __at (address) type name; syntax
* manually allocate the addresses of these variables, for
* - saved stack pointers (MAXTHREADS)
* - current thread ID
* - a bitmap for which thread ID is a valid thread;
* maybe also a count, but strictly speaking not necessary
* - plus any temporaries that you need.
*/
/*__idata __at (0x3B) char x;
__idata __at (0x3C) char y;
__idata __at (0x3D) char z;
__idata __at (0x3E) char w;*/
// TODO: add other temp vars.
/*
* @@@ [8 pts]
* define a macro for saving the context of the current thread by
* 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
* 2) save SP into the saved Stack Pointers array
* as indexed by the current thread ID.
* Note that 1) should be written in assembly,
* while 2) can be written in either assembly or C
*/
#define SAVESTATE \
{ __asm \
/*@@@ your code here*/\
push ACC \
push B \
push DPL \
push DPH \
push PSW \
__endasm; \
}; \
{ __asm \
/* Note that we cannot trash the registers here. */ \
/* Save r0 using B (B has been pushed so it's ok to trash B).*/ \
mov B, r0 \
/* savedSP[current_thread] = SP; */ \
mov	a, #_savedSP \
add	a, _current_thread \
mov r0, a \
mov	@r0, _SP \
/* Restore r0.*/ \
mov r0, B \
__endasm; \
};
/*
* @@@ [8 pts]
* define a macro for restoring the context of the current thread
by
* essentially doing the reverse of SAVESTATE:
* 1) assign SP to the saved SP from the saved stack pointer array
* 2) pop the registers PSW, data pointer registers, B reg, and
ACC
* Again, popping must be done in assembly but restoring SP can be
* done in either C or assembly.
*/
#define RESTORESTATE \
{ __asm \
/* Save r0.*/ \
mov B, r0 \
/* SP = savedSP[current_thread]; */ \
mov	a, #_savedSP \
add	a, _current_thread \
mov	r0, a \
mov	_SP, @r0 \
/* Restore r0.*/ \
mov r0, B \
__endasm; \
}; \
{ __asm \
/*@@@ your code here*/\
pop PSW \
pop DPH \
pop DPL \
pop B \
pop ACC \
__endasm; \
};
/*
* we declare main() as an extern so we can reference its symbol
* when creating a thread for it.
*/
extern void main( void );
void add_time(void);
void push_exit_addr(FunctionPtr fp);
ThreadID ThreadCreateBody(FunctionPtr fp);
/*
* Bootstrap is jumped to by the startup code to make the thread
for
* main, and restore its context so the thread can run.
*/
void Bootstrap( void ) {
    /*
    * @@@ [2 pts]
    * initialize data structures for threads (e.g., mask)
    *
    * optional: move the stack pointer to some known location
    * only during bootstrapping. by default, SP is 0x07.
    *
    * @@@ [2 pts]
    * create a thread for main; be sure current thread is
    * set to this thread ID, and restore its context,
    * so that it starts running main().
    */

    // Init thread package.
    thread_bitmap = 0;
    current_thread = 0;
    for (i = 0; i < MAXTHREADS; i++)
        savedSP[i] = 0;
    SP = 0x07;

    // Init thread and semaphore package.
    //for (i = 0; i < MAXTHREADS; i++)
    //    waiting_threads[i] = 0;
    for (i = 0; i < MAXTHREADS; i++) {
        waiting_time[i] = 0;
        count_down[i] = 0;
    }
    waiting_time_global = 0;
    time = 0;
    seed = 0;

    // Semaphore limiting threads.
    SemaphoreCreate(sem_thread, MAXTHREADS)

    // Enable interrupt timer.
    TMOD = 0;
    IE = 0x82;
    TR0 = 1;

    current_thread = ThreadCreate(&main);

    // Restore context.
    RESTORESTATE;
    // Return to main function.
}
/*
* ThreadCreate() creates a thread data structure so it is ready
* to be restored (context switched in).
* The function pointer itself should take no argument and should
* return no argument.
*/
void push_exit_addr(FunctionPtr fp) {
    __asm
    POP _tmp
    POP _uart
    PUSH DPL
    PUSH DPH
    PUSH _uart
    PUSH _tmp
    __endasm ;
}
ThreadID ThreadCreate(FunctionPtr fp) {
    SemaphoreWait(sem_thread)
    return ThreadCreateBody(fp);
}
ThreadID ThreadCreateBody(FunctionPtr fp) __critical {
    // Store backed up psw.
    __asm
    POP _k
    __endasm ;
    /*
    * @@@ [2 pts]
    * check to see we have not reached the max #threads.
    * if so, return -1, which is not a valid thread ID.
    */
    // if thread_bitmap is 1111B
    if (thread_bitmap == (1 << MAXTHREADS) - 1) {
      //Won't execute.
      __asm
      PUSH _k
      __endasm ;
      return -1;
    }
    /*
    * @@@ [5 pts]
    * otherwise, find a thread ID that is not in use,
    * and grab it. (can check the bit mask for threads),
    *
    * @@@ [18 pts] below
    * a. update the bit mask
    (and increment thread count, if you use a thread
    count,
    but it is optional)
    b. calculate the starting stack location for new thread
    c. save the current SP in a temporary
    set SP to the starting location for the new thread
    d. push the return address fp (2-byte parameter to
    ThreadCreate) onto stack so it can be the return
    address to resume the thread. Note that in SDCC
    convention, 2-byte ptr is passed in DPTR. but
    push instruction can only push it as two separate
    registers, DPL and DPH.
    e. we want to initialize the registers to 0, so we
    assign a register to 0 and push it four times
    for ACC, B, DPL, DPH. Note: push #0 will not work
    because push takes only direct address as its
    operand,
    but it does not take an immediate (literal) operand.
    f. finally, we need to push PSW (processor status word)
    register, which consist of bits
    CY AC F0 RS1 RS0 OV UD P
    all bits can be initialized to zero, except
    <RS1:RS0>
    which selects the register bank.
    Thread 0 uses bank 0, Thread 1 uses bank 1, etc.
    Setting the bits to 00B, 01B, 10B, 11B will select
    the register bank so no need to push/pop registers
    R0-R7. So, set PSW to
    00000000B for thread 0, 00001000B for thread 1,
    00010000B for thread 2, 00011000B for thread 3.
    g. write the current stack pointer to the saved stack
    pointer array for this newly created thread ID
    h. set SP to the saved SP in step c.
    i. finally, return the newly created thread ID.
    */
    // Find a non-used thread.
    for (i = 0; i < MAXTHREADS; i++) {
        if (!((thread_bitmap >> i) & 1)) {
            // If thread[i] is not used.
            // i will be the current thread.
            break;
        }
    }

    // Update thread bitmap.
    thread_bitmap += (1 << i);

    // Save Stack Pointer in variable j.
    j = SP;
    // SP is 3F, 4F, 5F, ...
    SP = ((4 + i) << 4) - 1;

    {__asm \
    push DPL \
    push DPH \
    __endasm ; \
    };
    push_exit_addr(&ThreadExit);
    {__asm \
    pop _tmp \
    pop _uart \
    pop DPH \
    pop DPL \
    push _uart \
    push _tmp \
    __endasm ; \
    };
    // Push fp to DPL, DPH to return to that function when RESTORESTATE.
    {__asm \
    push DPL \
    push DPH \
    __endasm ; \
    };

    // Push 0 for ACC, B, DPL, DPH.
    // Trash DPL, since the real DPL is already pushed.
    {__asm \
    mov DPL, #0 \
    push DPL \
    push DPL \
    push DPL \
    push DPL \
    __endasm ; \
    };

    // Calculate the <RS1:RS0> bits for register bank.
    // 00000000B or 00001000B
    DPL = (i << 3);
    {__asm \
    push DPL \
    __endasm ; \
    };

    // Store Stack Pointer for newly created thread.
    savedSP[i] = SP;

    // Restore Stack Pointer.
    SP = j;

    // Restore backed up psw.
    __asm
    PUSH _k
    __endasm ;
    // Return new thread id.
    waiting_threads[i] = 0;
    waiting_time[i] = 0;
    /*{__asm \
    pop _i \
    pop _j \
    pop _uart \
    pop _tmp \
    push _j \
    push _i \
    push _tmp \
    push _uart \
    __endasm ; \
    };*/
    return i;
}
/*
* this is called by a running thread to yield control to another
* thread. ThreadYield() saves the context of the current
* running thread, picks another thread (and set the current
thread
* ID to it), if any, and then restores its state.
*/
void ThreadYield( void ) {
    // Save psw.
    /*__asm
    POP _k
    __endasm ;*/
    //__critical {
    EA = 0;
    SAVESTATE;
    // TODO: Better code.
    // Save r0 in DPL (They can be trashed).
    {__asm \
    mov DPL, r0 \
    mov DPH, r1 \
    __endasm ; \
    };
    i = 0;
    do {
        /*
        * @@@ [8 pts] do round-robin policy for now.
        * find the next thread that can run and
        * set the current thread ID to it,
        * so that it can be restored (by the last line of
        * this function).
        * there should be at least one thread, so this
        loop
        * will always terminate.
        */
        // The code below uses
        // - r0
        // - r1
        // - b
        // - a
        // - c
        // Find next available thread.
        current_thread++;
        if (current_thread == MAXTHREADS)
            current_thread = 0;
        if (((thread_bitmap >> current_thread) & 1) &&
            (waiting_threads[current_thread] == 0))
            break;
        i++;
        if (i >= 4) {
            add_time();
            i = 0;
        }
    } while (1);
    waiting_time[current_thread] = 0;
    // Restore r0.
    {__asm \
    mov r0, DPL \
    mov r1, DPH \
    __endasm ; \
    };
    RESTORESTATE;
    EA = 1;
    // Don't restore backed up psw.
    /*__asm
    mov _k, PSW
    PUSH _k
    __endasm ;*/
}
void add_time(void) {
    for (i = 0; i < MAXTHREADS; i++) {
        if ((thread_bitmap >> i) & 1) {
            //waiting_time[i]++;
            {__asm
            mov a, #_waiting_time
            add a, _i
            mov r0, a
            inc @r0
            __endasm ;
            };
            #define MACRO0 #_waiting_threads
            #define MACRO1 #_count_down
            #define MACRO2(c) \
            {__asm \
            /*if (waiting_threads[i] == NULLPTR) {*/ \
            mov a, MACRO0 \
            add a, _i \
            mov r1, a \
            cjne @r1, SHARP_NULL, CONCAT(c, DOLLAR) \
            /*if (waiting_time[i] > TIMELEN) {*/ \
            mov a, @r0 \
            add a, SHARP_MAX - SHARP_TIMELEN \
            jnc CONCAT(c, DOLLAR) \
            /*if (count_down[i] > 1) {*/ \
            /*mov a, MACRO1*/ \
            /*add a, _i*/ \
            /*mov r0, a*/ \
            /*mov a, @r0*/ \
            /*add a, SHARP_MAX - SHARP_ONE*/ \
            /*jnc CONCAT(c, DOLLAR)*/ \
            __endasm ; \
            }; \
            if (count_down[i] > 0) { \
                /*count_down[i]--;*/ \
                {__asm \
                mov a, MACRO1 \
                add a, _i \
                mov r0, a \
                dec @r0 \
                __endasm ; \
                }; \
            } else { \
                waiting_threads[i] = 0; \
            } \
            waiting_time[i] = 0; \
            {__asm \
            CONCAT(c, DOLLAR): \
            __endasm ; \
            };
            MACRO2(__COUNTER__)
        }
    }
    {__asm
    inc _waiting_time_global
    __endasm ;
    };
    if (waiting_time_global > TIMELEN) {
        {__asm
        inc _time
        mov _waiting_time_global, #0
        __endasm ;
        }
    }
}
void ThreadYield_Interrupt( void ) {
    // Save return place.
    __asm
    POP _k
    POP _j
    __endasm ;
    // Save PC
    /*__asm
    POP _z
    POP _w
    __endasm ;*/
    SAVESTATE;
    // TODO: Better code.
    // Save r0 in DPL (They can be trashed).
    {__asm \
    mov DPL, r0 \
    mov DPH, r1 \
    __endasm ; \
    };
    i = 0;
    do {
        /*
        * @@@ [8 pts] do round-robin policy for now.
        * find the next thread that can run and
        * set the current thread ID to it,
        * so that it can be restored (by the last line of
        * this function).
        * there should be at least one thread, so this
        loop
        * will always terminate.
        */
        // The code below uses
        // - r0
        // - r1
        // - b
        // - a
        // - c
        // Find next available thread.
        current_thread++;
        if (current_thread == MAXTHREADS)
            current_thread = 0;
        if (((thread_bitmap >> current_thread) & 1) &&
            (waiting_threads[current_thread] == 0))
            break;
        i++;
        if (i >= 4) {
            add_time();
            i = 0;
        }
    } while (1);
    add_time();
    waiting_time[current_thread] = 0;
    // Restore r0.
    {__asm \
    mov r0, DPL \
    mov r1, DPH \
    __endasm ; \
    };
    RESTORESTATE;
    __asm
    PUSH _j
    PUSH _k
    __endasm ;
    // Restore backed up psw.
    /*__asm
    PUSH _k
    __endasm ;*/
}
/*
* ThreadExit() is called by the thread's own code to terminate
* itself. It will never return; instead, it switches context
* to another thread.
*/
void ThreadExit(void) {
    /*
    * clear the bit for the current thread from the
    * bit mask, decrement thread count (if any),
    * and set current thread to another valid ID.
    * Q: What happens if there are no more valid threads?
    */
    // A: There are always the main thread in this case.
    EA = 0;
    /*__asm
    mov a, _current_thread
    __endasm ;*/
    j = 1;
    for (i = 0; i < current_thread; i++) {
        j = j + j;
    }
    //j = (1 << current_thread);
    thread_bitmap -= j;
    if (thread_bitmap == 0) {
        while(1);
    }
    // Find next ready thread.
    SemaphoreSignal(sem_thread)
    //EA = 1;
    ThreadYield();
}

void delay(unsigned char n) {
    __critical {
        __asm
        /*waiting_threads[current_thread] = NULLPTR;*/
        mov a, #_waiting_threads
        add a, _current_thread
        mov r0, a
        mov @r0, SHARP_NULL
        __endasm ;
        waiting_time[current_thread] = 8;
        count_down[current_thread] = n-1;
        count_down[current_thread]++;
    }
    ThreadYield();
}

unsigned char rand() {
    /*seed += 21839; //65521;
    seed = seed % 256;*/
    //return 3;
    seed += 91;
    while (seed >= 17)
        seed -= 17;
    //seed = seed % 17;
    return (unsigned char)seed;
}

unsigned char now(void) {
    /*if (waiting_time_global < 4)
        return time - 1;*/
    return time;
}