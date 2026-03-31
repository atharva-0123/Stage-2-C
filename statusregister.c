#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum Mode { MODE0=0, MODE1=1, MODE2=2, MODE3=3 };

/* documentation-only struct — not used for packing */
struct StatusBits {
    unsigned int rsv     : 7;   /* bits  6:0  */
    unsigned int fault   : 1;   /* bit   7    */
    unsigned int busy    : 1;   /* bit   8    */
    unsigned int ready   : 1;   /* bit   9    */
    unsigned int mode    : 2;   /* bits 11:10 */
    unsigned int retries : 3;   /* bits 14:12 */
    unsigned int par     : 1;   /* bit  15    */
};

int build_status_reg16(enum Mode mode, int retries,
                       bool ready, bool busy, bool fault,
                       int *out_reg16)
{
    if (out_reg16 == NULL)              return -1;
    if (retries < 0 || retries > 7)    return -1;
    if (mode < MODE0 || mode > MODE3)  return -1;

    int reg = 0;
    reg |= (retries   & 0x7) << 12;
    reg |= (mode      & 0x3) << 10;
    reg |= (ready  ? 1 : 0)  <<  9;
    reg |= (busy   ? 1 : 0)  <<  8;
    reg |= (fault  ? 1 : 0)  <<  7;
    /* bits 6:0 reserved = 0 */

    /* even parity over bits 0..14 */
    int tmp = reg & 0x7FFF;
    int par = 0;
    while (tmp) { par ^= (tmp & 1); tmp >>= 1; }
    reg |= (par & 1) << 15;

    *out_reg16 = reg & 0xFFFF;
    return 0;
}

static void print_reg(int reg)
{
    printf("     reg16=0x%04X  PAR=%d RETRIES=%d MODE=%d READY=%d BUSY=%d FAULT=%d\n",
           reg & 0xFFFF,
           (reg >> 15) & 1,
           (reg >> 12) & 0x7,
           (reg >> 10) & 0x3,
           (reg >>  9) & 1,
           (reg >>  8) & 1,
           (reg >>  7) & 1);
}

int main(void)
{
    int reg, r;

    r = build_status_reg16(MODE0, 0, false, false, false, &reg);
    printf("TC1: ret=%d\n", r); print_reg(reg);
    printf("     (exp 0, PAR=0, all fields 0)\n\n");

    r = build_status_reg16(MODE3, 7, true, true, true, &reg);
    printf("TC2: ret=%d\n", r); print_reg(reg);
    printf("     (exp 0, RETRIES=7,MODE=3,READY=1,BUSY=1,FAULT=1)\n\n");

    r = build_status_reg16(MODE1, 5, true, false, false, &reg);
    printf("TC3: ret=%d\n", r); print_reg(reg);
    printf("     (exp 0, RETRIES=5,MODE=1,READY=1,BUSY=0,FAULT=0)\n\n");

    r = build_status_reg16(MODE0, -1, false, false, false, &reg);
    printf("TC4: ret=%d (exp -1)\n\n", r);

    r = build_status_reg16((enum Mode)4, 3, false, false, false, &reg);
    printf("TC5: ret=%d (exp -1)\n\n", r);

    r = build_status_reg16(MODE0, 0, false, false, false, NULL);
    printf("TC6: ret=%d (exp -1)\n\n", r);

    return 0;
}