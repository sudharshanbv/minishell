
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
#include <sys/wait.h>
//#include <fcntl.h>

typedef struct background_process
{

int pid, status;
char name[100];
struct background_process *next, *prev;

} bck_process;

int prase_split(char *buff, char ***argu, int *var);
bck_process *sh_create_node(int pid, char *buff, int status);
void sh_print_list(bck_process *head);
void sh_insert_last(bck_process **head, int pid, char *buff, int status);
void sh_delete_node(bck_process **head, int pid);
void signal_handler_child(int signum, siginfo_t *ptr, void *context);
void signal_handler_stop(int signum);
void signal_handler_int(int signum);
void cd_fun(char *buff);
//void signal_handler(int signum);
#endif
