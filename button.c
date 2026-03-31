#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int debounce_and_fire(const int *samples,
                      int n,
                      int k,
                      void (*on_press)(int event_code),
                      bool *out_invoked)
{
    if (samples == NULL || on_press == NULL || out_invoked == NULL) return -1;
    if (n < 0 || k < 1) return -1;

    if (n < k) { *out_invoked = false; return 0; }

    bool all_one = true;
    for (int i = n - k; i < n; i++) {
        if (samples[i] != 1) { all_one = false; break; }
    }

    if (all_one) { on_press(1); *out_invoked = true; }
    else         {              *out_invoked = false; }

    return 0;
}

static void my_callback(int event_code)
{
    printf("     [callback fired: event_code=%d]\n", event_code);
}

int main(void)
{
    bool invoked;
    int  r;

    int s1[] = {0,1,1,1};
    r = debounce_and_fire(s1, 4, 3, my_callback, &invoked);
    printf("TC1: ret=%d invoked=%d  (exp 0,true)\n\n", r, invoked);

    int s2[] = {1,1,0,1};
    r = debounce_and_fire(s2, 4, 2, my_callback, &invoked);
    printf("TC2: ret=%d invoked=%d  (exp 0,false)\n\n", r, invoked);

    int s3[] = {1,1};
    r = debounce_and_fire(s3, 2, 3, my_callback, &invoked);
    printf("TC3: ret=%d invoked=%d  (exp 0,false)\n\n", r, invoked);

    int s4[] = {1,1,1,1};
    r = debounce_and_fire(s4, 4, 4, my_callback, &invoked);
    printf("TC4: ret=%d invoked=%d  (exp 0,true)\n\n", r, invoked);

    int s5[] = {0,0,0};
    r = debounce_and_fire(s5, 3, 1, my_callback, &invoked);
    printf("TC5: ret=%d invoked=%d  (exp 0,false)\n\n", r, invoked);

    r = debounce_and_fire(NULL, 4, 3, my_callback, &invoked);
    printf("TC6: ret=%d             (exp -1)\n\n", r);

    r = debounce_and_fire(s1, -1, 3, my_callback, &invoked);
    printf("TC7: ret=%d             (exp -1)\n\n", r);

    return 0;
}