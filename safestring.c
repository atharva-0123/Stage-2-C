#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int safe_copy_label(const char *src,
                    char       *dst,
                    int         dst_capacity,
                    bool        to_lower,
                    int        *out_copied,
                    bool       *out_truncated)
{
    if (src == NULL || dst == NULL ||
        out_copied == NULL || out_truncated == NULL) return -1;
    if (dst_capacity < 0) return -1;

    int limit = (dst_capacity > 0) ? (dst_capacity - 1) : 0;

    int i = 0;
    while (i < limit && src[i] != '\0') {
        char ch = src[i];
        if (to_lower && ch >= 'A' && ch <= 'Z')
            ch = (char)(ch + ('a' - 'A'));
        dst[i] = ch;
        i++;
    }

    if (dst_capacity > 0) dst[i] = '\0';

    *out_copied    = i;
    *out_truncated = (src[i] != '\0');
    return 0;
}

int main(void)
{
    char buf[16];
    int  copied, r;
    bool truncated;

    r = safe_copy_label("Hello", buf, 10, false, &copied, &truncated);
    printf("TC1: ret=%d copied=%d trunc=%d dst=\"%s\"  (exp 0,5,0,\"Hello\")\n",
           r, copied, truncated, buf);

    r = safe_copy_label("Hello", buf, 6, true, &copied, &truncated);
    printf("TC2: ret=%d copied=%d trunc=%d dst=\"%s\"  (exp 0,5,0,\"hello\")\n",
           r, copied, truncated, buf);

    r = safe_copy_label("FirmwareV1", buf, 5, false, &copied, &truncated);
    printf("TC3: ret=%d copied=%d trunc=%d dst=\"%s\"  (exp 0,4,1,\"Firm\")\n",
           r, copied, truncated, buf);

    r = safe_copy_label("", buf, 3, true, &copied, &truncated);
    printf("TC4: ret=%d copied=%d trunc=%d dst=\"%s\"  (exp 0,0,0,\"\")\n",
           r, copied, truncated, buf);

    r = safe_copy_label("ABC", buf, 0, false, &copied, &truncated);
    printf("TC5: ret=%d copied=%d trunc=%d           (exp 0,0,1)\n",
           r, copied, truncated);

    r = safe_copy_label(NULL, buf, 5, false, &copied, &truncated);
    printf("TC6: ret=%d                               (exp -1)\n", r);

    r = safe_copy_label("Hi", buf, -1, false, &copied, &truncated);
    printf("TC7: ret=%d                               (exp -1)\n", r);

    return 0;
}