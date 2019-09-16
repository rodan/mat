/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "proj.h"

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <string.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "uart1.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */
uint8_t str[2] = "hi";


// execute internal function
//#define SCENARIO1

// mmap() is not implemented, so we skip a few scenarios that depend on it
// execute stored opcodes via mmap while obeying NX
//#define SCENARIO2

// execute detected opcodes via mmap while obeying NX
//#define SCENARIO3

// execute detected opcodes via mmap not obeying NX
//#define SCENARIO4

// execute detected opcodes without mmap, also not obeying NX
#define SCENARIO5

#define OPCODE_LEN 6
uint8_t f_opcode[OPCODE_LEN] = {0x00, 0x00, 0x30, 0xee, 0x00, 0x08};

uint8_t f_int(void)
{
    __asm("nop");
    return 0xee;
}

#ifdef SCENARIO2
// function that executes an array of opcodes
// based on https://stackoverflow.com/questions/37122186/c-put-x86-instructions-into-array-and-execute-them
uint8_t exec_opcode(const uint8_t *code, size_t codelen)
{
    uint8_t ret;
    // in order to manipulate memory protection, we must work with
    // whole pages allocated directly from the operating system.
    static size_t pagesize;
    if (!pagesize) {
        pagesize = sysconf(_SC_PAGESIZE);
        if (pagesize == (size_t)-1) {
            errExit("getpagesize");
        }
    }

    // allocate at least enough space for the code + 1 byte
    // (so that there will be at least one INT3 - see below),
    // rounded up to a multiple of the system page size.
    size_t rounded_codesize = ((codelen + 1 + pagesize - 1)
                               / pagesize) * pagesize;

    void *executable_area = mmap(0, rounded_codesize,
                                 PROT_READ|PROT_WRITE,
                                 MAP_PRIVATE|MAP_ANONYMOUS,
                                 -1, 0);
    if (!executable_area) {
        errExit("mmap");
    }

    // at this point, executable_area points to memory that is writable but
    // *not* executable.  load the code into it.
    memcpy(executable_area, code, codelen);

    // fill the space at the end with INT3 instructions, to guarantee
    // a prompt crash if the generated code runs off the end.
    // must change this if generating code for non-x86.
    memset(executable_area + codelen, 0xCC, rounded_codesize - codelen);

    // make executable_area actually executable (and unwritable)
    if (mprotect(executable_area, rounded_codesize, PROT_READ|PROT_EXEC)) {
        errExit("mprotect");
    }

    // now we can call it
    ret = ((uint8_t (*)(void)) executable_area)();

    munmap(executable_area, rounded_codesize);
    return ret;
}
#endif

#ifdef SCENARIO3
// same as above, but asume writable memory can be executed
uint8_t exec_opcode_WX(const uint8_t *code, size_t codelen)
{
    uint8_t ret;
    // in order to manipulate memory protection, we must work with
    // whole pages allocated directly from the operating system.
    static size_t pagesize;
    if (!pagesize) {
        pagesize = sysconf(_SC_PAGESIZE);
        if (pagesize == (size_t)-1) {
            errExit("getpagesize");
        }
    }

    // allocate at least enough space for the code + 1 byte
    // (so that there will be at least one INT3 - see below),
    // rounded up to a multiple of the system page size.
    size_t rounded_codesize = ((codelen + 1 + pagesize - 1)
                               / pagesize) * pagesize;

    void *executable_area = mmap(0, rounded_codesize,
                                 PROT_READ|PROT_WRITE|PROT_EXEC,
                                 MAP_PRIVATE|MAP_ANONYMOUS,
                                 -1, 0);
    if (!executable_area) {
        errExit("mmap");
    }

    // at this point, executable_area points to memory that is writable and
    // also executable.  load the code into it.
    memcpy(executable_area, code, codelen);

    // fill the space at the end with INT3 instructions, to guarantee
    // a prompt crash if the generated code runs off the end.
    // must change this if generating code for non-x86.
    memset(executable_area + codelen, 0xCC, rounded_codesize - codelen);

    // now we can call it
    ret = ((uint8_t (*)(void)) executable_area)();

    munmap(executable_area, rounded_codesize);
    return ret;
}
#endif

#ifdef SCENARIO5
// same as above, but asume writable memory can be executed and no mmap is needed
uint8_t exec_opcode_no_mmap(const uint8_t *code, size_t codelen)
{
    uint8_t ret;

    // now we can call it
    ret = ((uint8_t (*)(void)) code)();
    return ret;
}
#endif

#define STR_LEN 20
 
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    uint8_t i=0;
    uint8_t *ap;
    //uint8_t current_opcode[OPCODE_LEN];
    uint8_t ret;
    char buf[STR_LEN+1];
    
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    /* Initialize I/O and Peripherals for application */
    InitApp();

#if 0
    ap = (uint8_t *)&f_int;

    memcpy(current_opcode, ap, OPCODE_LEN);
    if (memcmp(current_opcode, f_opcode, OPCODE_LEN) != 0) {
        uart1_print(" [!!] opcodes should be updated\r\n");
        snprintf(buf, STR_LEN, "0x%p ", &f_int);
        uart1_print(buf);
        for (i = 0; i < 8; i++) {
            snprintf(buf, STR_LEN, "%02x%02x%02x%02x ", (uint8_t) (*(ap + 4 * i)), (uint8_t) (*(ap + 4 * i + 1)), (uint8_t)*((ap + 4 * i + 2)), (uint8_t) (*(ap + 4 * i + 3)));
            uart1_print(buf);
        }
        snprintf(buf, STR_LEN, "\r\nuint8_t f_opcode[%d] = {", OPCODE_LEN);
        uart1_print(buf);
        for (i = 0; i < OPCODE_LEN; i++) {
            snprintf(buf, STR_LEN, "0x%02x%s", (uint8_t) (*(ap + i)), i < (OPCODE_LEN - 1) ? ", " : "");
            uart1_print(buf);
        }
        uart1_print("};\r\n");
    }
#endif
        
#ifdef SCENARIO1
        uart1_print("* execute internal function from ");
        snprintf(buf, STR_LEN, "%p", f_int);
        uart1_print(buf);
        ret = f_int();
        if (ret == 0xee) {
            uart1_print(" \t[ok]\r\n");
        } else {
            snprintf(buf, STR_LEN, ", ret 0x%02x\r\n", ret);
            uart1_print(buf);
        }
#endif

#ifdef SCENARIO5
        uart1_print("* execute stored opcodes without mmap, also not obeying NX ");
        snprintf(buf, STR_LEN, "%p", &f_opcode);
        uart1_print(buf);
        ret = exec_opcode_no_mmap(f_opcode, sizeof(f_opcode));
        if (ret == 0xee) {
            uart1_print(" \t[ok]\r\n");
        } else {
            snprintf(buf, STR_LEN, ", ret 0x%02x\r\n", ret);
            uart1_print(buf);
        }
#endif 
    uart1_print("hello");
    while(1) {
    }
}
