#include <stddef.h>
#include <stdio.h>

enum ParseStatus {
    PARSE_OK               = 0,
    PARSE_OVERFLOW         = 1,
    PARSE_TIMEOUT          = 2,
    PARSE_NEED_MORE        = 3,
    PARSE_PARTIAL_NO_DELIM = 4
};

int parse_record_while(const char *in,
                       int n,
                       int *index,
                       char delimiter,
                       char *out,
                       int out_capacity,
                       int max_scan_without_delim,
                       int *out_len,
                       enum ParseStatus *out_status)
{
    /* 1. Validate */
    if (in == NULL || index == NULL || out == NULL ||
        out_len == NULL || out_status == NULL)       return -1;
    if (n < 0 || out_capacity <= 0)                  return -1;
    if (max_scan_without_delim < 0)                  return -1;
    if (*index < 0 || *index > n)                    return -1;

    /* 2. Locals */
    int out_count = 0;
    int scanned   = 0;
    int i         = *index;

    /* 3. While loop */
    while (i < n && scanned <= max_scan_without_delim) {
        char c = in[i];

        if (c == delimiter) {
            *index      = i + 1;
            *out_len    = out_count;
            *out_status = PARSE_OK;
            return 0;
        }

        if (out_count == out_capacity) {
            *out_len    = out_count;
            *out_status = PARSE_OVERFLOW;
            /* do not advance *index */
            return 0;
        }

        if ((c < ' ' && c != '\t' && c != '\r') || c > '~') {
            i++;
            scanned++;
            continue;
        }

        out[out_count++] = c;
        i++;
        scanned++;
    }

    /* 4. Post-loop */
    if (scanned > max_scan_without_delim) {
        *out_len    = out_count;
        *out_status = PARSE_TIMEOUT;
        *index      = i;
        return 0;
    }

    /* i == n */
    *out_len    = out_count;
    *out_status = (out_count == 0) ? PARSE_NEED_MORE : PARSE_PARTIAL_NO_DELIM;
    *index      = i;
    return 0;
}

/* ── helpers ─────────────────────────────────────────────────────── */
static const char *status_str(enum ParseStatus s)
{
    if (s == PARSE_OK)               return "PARSE_OK";
    if (s == PARSE_OVERFLOW)         return "PARSE_OVERFLOW";
    if (s == PARSE_TIMEOUT)          return "PARSE_TIMEOUT";
    if (s == PARSE_NEED_MORE)        return "PARSE_NEED_MORE";
    if (s == PARSE_PARTIAL_NO_DELIM) return "PARSE_PARTIAL_NO_DELIM";
    return "UNKNOWN";
}

int main(void)
{
    char out[16];
    int  idx, out_len, r;
    enum ParseStatus st;

    /* TC1 */
    idx = 0;
    r = parse_record_while("ABC\nXYZ", 7, &idx, '\n', out, 8, 100, &out_len, &st);
    out[out_len] = '\0';
    printf("TC1: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_OK,\"ABC\",3,4)\n", r, status_str(st), out, out_len, idx);

    /* TC2 */
    idx = 0;
    r = parse_record_while("ABCD", 4, &idx, '\n', out, 3, 100, &out_len, &st);
    out[out_len] = '\0';
    printf("TC2: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_OVERFLOW,\"ABC\",3,0)\n", r, status_str(st), out, out_len, idx);

    /* TC3 */
    idx = 0;
    r = parse_record_while("A\tB\rC\n", 6, &idx, '\n', out, 8, 100, &out_len, &st);
    out[out_len] = '\0';
    printf("TC3: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_OK,\"A\\tB\\rC\",5,6)\n", r, status_str(st), out, out_len, idx);

    /* TC4 */
    idx = 0;
    r = parse_record_while("\x01" "A\x7F\n", 4, &idx, '\n', out, 8, 100, &out_len, &st);
    out[out_len] = '\0';
    printf("TC4: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_OK,\"A\",1,4)\n", r, status_str(st), out, out_len, idx);

    /* TC5 */
    idx = 0;
    r = parse_record_while("ABCDEFGHI", 9, &idx, '\n', out, 8, 5, &out_len, &st);
    out[out_len] = '\0';
    printf("TC5: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_TIMEOUT,\"ABCDE\",5,5)\n", r, status_str(st), out, out_len, idx);

    /* TC6 */
    idx = 0;
    r = parse_record_while("", 0, &idx, '\n', out, 8, 10, &out_len, &st);
    out[out_len] = '\0';
    printf("TC6: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_NEED_MORE,\"\",0,0)\n", r, status_str(st), out, out_len, idx);

    /* TC7 */
    idx = 0;
    r = parse_record_while("PARTIAL", 7, &idx, '\n', out, 8, 100, &out_len, &st);
    out[out_len] = '\0';
    printf("TC7: ret=%d st=%-22s out=\"%s\" len=%d idx=%d\n"
           "     (exp 0,PARSE_PARTIAL_NO_DELIM,\"PARTIAL\",7,7)\n", r, status_str(st), out, out_len, idx);

    /* TC8 */
    r = parse_record_while("ABC", 3, NULL, '\n', out, 8, 10, &out_len, &st);
    printf("TC8: ret=%d (exp -1)\n", r);

    /* TC9 */
    idx = 0;
    r = parse_record_while("ABC", -1, &idx, '\n', out, 8, 10, &out_len, &st);
    printf("TC9: ret=%d (exp -1)\n", r);

    return 0;
}