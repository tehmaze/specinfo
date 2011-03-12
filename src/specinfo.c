#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>

#include <rpmcli.h>
#include <rpmdb.h>
#include <rpmbuild.h>

#include "config.h"
#include "specinfo.h"

void usage(char **argv) {
    fprintf(stderr, "%s [<options>] <spec file>\n\n", argv[0]);
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, "\t-h           this help\n");
    fprintf(stderr, "\t-m <dir>     parse macros from the given directory\n");
    fprintf(stderr, "\n");
}

void loadMacros(char *macrodir) {
    DIR *dp;
    size_t size = 512;
    char *oldcwd = (char *)malloc(size * sizeof(char));
    struct dirent *ep;
    struct stat fileinfo;

    dp = opendir(macrodir);
    if (dp == NULL) {
        fprintf(stderr, "Error: can not open \"%s\"\n", macrodir);
        exit(1);
    }

    if (getcwd(oldcwd, size) == NULL)
        return;
    if (chdir(macrodir) != 0)
        return;
    while ((ep = readdir(dp))) {
        if (!stat(ep->d_name, &fileinfo)) {
            if (S_ISREG(fileinfo.st_mode)) {
                rpmLoadMacroFile(rpmGlobalMacroContext, ep->d_name);
            }
        }
    }
    if (chdir(oldcwd) != 0)
        return;
}

int main(int argc, char **argv) {
    rpmts ts;
    int macrodirc = 0;
    char *specfile = NULL;
    struct stat specstat;
    Spec spec;
    char *buildRoot = NULL;
    int recursing = 0;
    char *passPhrase = "";
    char *cookie = NULL;
    int anyarch = 1;
    int force = 1;
    int debug = 0;
    int c;

    if ((ts = rpmtsCreate()) == NULL) {
        fprintf(stderr, "rpmtsCreate fail\n");
        return 1;
    }

    opterr = 0;
    while ((c = getopt(argc, argv, "dhm:")) != -1) {
        switch (c) {
            case 'd':
                debug = 1;
                break;
            case 'm':
                loadMacros(optarg);
                ++macrodirc;
                break;
            case 'h':
            default:
                usage(argv);
                return 1;
        }
    }

    // Make sure we load at least the macros from /etc/rpm
    if (macrodirc == 0) {
        loadMacros("/etc/rpm");
    }

    // Check input spec
    if (optind < 1) {
        fprintf(stderr, "Error: invalid number of arguments\n");
        usage(argv);
        return 1;
    }
    specfile = argv[optind];

    if (specfile == NULL) {
        fprintf(stderr, "Error: invalid spec file\n");
        usage(argv);
        return 1;
    }

    if (stat(specfile, &specstat) != 0) {
        fprintf(stderr, "Error: could not open spec file \"%s\"\n", specfile);
        return 1;
    }

    // Read spec
    if (parseSpec(ts, specfile, "/", buildRoot, recursing, passPhrase,
        cookie, anyarch, force) != 0) {
        fprintf(stderr, "parseSpec fail\n");
        return 1;
    }

    // Parse spec
#ifdef _RPM_4_4_COMPAT
    if ((spec = rpmtsSpec(ts)) == NULL) {
#else
    if ((spec = rpmtsSpec(ts, NULL)) == NULL) {
#endif
        fprintf(stderr, "rpmtsSpec fail\n");
        return 1;
    }

    /* Assemble source header from parsed components */
    initSourceHeader(spec);

    // Output spec information
    printf("file:       %s\n", basename(specfile));
    printf("name:       %s\n", rpmExpand("%{name}", NULL));
    printf("version:    %s\n", rpmExpand("%{version}", NULL));
    printf("release:    %s\n", rpmExpand("%{release}", NULL));
    printf("group:      %s\n", rpmExpand("%{group}", NULL));
    printf("license:    %s\n", rpmExpand("%{license}", NULL));
    printf("summary:    %s\n", rpmExpand("%{summary}", NULL));
    printf("requires:   %s\n", rpmExpand("%{requires}", NULL));
    printf("provides:   %s\n", rpmExpand("%{provides}", NULL));
    printf("obsoletes:  %s\n", rpmExpand("%{obsoletes}", NULL));
    //printf("conflicts:  %s\n", rpmExpand("%{conflicts}", NULL));

    // Cleanup
    ts = rpmtsFree(ts);

    return 0;
}
