#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<unistd.h>
#include<limits.h>
#include<sys/types.h>
void remove_newline(char *s)
{
	while(*s)
	{
		if(*s=='\n')
		{
			*s='\0';
			break;
		}
		s++;
	}
}
int str_cmp(char *s,char *d)
{
	while(*s&&*d)
	{
		if(*s!=*d)
		{
			return *s-*d;
		}
		s++;
		d++;
	}
	return *s-*d;
}
int main()
{
	char str[1000]={0};//taking input from the user
	char *cmd,*arg;
	char cwd[PATH_MAX];
	char prev_dir[PATH_MAX]="";
	pid_t q;
	while(1)
	{
		getcwd(cwd,sizeof(cwd));
		printf("myshell:$ "); //for printing myshell:$ on the terminal screen
		fgets(str,sizeof(str),stdin); //For taking input from the user
		remove_newline(str);
		cmd=strtok(str," ");
		arg=strtok(NULL," ");
		if(cmd==NULL)
			continue;
		if(str_cmp(cmd,"ls")==0) //If user enters ls it will execute execl
		{
			q=fork();
			if(q==0)
			{
				execlp("ls","ls","--color=auto",NULL);
				perror("exec failed");
			}
			else 
			{
				wait(NULL);
			}
		}
		else if(str_cmp(cmd,"mkdir")==0)
		{
			if(arg==NULL)
			{
				printf("mkdir: missing operand\n");
			}
			else 
			{
				if(mkdir(arg,0777)!=0)
				{
					perror("mkdir failed");
				}
				else
				{
					printf("Directory '%s' created\n",arg);
				}
			}
		}
		else if(str_cmp(cmd,"cd")==0)
		{
			char *target=NULL;
			if(arg==NULL || str_cmp(arg,"~")==0)
			{
				target=getenv("HOME");
			}
			else if(str_cmp(arg,"-")==0)
			{
				if(strlen(prev_dir)==0)
				{
					printf("cd: OLDPWD not set\n");
					continue;
				}
				target=prev_dir;
				printf("%s\n",target);
			}
			else
			{
				target=arg;
			}
			if(target)
			{
				char temp[PATH_MAX];
				strcpy(temp,cwd);
				if(chdir(target)!=0)
				{
					perror("cd error");
				}
				else
				{
					strcpy(prev_dir,temp);
				}
			}
		}
		else if(str_cmp(cmd,"rm")==0)
		{
			if(arg==NULL)
				continue;
			if(fork()==0)
			{
				execlp("rm","rm",arg,NULL);
				perror("exec failed");
			}
			else
			{
				wait(NULL);
			}
		}
		else if(str_cmp(cmd,"vi")==0)
		{
			if(arg==NULL)
				continue;
			q=fork();
			if(q==0)
			{
				execlp("vi","vi",arg,NULL);
				perror("exec failed");
			}
			else
			{
				wait(NULL);
			}
		}
		else if(str_cmp(cmd,"pwd")==0)
		{
			printf("%s\n",cwd);
		}
		else if(str_cmp(cmd,"clear")==0)
		{
			system("clear");
		}
		else if(str_cmp(cmd,"exit")==0)
		{
			break;
		}
		else
		{
			printf("Command not found: %s\n",str);
		}
	}
	return 0;
}
