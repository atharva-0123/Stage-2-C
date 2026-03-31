#include <stddef.h>
#include <stdio.h>

static int abs_int(int v) { return v < 0 ? -v : v; }

int find_longest_stable_segment(const int *a, int n,
                                int step_max, int span_max,
                                double avg_min, double avg_max,
                                int min_len, int max_len,
                                int *out_start, int *out_end,
                                int *out_len,  double *out_avg)
{
    /* 1. Validate */
    if (a == NULL || out_start == NULL || out_end == NULL ||
        out_len == NULL || out_avg == NULL)          return -1;
    if (n < 0 || step_max < 0 || span_max < 0)      return -1;
    if (min_len < 1 || avg_min > avg_max)            return -1;
    if (max_len > 0 && max_len < min_len)            return -1;
    if (n == 0)                                      return -2;

    /* 2. Best trackers */
    int    best_L   = -1, best_R = -1, best_len = 0;
    double best_avg = 0.0;

    /* 3. Brute-force O(n^2) */
    for (int L = 0; L < n; L++) {
        long sum  = a[L];
        int  vmin = a[L];
        int  vmax = a[L];

        for (int R = L; R < n; R++) {
            /* step check */
            if (R > L) {
                if (abs_int(a[R] - a[R-1]) > step_max) break;
                sum  += a[R];
                if (a[R] < vmin) vmin = a[R];
                if (a[R] > vmax) vmax = a[R];
            }

            int len = R - L + 1;

            /* upper length bound */
            if (max_len > 0 && len > max_len) break;

            /* lower length bound */
            if (len < min_len) continue;

            /* span check */
            if (vmax - vmin > span_max) continue;

            /* average check */
            double mean = (double)sum / (double)len;
            if (mean < avg_min || mean > avg_max) continue;

            /* tie-breakers: longer > higher avg > earlier start */
            if (len > best_len ||
               (len == best_len && mean > best_avg) ||
               (len == best_len && mean == best_avg && L < best_L)) {
                best_L   = L;
                best_R   = R;
                best_len = len;
                best_avg = mean;
            }
        }
    }

    /* 4. Finalize */
    if (best_len == 0) return -2;

    *out_start = best_L;
    *out_end   = best_R;
    *out_len   = best_len;
    *out_avg   = best_avg;
    return 0;
}

int main(void)
{
    int    s, e, l, r;
    double avg;

    int a1[] = {10,12,13,13,15,20};
    r = find_longest_stable_segment(a1,6, 3,6, 11.0,15.0, 3,0, &s,&e,&l,&avg);
    printf("TC1: ret=%d [%d..%d] len=%d avg=%.4f  (exp 0,[0..4],5,12.6)\n",r,s,e,l,avg);

    int a2[] = {5,5,5,5};
    r = find_longest_stable_segment(a2,4, 0,0, 5.0,5.0, 2,0, &s,&e,&l,&avg);
    printf("TC2: ret=%d [%d..%d] len=%d avg=%.4f  (exp 0,[0..3],4,5.0)\n",r,s,e,l,avg);

    int a3[] = {1,20,2,21,3,22};
    r = find_longest_stable_segment(a3,6, 3,5, 0.0,100.0, 2,0, &s,&e,&l,&avg);
    printf("TC3: ret=%d [%d..%d] len=%d avg=%.4f  (exp 0,[4..5],2,12.5)\n",r,s,e,l,avg);

    int a4[] = {3,4,7,8,9};
    r = find_longest_stable_segment(a4,5, 4,3, 5.0,7.0, 2,3, &s,&e,&l,&avg);
    printf("TC4: ret=%d [%d..%d] len=%d avg=%.4f  (exp 0,[2..3],2,7.5)\n",r,s,e,l,avg);

    int a5[] = {100};
    r = find_longest_stable_segment(a5,1, 0,0, 200.0,300.0, 1,0, &s,&e,&l,&avg);
    printf("TC5: ret=%d                              (exp -2)\n",r);

    int a6[] = {-2,-1,-1,0,1};
    r = find_longest_stable_segment(a6,5, 2,3, -1.5,0.5, 3,0, &s,&e,&l,&avg);
    printf("TC6: ret=%d [%d..%d] len=%d avg=%.4f  (exp 0,[0..4],5,-0.6)\n",r,s,e,l,avg);

    r = find_longest_stable_segment(NULL,5, 1,1, 0.0,1.0, 1,0, &s,&e,&l,&avg);
    printf("TC7: ret=%d                              (exp -1)\n",r);

    int a8[] = {1,2,3};
    r = find_longest_stable_segment(a8,0, 1,1, 0.0,5.0, 1,0, &s,&e,&l,&avg);
    printf("TC8: ret=%d                              (exp -2)\n",r);

    r = find_longest_stable_segment(a8,3, 1,1, 0.0,5.0, 0,0, &s,&e,&l,&avg);
    printf("TC9: ret=%d                              (exp -1)\n",r);

    return 0;
}