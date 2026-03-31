#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

int tokenize_in_place(char *s, char delim,
                      char **out_tokens, int max_tokens,
                      int *out_count, bool *out_truncated)
{
    if (s == NULL || out_tokens == NULL ||
        out_count == NULL || out_truncated == NULL) return -1;
    if (max_tokens < 0)                             return -1;

    char *read      = s;
    char *write     = s;
    int   count     = 0;
    bool  truncated = false;

    while (1) {
        /* skip leading spaces */
        while (*read == ' ') read++;

        char *token_start = write;

        /* copy until delimiter or end */
        while (*read != delim && *read != '\0')
            *write++ = *read++;

        /* trim trailing spaces */
        char *p = write - 1;
        while (p >= token_start && *p == ' ') p--;

        /* terminate */
        write = p + 1;
        *write = '\0';

        /* store if non-empty */
        if (write > token_start) {
            if (count < max_tokens)
                out_tokens[count++] = token_start;
            else
                truncated = true;
        }

        if (*read == '\0') break;

        /* advance past delimiter; move write forward */
        read++;
        write++;
    }

    *out_count     = count;
    *out_truncated = truncated;
    return 0;
}

int main(void)
{
    char   *toks[8];
    int     count, r;
    bool    trunc;

    /* TC1 */
    char s1[] = " alpha, beta , , gamma ";
    r = tokenize_in_place(s1, ',', toks, 5, &count, &trunc);
    printf("TC1: ret=%d count=%d trunc=%d\n     (exp 0,3,false)\n", r, count, trunc);
    for (int i = 0; i < count; i++) printf("     tok[%d]=\"%s\"\n", i, toks[i]);

    /* TC2 */
    char s2[] = "a,b,c";
    r = tokenize_in_place(s2, ',', toks, 2, &count, &trunc);
    printf("TC2: ret=%d count=%d trunc=%d\n     (exp 0,2,true)\n", r, count, trunc);
    for (int i = 0; i < count; i++) printf("     tok[%d]=\"%s\"\n", i, toks[i]);

    /* TC3 */
    char s3[] = " , , ";
    r = tokenize_in_place(s3, ',', toks, 4, &count, &trunc);
    printf("TC3: ret=%d count=%d trunc=%d\n     (exp 0,0,false)\n", r, count, trunc);

    /* TC4 */
    char s4[] = "one";
    r = tokenize_in_place(s4, ';', toks, 3, &count, &trunc);
    printf("TC4: ret=%d count=%d trunc=%d\n     (exp 0,1,false)\n", r, count, trunc);
    for (int i = 0; i < count; i++) printf("     tok[%d]=\"%s\"\n", i, toks[i]);

    /* TC5 */
    char s5[] = "";
    r = tokenize_in_place(s5, ',', toks, 3, &count, &trunc);
    printf("TC5: ret=%d count=%d trunc=%d\n     (exp 0,0,false)\n", r, count, trunc);

    /* TC6 */
    r = tokenize_in_place(NULL, ',', toks, 3, &count, &trunc);
    printf("TC6: ret=%d (exp -1)\n", r);

    /* TC7 */
    char s7[] = "x,y";
    r = tokenize_in_place(s7, ',', toks, -1, &count, &trunc);
    printf("TC7: ret=%d (exp -1)\n", r);

    return 0;
}