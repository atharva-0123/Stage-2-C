#include <stddef.h>
#include <stdio.h>

int find_min_in_array(const int *a, int n, int *out_min)
{
    if (a == NULL || out_min == NULL || n <= 0) return -1;

    int min_value = a[0];
    for (int i = 1; i < n; i++) {
        if (a[i] < min_value) min_value = a[i];
    }

    *out_min = min_value;
    return 0;
}

int main(void)
{
    int min, r;

    int a1[] = {5, 2, 8, 1, 9};
    r = find_min_in_array(a1, 5, &min);
    printf("TC1: ret=%d min=%d   (exp 0,1)\n", r, min);

    int a2[] = {10};
    r = find_min_in_array(a2, 1, &min);
    printf("TC2: ret=%d min=%d  (exp 0,10)\n", r, min);

    int a3[] = {-5, -2, -9, -1};
    r = find_min_in_array(a3, 4, &min);
    printf("TC3: ret=%d min=%d  (exp 0,-9)\n", r, min);

    int a4[] = {100, 100, 100};
    r = find_min_in_array(a4, 3, &min);
    printf("TC4: ret=%d min=%d (exp 0,100)\n", r, min);

    r = find_min_in_array(a1, 0, &min);
    printf("TC5: ret=%d        (exp -1)\n", r);

    r = find_min_in_array(NULL, 5, &min);
    printf("TC6: ret=%d        (exp -1)\n", r);

    r = find_min_in_array(a1, 5, NULL);
    printf("TC7: ret=%d        (exp -1)\n", r);

    return 0;
}