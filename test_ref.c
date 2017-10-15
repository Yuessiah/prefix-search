#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tst.h"

/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024, ORDMAX = 100000, REFMAX = 2560000000 };
#define REF 0
#define CPY 1

/* timing helper function */
static double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}

#define IN_FILE "dictionary/cities.txt"
#define OUT_FILE "output.txt"

int ord, reflen;
char *refer[ORDMAX];
char *pool_request(char *s)
{
    if(reflen + strlen(s) + 1 >= REFMAX) {
        refer[++ord] = (char*)malloc(sizeof(char)*REFMAX);
        reflen = 0;
    }
    char *tmp = strdup(s), *fp = NULL;
    fp = strcpy(refer[ord] + reflen, tmp);
    reflen += strlen(s) + 1;
    if(tmp) free(tmp);

    return fp;
}

int main(int argc, char **argv)
{
    char word[WRDMAX] = "";
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL;
    int rtn = 0, idx = 0, sidx = 0;
    FILE *fp = fopen(IN_FILE, "r");
    double t1, t2, exetime[4] = {};

    ord = reflen = 0;
    refer[ord] = (char*)malloc(sizeof(char)*REFMAX);

    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        return 1;
    }

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", word)) != EOF) {
        char *p = pool_request(word);
        if (!tst_ins_del(&root, &p, INS, REF)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            return 1;
        }
        idx++;
    }
    t2 = tvgetf();
    exetime[0] += t2 - t1;

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);

    for (;;) {
        printf(
            "\nCommands:\n"
            " a  add word to the tree\n"
            " f  find word in tree\n"
            " s  search words matching prefix\n"
            " d  delete word from the tree\n"
            " e  eject, output the execution time\n"
            " q  quit, freeing all data\n\n"
            "choice: ");
        fgets(word, sizeof word, stdin);

        char *p = NULL;
        switch (*word) {
        case 'a':
            printf("enter word to add: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            p = pool_request(word);
            t1 = tvgetf();
            res = tst_ins_del(&root, &p, INS, REF);
            t2 = tvgetf();
            exetime[0] += t2 - t1;
            if (res) {
                idx++;
                printf("  %s - inserted in %.6f sec. (%d words in tree)\n",
                       (char *) res, t2 - t1, idx);
            } else
                printf("  %s - already exists in list.\n", (char *) res);
            break;
        case 'f':
            printf("find word in tree: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search(root, word);
            t2 = tvgetf();
            exetime[1] += t2 - t1;
            if (res)
                printf("  found %s in %.6f sec.\n", (char *) res, t2 - t1);
            else
                printf("  %s not found.\n", word);
            break;
        case 's':
            printf("find words matching prefix (at least 1 char): ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            t1 = tvgetf();
            res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
            t2 = tvgetf();
            exetime[2] += t2 - t1;
            if (res) {
                printf("  %s - searched prefix in %.6f sec\n\n", word, t2 - t1);
                for (int i = 0; i < sidx; i++)
                    printf("suggest[%d] : %s\n", i, sgl[i]);
            } else
                printf("  %s - not found\n", word);
            break;
        case 'd':
            printf("enter word to del: ");
            if (!fgets(word, sizeof word, stdin)) {
                fprintf(stderr, "error: insufficient input.\n");
                break;
            }
            rmcrlf(word);
            p = word;
            printf("  deleting %s\n", word);
            t1 = tvgetf();
            res = tst_ins_del(&root, &p, DEL, REF);
            t2 = tvgetf();
            exetime[3] += t2 - t1;
            if (res)
                printf("  delete failed.\n");
            else {
                printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                idx--;
            }
            break;
        case 'e':
            fp = fopen(OUT_FILE, "a");
            if (!fp) { /* prompt, open, validate file for reading */
                fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
                return 1;
            }
            fprintf(fp, "  add,    total time %.6f sec.\n", exetime[0]);
            fprintf(fp, "  find,   total time %.6f sec.\n", exetime[1]);
            fprintf(fp, "  search, total time %.6f sec.\n", exetime[2]);
            fprintf(fp, "  delete, total time %.6f sec.\n", exetime[3]);
            fclose(fp);
            break;
        case 'q':
            for(int i = 0; i <= ord; i++) if(!refer[i]) free(refer[i]);
            tst_free(root);
            return 0;
            break;
        default:
            fprintf(stderr, "error: invalid selection.\n");
            break;
        }
    }

    return 0;
}
