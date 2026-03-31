#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int dynint_append(int **buf, int *count, int *capacity, int value)
{
    /* 1. Validate */
    if (buf == NULL || count == NULL || capacity == NULL) return -1;
    if (*count < 0 || *capacity < 0)                     return -1;
    if (*count > *capacity)                              return -1;

    /* 2. Grow if needed */
    if (*count == *capacity) {
        int new_cap;

        if (*capacity == 0) {
            new_cap = 4;
        } else {
            /* overflow check: new_cap = capacity * 2 */
            if (*capacity > INT_MAX / 2) {
                /* try minimal growth */
                if (*capacity >= INT_MAX - 1) return -3;
                new_cap = *capacity + 1;
            } else {
                new_cap = *capacity * 2;
            }
        }

        /* ensure new_cap >= count + 1 */
        if (new_cap < *count + 1) return -3;

        /* ensure byte size doesn't overflow long */
        long byte_size = (long)new_cap * (long)sizeof(int);
        if (byte_size > (long)INT_MAX)  return -3;

        /* no-leak realloc pattern */
        int *tmp;
        if (*capacity == 0)
            tmp = malloc((size_t)byte_size);
        else
            tmp = realloc(*buf, (size_t)byte_size);

        if (tmp == NULL) return -2;

        *buf      = tmp;
        *capacity = new_cap;
    }

    /* 3. Append */
    (*buf)[*count] = value;
    (*count)++;
    return 0;
}

int main(void)
{
    int *buf      = NULL;
    int  count    = 0;
    int  capacity = 0;
    int  r;

    /* TC1 */
    r = dynint_append(&buf, &count, &capacity, 10);
    printf("TC1: ret=%d count=%d cap=%d buf[0]=%d  (exp 0,1,4,10)\n",
           r, count, capacity, buf[0]);

    /* TC2 */
    dynint_append(&buf, &count, &capacity, 20);
    dynint_append(&buf, &count, &capacity, 30);
    r = dynint_append(&buf, &count, &capacity, 40);
    printf("TC2: ret=%d count=%d cap=%d buf={%d,%d,%d,%d}  (exp 0,4,4)\n",
           r, count, capacity, buf[0], buf[1], buf[2], buf[3]);

    /* TC3 */
    r = dynint_append(&buf, &count, &capacity, 50);
    printf("TC3: ret=%d count=%d cap=%d buf[4]=%d  (exp 0,5,8,50)\n",
           r, count, capacity, buf[4]);

    /* TC4 */
    int bad_count = -1, bad_cap = 4;
    r = dynint_append(&buf, &bad_count, &bad_cap, 1);
    printf("TC4: ret=%d  (exp -1)\n", r);

    /* TC5 */
    int  near_max_cap = INT_MAX / 2 + 1;
    int  near_max_cnt = near_max_cap;   /* count == capacity → triggers grow */
    int *fake_buf     = buf;            /* just needs a non-NULL pointer */
    r = dynint_append(&fake_buf, &near_max_cnt, &near_max_cap, 99);
    printf("TC5: ret=%d  (exp -3 or -2)\n", r);

    /* cleanup */
    free(buf);
    return 0;
}