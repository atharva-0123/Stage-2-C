#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static int abs_int(int v) { return v < 0 ? -v : v; }

int reconcile_config_with_defaults(const int *defaults,
                                   int       *current,
                                   int        n,
                                   int        tolerance,
                                   bool       dry_run,
                                   int       *out_mismatches,
                                   int       *out_updates,
                                   int       *out_first_index)
{
    if (defaults == NULL || current == NULL ||
        out_mismatches == NULL || out_updates == NULL ||
        out_first_index == NULL)          return -1;
    if (n < 0 || tolerance < 0)          return -1;
    if (defaults == (const int *)current) return -2;

    if (n == 0) {
        *out_mismatches  = 0;
        *out_updates     = 0;
        *out_first_index = -1;
        return 0;
    }

    int mismatches = 0, updates = 0, first = -1;

    for (int i = 0; i < n; i++) {
        int diff = current[i] - defaults[i];
        if (abs_int(diff) > tolerance) {
            mismatches++;
            if (first < 0) first = i;
            updates++;
            if (!dry_run) current[i] = defaults[i];
        }
    }

    *out_mismatches  = mismatches;
    *out_updates     = updates;
    *out_first_index = first;
    return 0;
}

int main(void)
{
    int mm, upd, fi, r;

    /* TC1 */
    const int d1[] = {10, 20, 30};
    int       c1[] = {10, 22, 27};
    r = reconcile_config_with_defaults(d1, c1, 3, 2, false, &mm, &upd, &fi);
    printf("TC1: ret=%d mm=%d upd=%d fi=%d cur=[%d,%d,%d]\n"
           "     (exp 0,1,1,1,[10,20,27])\n", r,mm,upd,fi,c1[0],c1[1],c1[2]);

    /* TC2 dry_run */
    int c2[] = {10, 22, 27};
    r = reconcile_config_with_defaults(d1, c2, 3, 2, true, &mm, &upd, &fi);
    printf("TC2: ret=%d mm=%d upd=%d fi=%d cur=[%d,%d,%d]\n"
           "     (exp 0,1,1,1,[10,22,27])\n", r,mm,upd,fi,c2[0],c2[1],c2[2]);

    /* TC3 tol=0 */
    const int d3[] = {0, 0, 0};
    int       c3[] = {5, -1, 0};
    r = reconcile_config_with_defaults(d3, c3, 3, 0, false, &mm, &upd, &fi);
    printf("TC3: ret=%d mm=%d upd=%d fi=%d cur=[%d,%d,%d]\n"
           "     (exp 0,2,2,0,[0,0,0])\n", r,mm,upd,fi,c3[0],c3[1],c3[2]);

    /* TC4 all in-spec */
    const int d4[] = {1, 2, 3};
    int       c4[] = {1, 2, 3};
    r = reconcile_config_with_defaults(d4, c4, 3, 5, true, &mm, &upd, &fi);
    printf("TC4: ret=%d mm=%d upd=%d fi=%d\n"
           "     (exp 0,0,0,-1)\n", r,mm,upd,fi);

    /* TC5 within tol */
    const int d5[] = {100};
    int       c5[] = {120};
    r = reconcile_config_with_defaults(d5, c5, 1, 25, false, &mm, &upd, &fi);
    printf("TC5: ret=%d mm=%d upd=%d fi=%d cur=[%d]\n"
           "     (exp 0,0,0,-1,[120])\n", r,mm,upd,fi,c5[0]);

    /* TC6 n=0 */
    const int d6[] = {7, 8};
    int       c6[] = {7, 8};
    r = reconcile_config_with_defaults(d6, c6, 0, 0, false, &mm, &upd, &fi);
    printf("TC6: ret=%d mm=%d upd=%d fi=%d\n"
           "     (exp 0,0,0,-1)\n", r,mm,upd,fi);

    /* TC7 aliasing */
    int ca[] = {1, 2, 3};
    r = reconcile_config_with_defaults((const int *)ca, ca, 3, 0, false,
                                       &mm, &upd, &fi);
    printf("TC7: ret=%d (exp -2)\n", r);

    /* TC8 NULL */
    r = reconcile_config_with_defaults(NULL, c1, 3, 0, false, &mm, &upd, &fi);
    printf("TC8: ret=%d (exp -1)\n", r);

    /* TC9 invalid params */
    r = reconcile_config_with_defaults(d1, c1, 3, -1, false, &mm, &upd, &fi);
    printf("TC9: ret=%d (exp -1)\n", r);

    return 0;
}