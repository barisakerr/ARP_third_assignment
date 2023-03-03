#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>


//Log file
int FDlog;
char LogBuffer[100];

//Get time for Log file
time_t rawtime;
struct tm *info;

char process[30], address[20], port[10];

//Write log file function
void WriteLog(char *msg){
  time(&rawtime);
  info=localtime(&rawtime);

  sprintf(LogBuffer, msg);
  sprintf(LogBuffer + strlen(LogBuffer), asctime(info));

  if(write(FDlog, LogBuffer, strlen(LogBuffer))==-1){
    close(FDlog);
    perror("Error in writing function");
    }
}

int spawn(const char * program, char * arg_list[]){

  pid_t child_pid = fork();

  if(child_pid < 0){
    perror("Error while forking...");
    WriteLog("Error while forking...");
    return 1;
  } else if(child_pid != 0){
    return child_pid;
  } else{
    if(execvp (program, arg_list) == 0);
    perror("Exec failed");
    WriteLog("Exec failed");
    return 1;
  }
}

int main() {
  if((FDlog = open("log/master.log",O_WRONLY|O_CREAT|O_APPEND, 0666))==-1){
    perror("Error opening master log file");
    return 1;
  }
  
  WriteLog("Master process started.");

  int mode, status;
  
  do{
    
    do{
      printf("There are 3 modes available:\n 1.Normal mode\n 2.Server mode\n 3.Client mode\n\nSelect one: ");
      scanf("%d", &mode);

      if(mode<1 || mode>3){
        printf("Error in the selection. Please try again.\n");
      }
    } while(mode<1 || mode>3);

    switch(mode){
      case 1:
        printf("Normal mode selected.\n");
        sprintf(process, "./bin/processA");
        WriteLog("Normal mode selected.");
        break;
      case 2:
        printf("Server mode selected.\nInsert port: ");
        scanf("%s", port);
        sprintf(process, "./bin/processAserver");
        WriteLog("Server mode selected.");
        break;
      case 3:
        printf("Client mode selected.\nInsert an address: ");
        scanf("%s", address);
        printf("Insert a port: ");
        scanf("%s", port);
        sprintf(process, "./bin/processAclient");
        WriteLog("Client mode selected.");
        break;
      default:
        perror("Error selecting the mode.");
        WriteLog("Error selecting the mode.");
        return EXIT_FAILURE;
        break;
    }

    char * arg_list_A[] = { "/usr/bin/konsole", "-e", process, port, address, NULL };
    char * arg_list_B[] = { "/usr/bin/konsole", "-e", "./bin/processB", NULL };

    pid_t pid_procA = spawn("/usr/bin/konsole", arg_list_A);
    pid_t pid_procB = spawn("/usr/bin/konsole", arg_list_B);

    WriteLog("A & B processes spawned.");

    waitpid(pid_procA, &status, 0);
    waitpid(pid_procB, &status, 0);

    if(status==-1){
      printf ("Encountered an error in process: %s\n", process);
    }
    printf ("Processes exiting with status %d\n\n", status);
  
  } while(status==0 || status==-1);
  
  WriteLog("Master process terminated.");

  close(FDlog);
  printf ("Main program exiting with status %d\n", status);
  
  return 0;
}

