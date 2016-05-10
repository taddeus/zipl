#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <alloca.h>

#define DEFAULT_WIDTH 80
#define SEPARATOR " "

#define FORN(i, n) for (int i = 0, _l = (n); i < _l; i++)

void usage(char **argv, int status) {
    printf("usage: %s [-h] [-w WIDTH=%d] [FILE]...\n", argv[0], DEFAULT_WIDTH);
    exit(status);
}

void zipl(FILE **files, int nfiles, int width) {
    int nopen = nfiles;
    char buf[width + 1];

    while (nopen > 0) {
        FORN(i, nfiles) {
            if (i)
                printf(SEPARATOR);

            if (files[i]) {
                if (!fgets(buf, width + 1, files[i])) {
                    fclose(files[i]);
                    files[i] = NULL;
                    nopen--;
                    *buf = 0;
                } else {
                    char *last = buf + strlen(buf) - 1;

                    if (*last == '\n')
                        *last = 0;
                    else
                        while (getc(files[i]) != '\n');

                    printf("%s", buf);

                    char next = fgetc(files[i]);
                    if (next == EOF) {
                        fclose(files[i]);
                        files[i] = NULL;
                        nopen--;
                    } else {
                        ungetc(next, files[i]);
                    }
                }

                FORN(j, width - strlen(buf)) putchar(' ');
            } else {
                FORN(j, width) putchar(' ');
            }
        }
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    int width = DEFAULT_WIDTH, nfiles, opt;

    while ((opt = getopt(argc, argv, "hw:")) != -1) {
        switch (opt) {
            case 'h':
                usage(argv, 0);
                break;
            case 'w':
                width = atoi(optarg);
                break;
            case '?':
                usage(argv, 1);
                break;
        }
    }
    nfiles = argc - optind;

    if (nfiles == 0) {
        FILE *f = stdin;
        zipl(&f, 1, width);
    } else {
        FILE **files = alloca(nfiles * sizeof (FILE*));

        FORN(i, nfiles) {
            char *filename = argv[optind + i];
            if (!strcmp(filename, "-")) {
                files[i] = stdin;
            } else if (!(files[i] = fopen(filename, "r"))) {
                perror(filename);
                return -1;
            }
        }

        zipl(files, nfiles, width);
    }

    return 0;
}
