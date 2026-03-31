#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int decide_pump_command(int level_pct,
                        bool pump_running,
                        bool manual_on,
                        bool manual_off,
                        bool leak,
                        bool overcurrent,
                        int *out_cmd)
{
    if (out_cmd == NULL) return -1;

    if (level_pct < 0)   level_pct = 0;
    if (level_pct > 100) level_pct = 100;

    if (leak || overcurrent)  { *out_cmd = 0; return 0; }
    else if (manual_off)      { *out_cmd = 0; return 0; }
    else if (manual_on)       { *out_cmd = 1; return 0; }
    else if (level_pct <= 25) { *out_cmd = 1; return 0; }
    else if (level_pct >= 80) { *out_cmd = 0; return 0; }
    else                      { *out_cmd = pump_running ? 1 : 0; return 0; }
}

int main(void)
{
    int cmd, r;

    r = decide_pump_command(15,  false, false, false, false, false, &cmd);
    printf("TC1:  ret=%d cmd=%d  (exp 0,1)\n", r, cmd);

    r = decide_pump_command(85,  true,  false, false, false, false, &cmd);
    printf("TC2:  ret=%d cmd=%d  (exp 0,0)\n", r, cmd);

    r = decide_pump_command(50,  true,  false, false, false, false, &cmd);
    printf("TC3:  ret=%d cmd=%d  (exp 0,1)\n", r, cmd);

    r = decide_pump_command(50,  false, false, false, false, false, &cmd);
    printf("TC4:  ret=%d cmd=%d  (exp 0,0)\n", r, cmd);

    r = decide_pump_command(40,  false, true,  false, false, false, &cmd);
    printf("TC5:  ret=%d cmd=%d  (exp 0,1)\n", r, cmd);

    r = decide_pump_command(10,  true,  false, true,  false, false, &cmd);
    printf("TC6:  ret=%d cmd=%d  (exp 0,0)\n", r, cmd);

    r = decide_pump_command(70,  true,  false, false, true,  false, &cmd);
    printf("TC7:  ret=%d cmd=%d  (exp 0,0)\n", r, cmd);

    r = decide_pump_command(120, false, false, false, false, false, &cmd);
    printf("TC8:  ret=%d cmd=%d  (exp 0,0)\n", r, cmd);

    r = decide_pump_command(-5,  false, false, false, false, false, &cmd);
    printf("TC9:  ret=%d cmd=%d  (exp 0,1)\n", r, cmd);

    r = decide_pump_command(50,  false, false, false, false, false, NULL);
    printf("TC10: ret=%d         (exp -1)\n",  r);

    return 0;
}