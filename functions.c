/*header file inclusion*/
#include "header.h"

/*global variable declaration*/
extern bck_process *head;
extern int pid_run;
int flag_stp = 0;
extern int bg_flag;
int kill_flag = 0;
extern int ret;
extern char prompt[100];
/*--------------------------------------------------------------------------------------------------------------------------*/
/*change directory function block*/
void cd_fun(char *buff)
{
	char *ptr;
	ptr = strchr(buff, ' ');
	++ptr;

	if (chdir(ptr) == -1)
	{
		printf("\nbash : buff : NO such file or directory\n\n");
	}

}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*signal handler for SIGINIT*/
void signal_handler_int(int signum)
{

	if (pid_run == 0)
	{

		printf("\n\n");
		printf("%s : ", prompt);
		fflush(stdout);
		return;
	}

	else
	{
		kill_flag = 1;
		kill(pid_run, SIGKILL);
		pid_run = 0;
	}
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*signal handler for SIGTSTP*/
void signal_handler_stop(int signum)
{
	flag_stp = 1;
	ret = 1;
}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*signal handler function block*/
void signal_handler_child(int signum, siginfo_t *ptr, void *context)
{
	if (kill_flag == 0)
	{
		if (!bg_flag)
		{
			if (!flag_stp)
			{
				sh_delete_node(&head, ptr->si_pid);
			}

			else
			{
				if (flag_stp)
				{
					kill(ptr->si_pid, SIGSTOP);
					pid_run = 0;
				}

			}

		}

		bg_flag = 0;
	}

	kill_flag = 0;
}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*create node function block*/
bck_process *sh_create_node(int pid, char *buff, int status)
{
	int idx;

	/*allocate memory*/
	bck_process *new = malloc(sizeof (bck_process));

	/*assign data*/	
	new->pid = pid;
	new->status = status;

	for (idx = 0; buff[idx] != '\0'; idx++)
	{
		new->name[idx] = buff[idx];
	}
	new->name[idx] = '\0';

	new->prev = NULL;
	new->next = NULL;

	return new;
}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*print list function block*/
void sh_print_list(bck_process *head)
{
	bck_process *ptr = head;

	int idx = 1;

	if (head == NULL)
	{
		;
	}

	else
	{
		printf("\n\n");
		while (ptr != NULL)
		{
			printf("[%d]", idx);

			if (ptr->status == 1)
				printf("Running\t\t");

			else
				printf("Stopped\t\t");

			printf("%s\n", ptr->name);
			++idx;
			ptr = ptr->next;
		}
	}
}
/*--------------------------------------------------------------------------------------------------------------------------*/
/*insert last function block*/
void sh_insert_last(bck_process **head, int pid, char *buff, int status)
{

	/*local variable declaration*/
	bck_process *ptr = *head, *new;

	/*if no list exists, create a node*/
	if (*head == NULL)
	{
		*head = sh_create_node(pid, buff, status);
	}

	else
	{
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}

		new = sh_create_node(pid, buff, status);
		ptr->next = new;
		new->prev = ptr;
	}

}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*delete node function block*/
void sh_delete_node(bck_process **head, int pid)
{
	/*local variable declaration*/
	bck_process *ptr = *head;

	/*check if list exists*/
	if (*head == NULL)
	{
		;
	}

	else
	{

		/*check for val*/
		while (ptr->next != NULL && ptr->pid != pid)
		{
			ptr = ptr->next;
		}

		/*if val not found*/
		if (ptr->next == NULL && ptr->pid != pid)
		{
			//	printf("\nvalue not found\n");
		}

		else if (ptr == *head && ptr->pid == pid && ptr->next == NULL)
		{
		free(ptr);
ptr = NULL;
			*head = NULL;

		}
		/*if val found in first node*/
		else if (ptr == *head && ptr->pid == pid)
		{
			ptr->next->prev = NULL;
			*head = ptr->next;
		}

		/*if val found in middle node*/
		else if (ptr->next != NULL && ptr->pid == pid)
		{
			ptr->next->prev = ptr->prev;
			ptr->prev->next = ptr->next;
		}

		/*if value found in last node*/
		else
		{
			ptr->prev->next = NULL;
		}
if (ptr != NULL)
		free(ptr);
	}
}
/*--------------------------------------------------------------------------------------------------------------------------*/



int prase_split(char *buff, char ***argu, int *var)
{
	int idx = 0, pipe_count = 0, jdx = 0, kdx = 0, ldx = 1,  flag_space = 0, flag_pipe = 0;

	for (idx = 0; buff[idx] != '\0'; idx++)
	{
		if (buff[idx] == '|')
		{
			++pipe_count;

			if (flag_space == 1)
			{
				*argu = realloc(*argu, (jdx + 1) * sizeof (char *));
				(*argu)[jdx] = NULL;
				++jdx;
				var[ldx] = jdx;
				//			printf("\nvar[ldx] = %d, ldx = %d\n", var[ldx], ldx);

				++ldx;
			}

			if (flag_space == 1 || flag_pipe == 1)
				continue;

			flag_pipe = 1;
			(*argu)[jdx] = realloc((*argu)[jdx], kdx + 1);
			(*argu)[jdx][kdx] = '\0';
			//	printf("\n%s\n", (*argu)[jdx]);
			kdx = 0;
			++jdx;


			*argu = realloc(*argu, (jdx + 1) * sizeof (char *));
			(*argu)[jdx] = NULL;
			++jdx;
			var[ldx] = jdx;
			++ldx;

		}

		else if (buff[idx] == ' ')
		{
			if (flag_space == 1 || flag_pipe == 1)
				continue;

			flag_space = 1;
			(*argu)[jdx] = realloc((*argu)[jdx], kdx + 1);
			(*argu)[jdx][kdx] = '\0';
			//printf("\n%s\n", (*argu)[jdx]);
			kdx = 0;
			++jdx;
		}

		else
		{

			if (idx == 0 || flag_pipe == 1 || flag_space == 1)
			{
				if (idx == 0)
				{
					*argu = malloc(sizeof (char *));
				}

				if (kdx == 0 && idx != 0)
				{
					*argu = realloc(*argu, (jdx + 1) * sizeof (char *));
				}

				(*argu)[jdx] = malloc(sizeof (char));
				(*argu)[jdx][kdx] = buff[idx];
				++kdx;
			}

			else
			{
				(*argu)[jdx] = realloc((*argu)[jdx], kdx + 1);
				(*argu)[jdx][kdx] = buff[idx];
				++kdx;
			}

			if (flag_space == 1)
				flag_space = 0;

			if (flag_pipe == 1)
				flag_pipe = 0;
		}
	}

	if (flag_space == 0)
		(*argu)[jdx][kdx] = '\0';

	(*argu) [jdx + 1] = '\0';
	return pipe_count;
}


