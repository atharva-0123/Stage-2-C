#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int count_lines_filtered(const char *path,
                         bool ignore_blank,
                         bool ignore_comment_hash,
                         int *out_count)
{
    if (path == NULL || out_count == NULL) return -1;

    FILE *f = fopen(path, "rb");
    if (f == NULL) return -1;

    int  count       = 0;
    int  in_line     = 0;   /* 1 while accumulating a logical line */
    int  only_space  = 1;   /* blank-line tracker for current line */
    int  first_nonsp = 0;   /* first non-space char of current line */
    int  c;

    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            /* end of logical line — evaluate */
            if (!in_line) {
                /* empty logical line (bare \n) */
                only_space  = 1;
                first_nonsp = 0;
                if (!ignore_blank) count++;
                continue;
            }
            in_line = 0;

            int skip = 0;
            if (ignore_blank && only_space)                        skip = 1;
            if (!skip && ignore_comment_hash && first_nonsp == '#') skip = 1;
            if (!skip) count++;

            only_space  = 1;
            first_nonsp = 0;
        } else if (c == '\r') {
            /* skip CR (handles CRLF; bare CR treated as invisible) */
        } else {
            in_line = 1;
            if (c != ' ' && c != '\t') {
                if (only_space) first_nonsp = c;   /* first non-space seen */
                only_space = 0;
            }
        }
    }

    /* handle file not ending with \n */
    if (in_line) {
        int skip = 0;
        if (ignore_blank && only_space)                        skip = 1;
        if (!skip && ignore_comment_hash && first_nonsp == '#') skip = 1;
        if (!skip) count++;
    }

    fclose(f);
    *out_count = count;
    return 0;
}

/* ── test helper ─────────────────────────────────────────────────── */
static int write_tmp(const char *name, const char *data, int len)
{
    FILE *f = fopen(name, "wb");
    if (!f) return -1;
    fwrite(data, 1, (size_t)len, f);
    fclose(f);
    return 0;
}

int main(void)
{
    int count, r;

    /* TC1: plain LF */
    write_tmp("/tmp/tc1.txt", "A\nB\nC\n", 6);
    r = count_lines_filtered("/tmp/tc1.txt", false, false, &count);
    printf("TC1: ret=%d count=%d  (exp 0,3)\n", r, count);

    /* TC2: CRLF, blank lines, comments */
    write_tmp("/tmp/tc2.txt", "A\r\n\r\n#x\r\nB\r\n", 14);
    r = count_lines_filtered("/tmp/tc2.txt", true, true, &count);
    printf("TC2: ret=%d count=%d  (exp 0,2)\n", r, count);

    /* TC3: leading spaces before # */
    write_tmp("/tmp/tc3.txt", " # comment\n data \n", 18);
    r = count_lines_filtered("/tmp/tc3.txt", false, true, &count);
    printf("TC3: ret=%d count=%d  (exp 0,1)\n", r, count);

    /* TC4: very long line (> 4 KB) */
    {
        FILE *f = fopen("/tmp/tc4.txt", "wb");
        for (int i = 0; i < 5000; i++) fputc('X', f);
        fputc('\n', f);
        for (int i = 0; i < 5000; i++) fputc('Y', f);
        fputc('\n', f);
        fclose(f);
    }
    r = count_lines_filtered("/tmp/tc4.txt", false, false, &count);
    printf("TC4: ret=%d count=%d  (exp 0,2)\n", r, count);

    /* TC5: non-existent file */
    r = count_lines_filtered("/tmp/no_such_file_xyz.txt", false, false, &count);
    printf("TC5: ret=%d          (exp -1)\n", r);

    /* TC6: NULL out_count */
    r = count_lines_filtered("/tmp/tc1.txt", false, false, NULL);
    printf("TC6: ret=%d          (exp -1)\n", r);

    return 0;
}