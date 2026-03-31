#include <stdio.h>

int compute_kmpl(int distance_meters, int fuel_milliliters, double *out_kmpl)
{
    if (out_kmpl == NULL)        return -1;
    if (distance_meters < 0)     return -1;
    if (fuel_milliliters <= 0)   return -1;

    *out_kmpl = (distance_meters / 1000.0) / (fuel_milliliters / 1000.0);
    return 0;
}

int main(void)
{
    double kmpl;
    int r;

    r = compute_kmpl(0,     500,  &kmpl);
    printf("TC1: ret=%d kmpl=%.4f  (exp 0, 0.0000)\n", r, kmpl);

    r = compute_kmpl(50000, 2500, &kmpl);
    printf("TC2: ret=%d kmpl=%.4f (exp 0, 20.0000)\n", r, kmpl);

    r = compute_kmpl(12345, 678,  &kmpl);
    printf("TC3: ret=%d kmpl=%.4f (exp 0, ~18.2124)\n", r, kmpl);

    r = compute_kmpl(1000,  0,    &kmpl);
    printf("TC4: ret=%d         (exp -1)\n", r);

    r = compute_kmpl(-10,   100,  &kmpl);
    printf("TC5: ret=%d         (exp -1)\n", r);

    r = compute_kmpl(1000,  1000, NULL);
    printf("TC6: ret=%d         (exp -1)\n", r);

    r = compute_kmpl(999,   1000, &kmpl);
    printf("TC7: ret=%d kmpl=%.4f  (exp 0, 0.9990)\n", r, kmpl);

    return 0;
}