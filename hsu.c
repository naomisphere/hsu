#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>

// macOS service accounts have /usr/bin/false as login shell.
// work around that.
int validShells(const char *p) {
    const char *INVALID_SHELLS[] = {
        "/usr/bin/false",
        "/sbin/nologin",
        NULL
    };
    
    for (int i = 0; INVALID_SHELLS[i] != NULL; i++) {
        if (strcmp(p, INVALID_SHELLS[i]) == 0) {
            return 0;
        }
    }
    
    if (access(p, X_OK) != 0) {
        return 0;
    }
    
    return 1;
}

const char *TW_SHELL() {
    const char *C_SHELLS[] = {"/bin/zsh", "/bin/sh", "/bin/bash", "/bin/csh", NULL};
    for (int i = 0; C_SHELLS[i] != NULL; i++) {
        if (access(C_SHELLS[i], X_OK) == 0) {
            return C_SHELLS[i];
        }
    }
    return "/bin/sh";
}

void uexec(struct passwd *pw, char **r_args, int arg_count) {
    const char *PSHELL = pw->pw_shell;
    if (!PSHELL || strlen(PSHELL) == 0 || !validShells(PSHELL)) {
        PSHELL = TW_SHELL();
    }

    if (pw->pw_dir && strlen(pw->pw_dir) > 0) {
        setenv("HOME", pw->pw_dir, 1);
    } else {
        setenv("HOME", "/var/root", 1);
    }
    
    setenv("SHELL", PSHELL, 1);
    setenv("USER", pw->pw_name, 1);
    setenv("LOGNAME", pw->pw_name, 1);

    if (setgid(pw->pw_gid) != 0 || setuid(pw->pw_uid) != 0) {
        perror("setuid/gid");
        exit(1);
    }

    if (arg_count > 0) {
        char **exec_args = malloc((arg_count + 3) * sizeof(char*));
        exec_args[0] = (char *)PSHELL;
        exec_args[1] = "-c";
        
        size_t cmd_len = 0;
        for (int i = 0; i < arg_count; i++) {
            cmd_len += strlen(r_args[i]) + 1;
        }
        char *CMD = malloc(cmd_len);
        CMD[0] = '\0';
        for (int i = 0; i < arg_count; i++) {
            if (i > 0) strcat(CMD, " ");
            strcat(CMD, r_args[i]);
        }
        
        exec_args[2] = CMD;
        exec_args[3] = NULL;

        execvp(PSHELL, exec_args);
        perror("execvp");
        free(exec_args);
        free(CMD);
        exit(1);
    } else {
        execl(PSHELL, PSHELL, NULL);
        perror("execl");
        exit(1);
    }
}

void usage(char *RANAS_BINARY) {
    fprintf(stderr, "Usage: %s [USER] [-c command [args...]]\n", RANAS_BINARY);
    exit(1);
}

int main(int argc, char *argv[]) {
    char *sduser = "root";
    char **r_args = NULL;
    int CMD_ARG_COUNT = 0;
    int HSU_CMD_MODE = 0;
    int SU_USER_GIVEN = 0;

    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-c") == 0) {
            HSU_CMD_MODE = 1;
            if (i + 1 < argc) {
                r_args = argv + i + 1;
                CMD_ARG_COUNT = argc - i - 1;
                break;
            } else {
                fprintf(stderr, "hsu -c: missing arguments\n");
                usage(argv[0]);
            }
        } else if (argv[i][0] != '-') {
            if (SU_USER_GIVEN) {
                fprintf(stderr, "hsu: too many arguments\n");
                usage(argv[0]);
            }
            sduser = argv[i];
            SU_USER_GIVEN = 1;
        } else {
            fprintf(stderr, "hsu: unknown option %s\n", argv[i]);
            usage(argv[0]);
        }
        i++;
    }

    struct passwd *pw = getpwnam(sduser);
    if (!pw) {
        fprintf(stderr, "hsu: bad user: %s\n", sduser);
        exit(1);
    }

    if (HSU_CMD_MODE) {
        uexec(pw, r_args, CMD_ARG_COUNT);
    } else {
        char *NO_ARG[] = {NULL};
        uexec(pw, NO_ARG, 0);
    }

    return 0;
}