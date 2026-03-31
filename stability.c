#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static int abs_int(int v) { return v < 0 ? -v : v; }

int detect_stability_do_while(const int *samples,
                               int n,
                               int start,
                               int delta,
                               int k_consecutive,
                               int max_to_check,
                               bool *out_stable,
                               int  *out_examined,
                               int  *out_end_index)
{
    /* 1. Validate */
    if (samples == NULL || out_stable == NULL ||
        out_examined == NULL || out_end_index == NULL) return -1;
    if (n < 0 || delta < 0 || k_consecutive < 1 || max_to_check < 1) return -1;
    if (n == 0 || start < 0 || start >= n) return -1;

    /* 2. Init */
    int ref      = samples[start];
    int i        = start;
    int streak   = 0;
    int examined = 0;

    /* 3. do-while */
    do {
        int x = samples[i];
        if (abs_int(x - ref) <= delta) streak++;
        else                           streak = 0;
        examined++;

        if (streak == k_consecutive) {
            *out_stable    = true;
            *out_end_index = i;
            *out_examined  = examined;
            return 0;
        }
        i++;
    } while (i < n && examined < max_to_check);

    /* 4. No stability */
    *out_stable   = false;
    *out_examined = examined;
    return 0;
}

int main(void)
{
    bool stable;
    int  examined, end_idx, r;

    int s1[] = {100, 101, 100, 99, 150};
    r = detect_stability_do_while(s1, 5, 0, 2, 3, 5, &stable, &examined, &end_idx);
    printf("TC1:  ret=%d stable=%d examined=%d end=%d  (exp 0,1,3,2)\n",
           r, stable, examined, end_idx);

    int s2[] = {100, 120, 100, 101, 102};
    r = detect_stability_do_while(s2, 5, 0, 1, 3, 5, &stable, &examined, &end_idx);
    printf("TC2:  ret=%d stable=%d examined=%d         (exp 0,0,5)\n",
           r, stable, examined);

    int s3[] = {200};
    r = detect_stability_do_while(s3, 1, 0, 0, 1, 10, &stable, &examined, &end_idx);
    printf("TC3:  ret=%d stable=%d examined=%d end=%d  (exp 0,1,1,0)\n",
           r, stable, examined, end_idx);

    int s4[] = {50, 49, 51, 48, 52};
    r = detect_stability_do_while(s4, 5, 1, 2, 2, 2, &stable, &examined, &end_idx);
    printf("TC4:  ret=%d stable=%d examined=%d end=%d  (exp 0,1,2,2)\n",
           r, stable, examined, end_idx);

    int s5[] = {10, 30, 10, 30, 10};
    r = detect_stability_do_while(s5, 5, 0, 0, 2, 5, &stable, &examined, &end_idx);
    printf("TC5:  ret=%d stable=%d examined=%d         (exp 0,0,5)\n",
           r, stable, examined);

    int s6[] = {5, 6, 7, 8};
    r = detect_stability_do_while(s6, 4, 3, 0, 2, 3, &stable, &examined, &end_idx);
    printf("TC6:  ret=%d stable=%d examined=%d         (exp 0,0,1)\n",
           r, stable, examined);

    int s7[] = {100, 101, 100, 103};
    r = detect_stability_do_while(s7, 4, 0, 1, 3, 3, &stable, &examined, &end_idx);
    printf("TC7:  ret=%d stable=%d examined=%d end=%d  (exp 0,1,3,2)\n",
           r, stable, examined, end_idx);

    r = detect_stability_do_while(NULL, 5, 0, 2, 3, 5, &stable, &examined, &end_idx);
    printf("TC8:  ret=%d (exp -1)\n", r);

    r = detect_stability_do_while(s1, 0, 0, 2, 3, 5, &stable, &examined, &end_idx);
    printf("TC9:  ret=%d (exp -1)\n", r);

    r = detect_stability_do_while(s1, 4, 5, 2, 3, 5, &stable, &examined, &end_idx);
    printf("TC10: ret=%d (exp -1)\n", r);

    r = detect_stability_do_while(s1, 5, 0, -1, 3, 5, &stable, &examined, &end_idx);
    printf("TC11: ret=%d (exp -1)\n", r);

    return 0;
}