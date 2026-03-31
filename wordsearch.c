#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum Direction {
    DIR_NONE = 0,
    DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW
};

/* direction vectors indexed by enum (1..8) */
static const int DR[9] = {0, -1,-1, 0, 1, 1, 1, 0,-1};
static const int DC[9] = {0,  0, 1, 1, 1, 0,-1,-1,-1};

static char to_upper(char c)
{
    if (c >= 'a' && c <= 'z') return (char)(c - ('a' - 'A'));
    return c;
}

/* context passed through DFS */
typedef struct {
    const char *grid;
    int         rows, cols;
    const char *word;
    int         word_len;
    bool        case_insensitive;
    bool        allow_diagonal;
    bool        allow_wildcard;
    char        obstacle;
    int         dir_count;
    int         count;
    int         first_row, first_col;
    enum Direction first_dir;
    char       *visited;
} Ctx;

static bool char_match(Ctx *ctx, char wc, char gc)
{
    if (gc == ctx->obstacle) return false;
    if (ctx->allow_wildcard && wc == '?') return true;
    if (ctx->case_insensitive)
        return to_upper(wc) == to_upper(gc);
    return wc == gc;
}

static void dfs(Ctx *ctx, int r, int c, int pos, int start_r, int start_c,
                enum Direction start_dir)
{
    ctx->visited[r * ctx->cols + c] = 1;

    if (pos == ctx->word_len - 1) {
        ctx->count++;
        if (ctx->first_row == -1) {
            ctx->first_row  = start_r;
            ctx->first_col  = start_c;
            ctx->first_dir  = start_dir;
        }
        ctx->visited[r * ctx->cols + c] = 0;
        return;
    }

    for (int d = 1; d <= ctx->dir_count; d++) {
        int nr = r + DR[d];
        int nc = c + DC[d];
        if (nr < 0 || nr >= ctx->rows || nc < 0 || nc >= ctx->cols) continue;
        if (ctx->visited[nr * ctx->cols + nc]) continue;
        char gc = ctx->grid[nr * ctx->cols + nc];
        if (!char_match(ctx, ctx->word[pos + 1], gc)) continue;

        enum Direction sd = (pos == 0) ? (enum Direction)d : start_dir;
        dfs(ctx, nr, nc, pos + 1, start_r, start_c, sd);
    }

    ctx->visited[r * ctx->cols + c] = 0;
}

int count_word_in_grid(const char *grid, int rows, int cols,
                       const char *word,
                       bool case_insensitive,
                       bool allow_diagonal,
                       bool allow_wildcard_qmark,
                       char obstacle_char,
                       int *out_count,
                       int *out_first_row, int *out_first_col,
                       enum Direction *out_first_dir)
{
    /* 1. Validate */
    if (grid == NULL || word == NULL)                    return -1;
    if (out_count == NULL || out_first_row == NULL ||
        out_first_col == NULL || out_first_dir == NULL)  return -1;
    if (rows <= 0 || cols <= 0)                          return -1;
    if (word[0] == '\0')                                 return -1;

    /* 2. Word length */
    int word_len = 0;
    while (word[word_len]) word_len++;

    /* 3. Visited array (VLA) */
    int cells = rows * cols;
    char visited[cells];
    for (int i = 0; i < cells; i++) visited[i] = 0;

    /* 4. Build context */
    Ctx ctx;
    ctx.grid            = grid;
    ctx.rows            = rows;
    ctx.cols            = cols;
    ctx.word            = word;
    ctx.word_len        = word_len;
    ctx.case_insensitive= case_insensitive;
    ctx.allow_diagonal  = allow_diagonal;
    ctx.allow_wildcard  = allow_wildcard_qmark;
    ctx.obstacle        = obstacle_char;
    ctx.dir_count       = allow_diagonal ? 8 : 4;
    ctx.count           = 0;
    ctx.first_row       = -1;
    ctx.first_col       = -1;
    ctx.first_dir       = DIR_NONE;
    ctx.visited         = visited;

    /* 5. Search */
    if (word_len <= cells) {
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                char gc = grid[r * cols + c];
                if (gc == obstacle_char) continue;
                if (!char_match(&ctx, word[0], gc)) continue;

                enum Direction sd = (word_len == 1) ? DIR_NONE : DIR_NONE;
                dfs(&ctx, r, c, 0, r, c, sd);
            }
        }
    }

    /* 6. Write outputs */
    *out_count      = ctx.count;
    *out_first_row  = ctx.first_row;
    *out_first_col  = ctx.first_col;
    *out_first_dir  = ctx.first_dir;
    return 0;
}

/* ── helpers ─────────────────────────────────────────────────────── */
static const char *dir_name(enum Direction d)
{
    switch (d) {
        case DIR_NONE: return "NONE";
        case DIR_N:    return "N";
        case DIR_NE:   return "NE";
        case DIR_E:    return "E";
        case DIR_SE:   return "SE";
        case DIR_S:    return "S";
        case DIR_SW:   return "SW";
        case DIR_W:    return "W";
        case DIR_NW:   return "NW";
        default:       return "?";
    }
}

int main(void)
{
    int count, fr, fc, r;
    enum Direction fd;

    /* TC1 */
    const char g1[] = "CATSA#ATTACT";  /* 3×4 row-major */
    r = count_word_in_grid(g1, 3, 4, "CAT",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC1: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,2,(0,0),E)\n", r,count,fr,fc,dir_name(fd));

    /* TC2 */
    r = count_word_in_grid(g1, 3, 4, "C?T",
                           false, false, true, '#',
                           &count, &fr, &fc, &fd);
    printf("TC2: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,≥1,(0,0))\n", r,count,fr,fc,dir_name(fd));

    /* TC3 */
    const char g3[] = "aBcd#EfGh";  /* 3×3 */
    r = count_word_in_grid(g3, 3, 3, "BEG",
                           true, true, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC3: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,1,(0,1),SE)\n", r,count,fr,fc,dir_name(fd));

    /* TC4 */
    const char g4[] = "ABCDEF";  /* 2×3 */
    r = count_word_in_grid(g4, 2, 3, "FED",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC4: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,1,(1,2),W)\n", r,count,fr,fc,dir_name(fd));

    /* TC5 */
    const char g5[] = "AXAX#XAXA";  /* 3×3 */
    r = count_word_in_grid(g5, 3, 3, "AXA",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC5: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,4,(0,0),E)\n", r,count,fr,fc,dir_name(fd));

    /* TC6 */
    const char g6[] = "CATA#TTAC";  /* 3×3 */
    r = count_word_in_grid(g6, 3, 3, "CAT",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC6: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,2,(0,0),E)\n", r,count,fr,fc,dir_name(fd));

    /* TC7: word longer than grid */
    const char g7[] = "ABCD";  /* 2×2 */
    r = count_word_in_grid(g7, 2, 2, "ABCDE",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC7: ret=%d count=%d first=(%d,%d) dir=%s\n"
           "     (exp 0,0,(-1,-1),NONE)\n", r,count,fr,fc,dir_name(fd));

    /* TC8: invalid */
    r = count_word_in_grid(NULL, 3, 3, "CAT",
                           false, false, false, '#',
                           &count, &fr, &fc, &fd);
    printf("TC8: ret=%d (exp -1)\n", r);

    return 0;
}