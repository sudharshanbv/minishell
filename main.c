


/*header file inclusion*/
#include "header.h"

bck_process *head = NULL;
int bg_flag = 0;
int pid_run = 0;
int ret;
char buff[1024], prompt[100] = "Sudharshan";

/*main function block*/
int main(int argc, char *argv, char *envp)
{


	/*local variable declaration*/
	int status = 0, flag = 0, count = 0, pipe_count, break_flag = 1, bck_flag = 0, wait_flag = WSTOPPED, save;
	int idx = 0, var[50], jdx = 1, pid = 0, kdx = 0, id, exit_flag = 0, ldx = 0;;
	extern int flag_stp;
	bck_process *d_ptr;
	char  **argu = NULL, *ptr = NULL;
	char buff[1024], prompt[100] = "Sudharshan";


	struct sigaction act;
	memset(&act, 0, sizeof (act));

	sigset_t myset;
	act.sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER | SA_NOCLDWAIT;
	act.sa_sigaction = signal_handler_child;

	sigaddset(&myset, SIGCHLD);

	act.sa_mask = myset;

	sigaction(SIGCHLD, &act, NULL);
	signal(SIGTSTP, signal_handler_stop);
	signal(SIGINT, signal_handler_int);

	while (1)
	{

		save  = dup2(0, 70);

		/*initializing to zero*/
		memset(var, 0, sizeof (var));
		memset(buff, 0, sizeof (buff));

		var[0] = 0;

		/*display the prompt and scan the user input*/
		printf("%s : ", prompt);
		scanf("%[^\n]", buff);
		__fpurge(stdin);

		/*resetting the variables to their initial values*/
		pipe_count = 0;
		flag = 0;
		break_flag = 1;
		bck_flag = 0;
		d_ptr = NULL;
		ret = 0;
		exit_flag = 0;
		wait_flag = WSTOPPED;

		/*if background process*/
		if (ptr = strchr(buff, '&'))
		{

			/*set the flag*/
			bck_flag = 1;
			*ptr = '\0';
			--ptr;

			if (*ptr == ' ')
				*ptr = '\0';

		}

		/*if changing the prompt name*/
		else if (ptr = strstr(buff, "PS1="))
		{
			ptr = strchr(buff, '=');
			++ptr;

			for (idx = 0; *ptr != '\0'; idx++, ptr++)
			{
				prompt[idx] = *ptr;
			}
			prompt[idx] = '\0';
			continue;
		}

		else if (strcmp(buff, "echo $?") == 0)
		{
			printf("\n%d\n", status);
			continue;
		}

		else if (strcmp(buff, "echo $$") == 0)
		{
			printf("\n%d\n", getpid());
			continue;
		}

		/*if "jobs" command*/
		else if (strcmp(buff, "jobs") == 0)
		{
			sh_print_list(head);
			continue;
		}

		/*if "mkdir" command*/
		else if (strstr(buff, "mkdir "))
		{
			ptr = strchr(buff, ' ');
			++ptr;

			if (mkdir(ptr, 0777) == -1)
			{
				printf("\nmkdir : failed to create the directory\n");
			}
			continue;
		}

		/*if "cd" command*/
		else if (strstr(buff, "cd "))
		{
			cd_fun(buff);
			continue;
		}

		else if (strcmp(buff, "echo $SHELL") == 0)
		{
			;	
		}

		/*if "exit" command*/
		else if (strcmp(buff, "exit") == 0)
		{
			exit(2);
		}

		/*if "pwd" command*/
		else if (strcmp(buff, "pwd") == 0)
		{
			memset(buff, 0, sizeof (buff));
			getcwd(buff, sizeof (buff));
			printf("\n%s\n", buff);
			continue;
		}

		/*if "bg" command*/
		else if (strcmp(buff, "bg") == 0)
		{
			d_ptr = head;

			if (d_ptr == NULL)
				printf("\nbash : bg : no process in background\n");

			else
			{
				bg_flag = 1;

				while (d_ptr->next != NULL)
					d_ptr = d_ptr->next;

				while (d_ptr->status != 2)
					d_ptr = d_ptr->prev;

				if (d_ptr->status == 2)
				{
					d_ptr->status = 1;
					kill(d_ptr->pid, SIGCONT);
//return;
				}
			}

			continue;
		}

		/*if "fg" command*/
		else if (strcmp(buff, "fg") == 0)
		{
			d_ptr = head;

			if (d_ptr == NULL)
				printf("\nbash : bg : no process in background\n");

			else
			{
				bg_flag = 1;

				while (d_ptr->next != NULL)
					d_ptr = d_ptr->next;


				if (d_ptr->status == 2)
				{
					d_ptr->status = 1;
					kill(d_ptr->pid, SIGCONT);
				}
				else if (d_ptr->status == 1)
				{
					sh_delete_node(&head, d_ptr->pid);
				}    

				while (ret == 0)
				{
					ret = waitpid(d_ptr->pid, &status, WNOHANG);

				}	
			}
			continue;
		}

		/*calling the parse function to count the number of pipes*/
		pipe_count = prase_split(buff, &argu, var);
		int p[pipe_count][2];

		/*opening the pipes*/
		if (pipe_count > 0)
		{

			for (idx = 0; idx < pipe_count; idx++)
			{
				/*pipe1 open failure error handling*/
				if (pipe(p[idx]) == -1)
				{
					perror("pipe : ");
					exit(2);
				}
			}
		}

		idx = 0;

		while (break_flag)
		{
			/*creating first child*/
			switch (id = fork())
			{

				/*error handling for vfork failure*/
				case -1:
					{
						perror("vfork : ");
						exit(1);
					}
					break;

					/*child process*/
				case 0:
					{

						if (bck_flag == 1)
							printf("\n%d\n\n", getpid());	
						else
							pid_run = getpid();

						if (pipe_count > 0)
						{

							if (!flag)
							{
								/*closing the read end of pipe[idx]*/
								close(p[idx][0]);
							}

							if (var[idx + 1] != 0)
							{
								dup2(p[idx][1], 1);
							}

						}

						///execvp(argu[var[idx]], argu + var[idx]);
						execvpe(argu[var[idx]], argu + var[idx], (char *)0);

						/*incase of exec failure*/
						printf("\nERROR !!! exec failure...\n");
						exit_flag = 1;
						exit(3);
					}
					break;

					/*parent process*/
				default:
					{
						sleep(1);


						//		printf("pid in parent = %d\n", pid_run);

						if (exit_flag == 1)
							exit(3);

						if (bck_flag == 1)
						{
							sh_insert_last(&head, id, buff, 1);
						}


						if (bck_flag == 0)
							//	waitpid(id, &status, 0);
							waitpid(id, &status, wait_flag);

						if (flag_stp == 1)
						{
							sh_insert_last(&head, id, buff, 2);
						}

						flag_stp = 0;

						if (pipe_count == 0)
							break_flag = 0;

						if (flag)
						{
							break_flag = 0;
							dup2(70, 0);
							continue;
						}

						if (pipe_count > 0)
						{

							++idx;
							if (idx != 0)
							{
								/*closing the write end of pipe[idx - 1]*/
								close(p[idx - 1][1]);
								dup2(p[idx - 1][0], 0);

							}
						}

						if (var[idx + 1] == 0)
							flag = 1;

						continue;
					}	
			}
		}

	}
}
