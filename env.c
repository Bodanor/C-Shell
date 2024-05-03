#include "env.h"

env_t *init_env(void)
{
    struct passwd *pwd;
    env_t *temp;

    temp = (env_t*)malloc(sizeof(env_t));
    if (temp == NULL){
        fprintf(stderr, "FATAL : malloc error\n");
        return NULL;
    }
    
    /* Get the user uid */
    temp->permissions = getuid();
    
    pwd = getpwuid(temp->permissions);
    if (pwd == NULL) {
        fprintf(stderr, "FATAL : %s\n", strerror(errno));
        free(temp);
        return NULL;
    }

    /* Copy the user name */
    temp->user = (char*)malloc(sizeof(char)*strlen(pwd->pw_name) + 1);
    if (temp->user == NULL) {
        fprintf(stderr, "FATAL : malloc error\n");
        free(temp);
        free(pwd);
        return NULL;
    }
    strcpy(temp->user, pwd->pw_name);

    /* Copy the home directory path */
    temp->home_dir = (char*)malloc(sizeof(char)*strlen(pwd->pw_dir) + 1);
    if (temp->home_dir == NULL) {
        fprintf(stderr, "FATAL : malloc error\n");
        free(temp->user);
        free(temp);
        free(pwd);
        return NULL;
    }
    strcpy(temp->home_dir, pwd->pw_dir);

    /* Copy the current working directory */
    temp->curr_wd = getcwd(NULL, 0);
    if (temp->curr_wd == NULL) {
        fprintf(stderr, "FATAL : Failed to retrieve current working directory !\n");
        free(temp->user);
        free(temp->home_dir);
        free(temp);
        free(pwd);
        return NULL;
    }
    
    /* Copy the hostname */
    temp->hostname = (char*)malloc(sizeof(char)*HOST_NAME_MAX + 1);
    if (temp->hostname == NULL) {
        fprintf(stderr, "FATAL : malloc error\n");
        free(temp->user);
        free(temp->home_dir);
        free(temp->curr_wd);
        free(temp);
        free(pwd);
        return NULL;
    }
    if (gethostname(temp->hostname, HOST_NAME_MAX + 1) != 0) {
        fprintf(stderr, "FATAL : Failed to retrieve the hostname !");
        free(temp->user);
        free(temp->home_dir);
        free(temp->curr_wd);
        free(temp->hostname);
        free(temp);
        free(pwd);
        return NULL;
    }

    return temp;
}
void destroy_env(env_t **env)
{
    if (*env != NULL) {
        if ((*env)->curr_wd != NULL) {
            free((*env)->curr_wd);
            (*env)->curr_wd = NULL;
        }

        if ((*env)->home_dir != NULL) {
            free((*env)->home_dir);
            (*env)->home_dir = NULL;
        }

        if ((*env)->hostname != NULL) {
            free((*env)->hostname);
            (*env)->hostname = NULL;
        }
        if ((*env)->user != NULL) {
            free((*env)->user);
            (*env)->user = NULL;
        }
        free(*env);
    }


}
