
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include "error_functions.h"

// execute internal function
#define SCENARIO1

// execute stored opcodes via mmap while obeying NX
#define SCENARIO2

// execute detected opcodes via mmap while obeying NX
#define SCENARIO3

// execute detected opcodes via mmap not obeying NX
#define SCENARIO4

// execute detected opcodes without mmap, also not obeying NX
#define SCENARIO5


#define OPCODE_LEN 52
uint8_t f_opcode[OPCODE_LEN] = {0x48, 0x83, 0xec, 0x18, 0x64, 0x48, 0x8b, 0x04, 0x25, 0x28, 0x00, 0x00, 0x00, 0x48, 0x89, 0x44, 0x24, 0x08, 0x31, 0xc0, 0x90, 0x48, 0x8b, 0x54, 0x24, 0x08, 0x64, 0x48, 0x33, 0x14, 0x25, 0x28, 0x00, 0x00, 0x00, 0x75, 0x0a, 0xb8, 0xee, 0xff, 0xff, 0xff, 0x48, 0x83, 0xc4, 0x18, 0xc3, 0xe8, 0xa0, 0xf6, 0xff, 0xff};

uint8_t f_int(void)
{
    __asm("nop");
    return 0xee;
}

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

    // at this point, executable_area points to memory that is writable but
    // *not* executable.  load the code into it.
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

// same as above, but asume writable memory can be executed and no mmap is needed
uint8_t exec_opcode_no_mmap(const uint8_t *code, size_t codelen)
{
    uint8_t ret;

    // now we can call it
    ret = ((uint8_t (*)(void)) code)();
    return ret;
}

int main()
{
    uint8_t i;
    uint8_t *ap;
    uint8_t current_opcode[OPCODE_LEN];
    uint8_t ret;

    ap = (uint8_t *)&f_int;

    memcpy(current_opcode, ap, OPCODE_LEN);
    if (memcmp(current_opcode, f_opcode, OPCODE_LEN) != 0) {
        printf(" [!!] opcodes should be updated\r\n");
        printf("0x%p ", f_int);
        for (i=0;i<8;i++) {
            printf("%02x%02x%02x%02x ", (uint8_t)(*(ap+4*i)), (uint8_t)(*(ap+4*i+1)), (uint8_t)*((ap+4*i+2)), (uint8_t)(*(ap+4*i+3)));
        }
        printf("\r\nuint8_t f_opcode[%d] = {", OPCODE_LEN);
        for (i=0; i<OPCODE_LEN; i++) {
            printf("0x%02x%s", (uint8_t)(*(ap+i)), i < (OPCODE_LEN-1) ? ", ":"");
        }

        printf("};\r\n");
    }

#ifdef SCENARIO1
    printf("* execute internal function from %p", &f_int);
    ret = f_int();
    if (ret == 0xee) {
        printf(" \t[ok]\r\n");
    } else {
        printf(", ret 0x%02x\r\n", ret);
    }
#endif

#ifdef SCENARIO2
    printf("* execute stored opcodes via mmap obeying NX %p", &f_opcode);
    ret = exec_opcode(f_opcode, sizeof(f_opcode));
    if (ret == 0xee) {
        printf(" \t[ok]\r\n");
    } else {
        printf(", ret 0x%02x\r\n", ret);
    }
#endif

#ifdef SCENARIO3
    printf("* execute detected opcodes via mmap obeying NX %p", &current_opcode);
    ret = exec_opcode(current_opcode, sizeof(f_opcode));
    if (ret == 0xee) {
        printf(" \t[ok]\r\n");
    } else {
        printf(", ret 0x%02x\r\n", ret);
    }
#endif

#ifdef SCENARIO4
    printf("* execute detected opcodes via mmap not obeying NX %p", &current_opcode);
    ret = exec_opcode_WX(current_opcode, sizeof(f_opcode));
    if (ret == 0xee) {
        printf(" \t[ok]\r\n");
    } else {
        printf(", ret 0x%02x\r\n", ret);
    }
#endif

#ifdef SCENARIO5
    printf("* execute detected opcodes without mmap, also not obeying NX %p", &current_opcode);
    ret = exec_opcode_no_mmap(current_opcode, sizeof(f_opcode));
    if (ret == 0xee) {
        printf(" \t[ok]\r\n");
    } else {
        printf(", ret 0x%02x\r\n", ret);
    }
#endif

}
