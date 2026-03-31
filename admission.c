#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum AdmissionReason {
    REASON_ACCEPTED     = 0,
    REASON_MAINTENANCE  = 1,
    REASON_COOLDOWN     = 2,
    REASON_MIN_GAP      = 3,
    REASON_WINDOW_LIMIT = 4
};

int decide_admission(long  now_ms,
                     long *window_start_ms,
                     int   window_ms,
                     int   max_in_window,
                     long  min_gap_ms,
                     long  cooldown_ms,
                     long *last_accept_ms,
                     long *cooldown_until_ms,
                     int  *count_in_window,
                     bool  maintenance_mode,
                     bool  emergency_override,
                     int  *out_decision,
                     enum AdmissionReason *out_reason)
{
    /* 1. Validate pointers */
    if (window_start_ms  == NULL) return -1;
    if (last_accept_ms   == NULL) return -1;
    if (cooldown_until_ms== NULL) return -1;
    if (count_in_window  == NULL) return -1;
    if (out_decision     == NULL) return -1;
    if (out_reason       == NULL) return -1;

    /* 2. Validate numeric constraints */
    if (now_ms            <  0)  return -1;
    if (*window_start_ms  <  0)  return -1;
    if (window_ms         <= 0)  return -1;
    if (max_in_window     <  1)  return -1;
    if (min_gap_ms        <  0)  return -1;
    if (cooldown_ms       <  0)  return -1;
    if (*count_in_window  <  0)  return -1;

    /* 3. Roll window if expired */
    if (now_ms >= *window_start_ms + window_ms) {
        *window_start_ms = now_ms;
        *count_in_window = 0;
    }

    /* 4. Priority if/else chain */
    if (maintenance_mode && !emergency_override) {
        *out_decision = 0;
        *out_reason   = REASON_MAINTENANCE;
    }
    else if (now_ms < *cooldown_until_ms) {
        *out_decision = 0;
        *out_reason   = REASON_COOLDOWN;
    }
    else if (*last_accept_ms >= 0 && (now_ms - *last_accept_ms) < min_gap_ms) {
        *out_decision = 0;
        *out_reason   = REASON_MIN_GAP;
    }
    else if (*count_in_window >= max_in_window) {
        *cooldown_until_ms = now_ms + cooldown_ms;
        *out_decision = 0;
        *out_reason   = REASON_WINDOW_LIMIT;
    }
    else {
        *last_accept_ms = now_ms;
        (*count_in_window)++;
        *out_decision = 1;
        *out_reason   = REASON_ACCEPTED;
    }

    return 0;
}

/* ── helpers ─────────────────────────────────────────────────────── */
static const char *reason_str(enum AdmissionReason r)
{
    if (r == REASON_ACCEPTED)     return "ACCEPTED";
    if (r == REASON_MAINTENANCE)  return "MAINTENANCE";
    if (r == REASON_COOLDOWN)     return "COOLDOWN";
    if (r == REASON_MIN_GAP)      return "MIN_GAP";
    if (r == REASON_WINDOW_LIMIT) return "WINDOW_LIMIT";
    return "UNKNOWN";
}

int main(void)
{
    int  ret, decision, count;
    long window_start, last_accept, cooldown_until;
    enum AdmissionReason reason;

    /* shared state reset */
#define RESET() do { \
    window_start   = 1000; \
    last_accept    = -1;   \
    cooldown_until = 0;    \
    count          = 0;    \
} while(0)

    /* TC1 */
    RESET();
    ret = decide_admission(1000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC1:  ret=%d dec=%d reason=%-14s (exp 0,1,ACCEPTED)\n",
           ret, decision, reason_str(reason));

    /* TC2 — 200ms later, violates min_gap */
    ret = decide_admission(1200, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC2:  ret=%d dec=%d reason=%-14s (exp 0,0,MIN_GAP)\n",
           ret, decision, reason_str(reason));

    /* TC3 — 700ms after TC1 */
    ret = decide_admission(1700, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC3:  ret=%d dec=%d reason=%-14s (exp 0,1,ACCEPTED) count=%d\n",
           ret, decision, reason_str(reason), count);

    /* TC4 — 600ms after TC3, hits quota=3 */
    ret = decide_admission(2300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC4:  ret=%d dec=%d reason=%-14s (exp 0,1,ACCEPTED) count=%d\n",
           ret, decision, reason_str(reason), count);

    /* TC5 — exceeds quota, cooldown triggers */
    ret = decide_admission(2400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC5:  ret=%d dec=%d reason=%-14s (exp 0,0,WINDOW_LIMIT) cooldown_until=%ld\n",
           ret, decision, reason_str(reason), cooldown_until);

    /* TC6 — during cooldown */
    ret = decide_admission(5300, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC6:  ret=%d dec=%d reason=%-14s (exp 0,0,COOLDOWN)\n",
           ret, decision, reason_str(reason));

    /* TC7 — cooldown ends but window still full */
    ret = decide_admission(5400, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC7:  ret=%d dec=%d reason=%-14s (exp 0,0,WINDOW_LIMIT)\n",
           ret, decision, reason_str(reason));

    /* TC8 — new window rolls */
    ret = decide_admission(11000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC8:  ret=%d dec=%d reason=%-14s (exp 0,1,ACCEPTED) window_start=%ld count=%d\n",
           ret, decision, reason_str(reason), window_start, count);

    /* TC9 — maintenance block */
    ret = decide_admission(12000, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, false, &decision, &reason);
    printf("TC9:  ret=%d dec=%d reason=%-14s (exp 0,0,MAINTENANCE)\n",
           ret, decision, reason_str(reason));

    /* TC10 — emergency override bypasses maintenance */
    ret = decide_admission(12550, &window_start, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           true, true, &decision, &reason);
    printf("TC10: ret=%d dec=%d reason=%-14s (exp 0, bypass maint → next guard)\n",
           ret, decision, reason_str(reason));

    /* TC11 — invalid: window_ms=0 */
    ret = decide_admission(12000, &window_start, 0, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC11: ret=%d         (exp -1)\n", ret);

    /* TC12 — invalid: NULL pointer */
    ret = decide_admission(12000, NULL, 10000, 3, 500, 3000,
                           &last_accept, &cooldown_until, &count,
                           false, false, &decision, &reason);
    printf("TC12: ret=%d         (exp -1)\n", ret);

    return 0;
}