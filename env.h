#ifndef __ENV_H__
#define __ENV_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <limits.h>
#include <errno.h>

typedef struct env_s{
    char *user;
    char *home_dir;
    char *curr_wd;
    char *hostname;
    uid_t permissions;

}env_t;

env_t *init_env(void);
void destroy_env(env_t **env);
#endif
