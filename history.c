#include "history.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

static int read_one_line_history(FILE *history_fd, history_t *history);
static void add_line_to_history(history_t *history, history_line_t *line_to_add);
static void print_history(history_t *history);
static void destroy_line(history_line_t *line);

static void destroy_line(history_line_t *line)
{
    free(line->line);
    free(line);

}
static void print_history(history_t *history)
{
    unsigned int i;
    for (i = 0; i < history->number_lines; i++)
        printf(" %d : %s", i, history->history_lines[i]->line);

}
static void add_line_to_history(history_t *history, history_line_t *line_to_add){
    static history_line_t **history_ptr = NULL;

    if (history_ptr == NULL)
        history_ptr = history->history_lines;
    *history_ptr++ = line_to_add;
}
int read_one_line_history(FILE *history_fd, history_t *history)
{
    history_line_t *current_line;
    size_t line_max_length;

    current_line = (history_line_t*)malloc(sizeof(history_t));
    if (current_line == NULL)
        return -1;
    
   current_line->line = NULL; 
    current_line->line_length = getline(&current_line->line, &line_max_length, history_fd);
    if (current_line->line_length == -1){
        free(current_line->line);
        free(current_line);
        return -1;
    }

    add_line_to_history(history, current_line);

    return 0;

}
history_t *load_history(void)
{
    FILE *history_fd;
    int c;
    history_t *temp_history;

    temp_history = (history_t*)malloc(sizeof(history_t));
    if (temp_history == NULL)
        return NULL;

    temp_history->history_lines = NULL;
    temp_history->number_lines = 0;
    
    temp_history->history_lines = (history_line_t**)malloc(sizeof(history_line_t*)*MAX_HISTORY_LINES);
    if (temp_history->history_lines == NULL)
        return NULL;
    
    if ((history_fd = fopen("test.txt", "r")) == NULL){
        fprintf(stderr, "No history found !");
        return NULL;
    }
    while (temp_history->number_lines < MAX_HISTORY_LINES && read_one_line_history(history_fd, temp_history) != -1)
        temp_history->history_lines++;
    
    temp_history->history_lines = (history_line_t**)realloc(temp_history->history_lines, sizeof(history_line_t*)*temp_history->number_lines);
    if (temp_history->history_lines == NULL)
        return NULL;

    fclose(history_fd);
    return temp_history;
}

void destroy_history(history_t *history)
{
    int i;
    for (i = 0; i < history->number_lines; i++){
        destroy_line(history->history_lines[i]);
    }
    free(history->history_lines);
    free(history);
}
