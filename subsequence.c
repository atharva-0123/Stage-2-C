#include <stddef.h>
#include <stdio.h>

int find_subsequence_in_slice(const char *buf, int n,
                               const char *needle, int m,
                               char **out_ptr, int *out_offset)
{
    if (buf == NULL || needle == NULL ||
        out_ptr == NULL || out_offset == NULL) return -1;
    if (n < 0 || m <= 0)                       return -1;

    const char *end = buf + n;

    for (const char *p = buf; p + m <= end; ++p) {
        const char *q = p;
        const char *r = needle;
        while (r < needle + m && *q == *r) { q++; r++; }
        if (r == needle + m) {
            *out_ptr    = (char *)p;
            *out_offset = (int)(p - buf);
            return 0;
        }
    }

    *out_ptr    = NULL;
    *out_offset = -1;
    return 0;
}

int main(void)
{
    char *ptr;
    int   off, r;

    const char *b1 = "ABCDEF";
    r = find_subsequence_in_slice(b1, 6, "CDE", 3, &ptr, &off);
    printf("TC1: ret=%d off=%d ptr=%s  (exp 0,2,\"CDEF\")\n", r, off, ptr);

    const char *b2 = "AAAAA";
    r = find_subsequence_in_slice(b2, 5, "AAA", 3, &ptr, &off);
    printf("TC2: ret=%d off=%d ptr=%s  (exp 0,0,\"AAAAA\")\n", r, off, ptr);

    const char *b3 = "ABCDE";
    r = find_subsequence_in_slice(b3, 5, "DEF", 3, &ptr, &off);
    printf("TC3: ret=%d off=%d ptr=%p  (exp 0,-1,NULL)\n", r, off, (void*)ptr);

    const char *b4 = "XYZ";
    r = find_subsequence_in_slice(b4, 3, "XYZ", 3, &ptr, &off);
    printf("TC4: ret=%d off=%d ptr=%s  (exp 0,0,\"XYZ\")\n", r, off, ptr);

    const char *b5 = "HI";
    r = find_subsequence_in_slice(b5, 2, "HIK", 3, &ptr, &off);
    printf("TC5: ret=%d off=%d ptr=%p  (exp 0,-1,NULL)\n", r, off, (void*)ptr);

    r = find_subsequence_in_slice(NULL, 5, "AB", 2, &ptr, &off);
    printf("TC6: ret=%d                (exp -1)\n", r);

    r = find_subsequence_in_slice(b1, -1, "AB", 2, &ptr, &off);
    printf("TC7: ret=%d                (exp -1)\n", r);

    return 0;
}