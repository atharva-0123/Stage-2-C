#include <stddef.h>
#include <stdio.h>

int count_above_threshold(const int *a, int n, int threshold, int *out_count)
{
    if (a == NULL || out_count == NULL || n < 0) return -1;

    int count = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] > threshold) count++;
    }

    *out_count = count;
    return 0;
}

int main(void)
{
    int count, r;

    int a1[] = {1, 5, 9};
    r = count_above_threshold(a1, 3, 4, &count);
    printf("TC1: ret=%d count=%d  (exp 0,2)\n", r, count);

    int a2[] = {10, 10, 10};
    r = count_above_threshold(a2, 3, 10, &count);
    printf("TC2: ret=%d count=%d  (exp 0,0)\n", r, count);

    int a3[] = {-1, -5, -3};
    r = count_above_threshold(a3, 3, -4, &count);
    printf("TC3: ret=%d count=%d  (exp 0,2)\n", r, count);

    r = count_above_threshold(a1, 0, 5, &count);
    printf("TC4: ret=%d count=%d  (exp 0,0)\n", r, count);

    r = count_above_threshold(NULL, 3, 5, &count);
    printf("TC5: ret=%d          (exp -1)\n", r);

    r = count_above_threshold(a1, 3, 5, NULL);
    printf("TC6: ret=%d          (exp -1)\n", r);

    r = count_above_threshold(a1, -1, 5, &count);
    printf("TC7: ret=%d          (exp -1)\n", r);

    return 0;
}