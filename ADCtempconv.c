#include <stddef.h>
#include <stdio.h>

int adc_to_percent(int adc, int adc_max, int *out_percent)
{
    if (out_percent == NULL) return -1;
    if (adc_max    <= 0)     return -1;
    if (adc        <  0)     return -1;
    if (adc        >  adc_max) return -1;

    long pct = ((long)adc * 100 + adc_max / 2) / adc_max;

    if (pct <   0) pct =   0;
    if (pct > 100) pct = 100;

    *out_percent = (int)pct;
    return 0;
}

int main(void)
{
    int pct;
    adc_to_percent(0,    409,  &pct); printf("0/409     = %d%%\n", pct);
    adc_to_percent(409,  409,  &pct); printf("409/409   = %d%%\n", pct);
    adc_to_percent(2048, 4095, &pct); printf("2048/4095 = %d%%\n", pct);
    adc_to_percent(1023, 1023, &pct); printf("1023/1023 = %d%%\n", pct);
    adc_to_percent(205,  409,  &pct); printf("205/409   = %d%%\n", pct);
    return 0;
}