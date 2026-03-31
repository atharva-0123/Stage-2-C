#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int calib_map_linear(int x,
                     int x1, int y1,
                     int x2, int y2,
                     bool clamp, int y_min, int y_max,
                     int *out_y)
{
    if (out_y == NULL)          return -1;
    if (x1 == x2)               return -1;
    if (clamp && y_min > y_max) return -1;

    long num = (long)(x  - x1) * (long)(y2 - y1);
    long den = (long)(x2 - x1);

    long adj  = (num >= 0) ? (den / 2) : -(den / 2);
    long term = (num + adj) / den;

    long y_long = (long)y1 + term;

    if (clamp) {
        if (y_long < y_min) y_long = y_min;
        if (y_long > y_max) y_long = y_max;
    }

    *out_y = (int)y_long;
    return 0;
}

int main(void)
{
    int y, r;

    r = calib_map_linear(75,  50,0,   100,100, false, 0,   0,   &y);
    printf("TC1: ret=%d y=%d   (exp 0, 50)\n",  r, y);

    r = calib_map_linear(100, 50,0,   100,100, false, 0,   0,   &y);
    printf("TC2: ret=%d y=%d  (exp 0, 100)\n", r, y);

    r = calib_map_linear(120, 50,0,   100,100, true,  0,   100, &y);
    printf("TC3: ret=%d y=%d  (exp 0, 100)\n", r, y);

    r = calib_map_linear(40,  50,0,   100,100, true,  0,   100, &y);
    printf("TC4: ret=%d y=%d    (exp 0, 0)\n", r, y);

    r = calib_map_linear(60,  100,200, 200,400, false, 0,   0,   &y);
    printf("TC5: ret=%d y=%d  (exp 0, 120)\n", r, y);

    r = calib_map_linear(10,  10,500,  10,900,  false, 0,   0,   &y);
    printf("TC6: ret=%d        (exp -1)\n",     r);

    r = calib_map_linear(75,  50,0,   100,100, false, 0,   0,   NULL);
    printf("TC7: ret=%d        (exp -1)\n",     r);

    return 0;
}