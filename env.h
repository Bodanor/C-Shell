#ifndef __ENV_H__
#define __ENV_H__

#include <sys/types.h>

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
