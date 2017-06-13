#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE 80
#define MAXARGS 20

void process_input(int argc, char **argv) {
  int i;
  int j;
  int count;

  if(strcmp(argv[0], ">") == 0){
    printf("ERROR: No/invalid command.\n");
    exit(0);
  }
  
  if(strcmp(argv[0], "<") == 0){
    printf("ERROR: No/invalid command.\n");
    exit(0);
  }

  if(argc == 2){
    if(strcmp(argv[1], ">") == 0){
       printf("ERROR: No redirection file specified.\n");
       exit(0);
    }
  }

  if(argc == 2){
    if(strcmp(argv[1], "<") == 0){
       printf("ERROR: No redirection file specified.\n");
       exit(0);
    }
  }

  for(j=0; j < argc; j++){
    if(strcmp(argv[j], "<")==0){
      count++;
      if(count >= 2){ 
        printf("ERROR: Cannot have two input redirects on one line.\n");
        exit(0);
      }
    }
  }

  for(i=0; i < argc; i++){
    if(strcmp(argv[i], ">")== 0){
        int fid = open(argv[i+1],O_CREAT|O_TRUNC|O_RDWR|S_IRUSR|S_IWUSR);
        dup2(fid, 1);
        close(fid);
        argv[i] = NULL;
    }else if(strcmp(argv[i], "<") ==0){
        int fid = open(argv[i+1], O_RDWR|O_CREAT|O_TRUNC|S_IRUSR|S_IWUSR);
        dup2(fid,0);
        close(fid);
        argv[i] = NULL;
    }
  }
  execvp(argv[0],argv);
}
/* ----------------------------------------------------------------- */
/*                  parse input line into argc/argv format           */
/* ----------------------------------------------------------------- */
int parseline(char *cmdline, char **argv)
{
  int count = 0;
  char *separator = " \n\t";
  argv[count] = strtok(cmdline, separator);
  while ((argv[count] != NULL) && (count+1 < MAXARGS)) {
   argv[++count] = strtok((char *) 0, separator);
  }
  return count;
}
/* ----------------------------------------------------------------- */
/*                  The main program starts here                     */
/* ----------------------------------------------------------------- */
int main(void)
{
 char cmdline[MAXLINE];
 char *argv[MAXARGS];
 char *cwd;
 char buff[MAXLINE+1];
 int argc;
 int status;
 pid_t pid;

 while (1) {
  printf(" csc60mshell > ");
  fgets(cmdline, MAXLINE, stdin);
  /* Call parseline to build argc/argv: argc/argv parameters declared above */  
  argc = parseline(cmdline, argv);
  
  /* If user types in nothing, go to top of loop. */
  if(argc == 0) continue;

  /* If user enters "cd dir", change to "dir" directory.    */ 
  /* If directory does not exit, print error.               */
  /* Else if user types only cd, change to home directory.  */ 
  if(strcmp(argv[0],"cd") == 0){
    if(argc == 1){
      char *temp = getenv("HOME");
      chdir(temp);
    }else if(argc == 2){
      if(chdir(argv[1]) == 0){
      
      }else{
        perror(argv[1]);
      }
    }
  /* If user types in "pwd", print the current working directory.*/
  }else if(argc == 1 && strcmp(argv[0],"pwd") == 0){
    cwd = getcwd( buff, 256 );
    printf("%s\n", cwd );
  
  /* If user types in "exit", exit the program.  */
  }else if(argc == 1 && strcmp(argv[0],"exit") == 0){
    exit(0);
    break;
  
  /* If user doesn't type in any built-in command, fork off a process. */
  }else{ 
    pid = fork();
    if (pid == -1) 
      perror("Shell Program fork error");
    else if (pid == 0) 
      /* I am child process. I will execute the command, call: execvp */
      process_input(argc, argv);
    else 
      /* I am parent process */
      if (wait(&status) == -1)
        perror("Shell Program error");
      else
        printf("Child returned status: %d\n",status);
    continue;
  }
 }
}
