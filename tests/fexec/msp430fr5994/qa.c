
#include <stdio.h>
#include <string.h>

#include "glue.h"
#include "qa.h"

#define STR_LEN 80

// execute internal function
#define SCENARIO1

// execute detected opcodes without mmap, also not obeying NX
#define SCENARIO5

#define OPCODE_LEN 8
uint8_t f_opcode[OPCODE_LEN] = {0x03, 0x43, 0x7c, 0x40, 0xee, 0xff, 0x30, 0x41};

uint8_t f_int(void)
{
    __asm("nop");
    return 0xee;
}

void display_menu(void)
{
    uart0_print("\r\n fexec test suite --- available commands:\r\n\r\n");
    uart0_print(" \e[33;1m?\e[0m             - show menu\r\n");
    uart0_print(" \e[33;1mr\e[0m             - run fct\r\n");
}

// same as above, but asume writable memory can be executed and no mmap is needed
uint8_t exec_opcode_no_mmap(const uint8_t *code, size_t codelen)
{
    uint8_t ret;

    // now we can call it
    ret = ((uint8_t (*)(void)) code)();
    return ret;
}

void parse_user_input(void)
{
    char *input = uart0_get_rx_buf();
    char f = input[0];
    char sconv[CONV_BASE_8_BUF_SZ];
    char buf[STR_LEN+1];
    uint32_t in;
    //uint8_t *ptr;
    //uint32_t addr;
    uint8_t i;
    uint8_t *ap;
    uint8_t current_opcode[OPCODE_LEN];
    uint8_t ret;


    if (f == '?') {
        display_menu();
    } else if (f == 'r') {
        ap = (uint8_t *)&f_int;

        memcpy(current_opcode, ap, OPCODE_LEN);
        if (memcmp(current_opcode, f_opcode, OPCODE_LEN) != 0) {
            uart0_print(" [!!] opcodes should be updated\r\n");
            snprintf(buf, STR_LEN, "0x%p ", &f_int);
            uart0_print(buf);
            for (i=0;i<8;i++) {
                snprintf(buf, STR_LEN, "%02x%02x%02x%02x ", (uint8_t)(*(ap+4*i)), (uint8_t)(*(ap+4*i+1)), (uint8_t)*((ap+4*i+2)), (uint8_t)(*(ap+4*i+3)));
                uart0_print(buf);
            }
            snprintf(buf, STR_LEN, "\r\nuint8_t f_opcode[%d] = {", OPCODE_LEN);
            uart0_print(buf);
            for (i=0; i<OPCODE_LEN; i++) {
                snprintf(buf, STR_LEN, "0x%02x%s", (uint8_t)(*(ap+i)), i < (OPCODE_LEN-1) ? ", ":"");
                uart0_print(buf);
            }
            uart0_print("};\r\n");
        }

#ifdef SCENARIO1
        snprintf(buf, STR_LEN, "* execute internal function from %p", f_int);
        uart0_print(buf);
        ret = f_int();
        if (ret == 0xee) {
            uart0_print(" \t[ok]\r\n");
        } else {
            snprintf(buf, STR_LEN, ", ret 0x%02x\r\n", ret);
            uart0_print(buf);
        }
#endif

#ifdef SCENARIO5
        snprintf(buf, STR_LEN, "* execute detected opcodes without mmap, also not obeying NX %p", &current_opcode);
        uart0_print(buf);
        ret = exec_opcode_no_mmap(current_opcode, sizeof(f_opcode));
        if (ret == 0xee) {
            uart0_print(" \t[ok]\r\n");
        } else {
            snprintf(buf, STR_LEN, ", ret 0x%02x\r\n", ret);
            uart0_print(buf);
        }
#endif

    } else if (f == 'h') {
        if (str_to_uint32(input, &in, 1, strlen(input) - 1, 0, -1) == EXIT_FAILURE) {
            uart0_print("error during str_to_uint32()");
        }
        uart0_print("received ");
        uart0_print(_utoh(sconv, in));
        uart0_print("\r\n");
    } else {
        uart0_tx_str("\r\n", 2);
    }
}

