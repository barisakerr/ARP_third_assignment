#include "./../include/processB_utilities.h"
#include <time.h>
#include <fcntl.h>
#include <bmpfile.h>
#include <semaphore.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>


int width=1600;
int height=600;
int depth=4;

int radius=30;

//Shared memory mtrx
struct SharedMemory{
    int mtrx[1600][600]; 
};

//Semaphores
sem_t * semaphore0;
sem_t * semaphore1; 

//Log file
int FDlog;
char LogBuffer[100];

//Get time for Log file
time_t rawtime;
struct tm *info;

//Write log file
void WriteLog(char *msg){
  time( &rawtime );
  info = localtime(&rawtime);

  sprintf(LogBuffer, msg);
  sprintf(LogBuffer + strlen(LogBuffer), asctime(info));

  if(write(FDlog, LogBuffer, strlen(LogBuffer))==-1){
    close(FDlog);
    perror("Error in writing function");
    exit(1);
    }
}

// Function to draw (cflag==True) or to delele (cflag==False) the blue circle
void BlueCircle(int radius, int x, int y, bmpfile_t *bmp, bool cflag){
    rgb_pixel_t pixel;

    if(cflag==true){
        pixel.blue=255; //Blue
        pixel.green=0;
        pixel.red=0;
        pixel.alpha=0;
    } else{
        pixel.blue=255; //Blue
        pixel.green=255;
        pixel.red=255;
        pixel.alpha=0;
    }
    
    for(int i=-radius; i<=radius; i++){
        for(int j=-radius; j<=radius; j++){
            // If distance is smaller, point is within the circle
            if(sqrt(i*i+j*j) < radius){
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, x*20+i, y*20+j, pixel);
            }
        }
    }
}

int main(int argc, char const *argv[]){
    if ((FDlog = open("log/processB.log",O_WRONLY|O_CREAT|O_APPEND, 0666))==-1){
        perror("Error opening process B log file");
        exit(1);
    }

    WriteLog("Process B started.");

    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    //Variables for shared memory
    int SMid;
    key_t SMkey;
    struct SharedMemory *SMpointer;

    SMkey = ftok(".", 'x');
    //Get ID
    SMid = shmget(SMkey, sizeof(struct SharedMemory), IPC_CREAT | 0666);
    if(SMid<0){
        perror("Error getting Shared Memory ID.");
        WriteLog("Error getting Shared Memory ID.");
        exit(1);
    }

    //Attach the shared memory
    SMpointer=(struct SharedMemory *) shmat(SMid, NULL, 0);   
    if((int) SMpointer==-1){
        perror("Error attaching Shared Memory.");
        WriteLog("Error attaching Shared Memory.");
        exit(1);
    }

    //Open semaphores
    semaphore0=sem_open("/sem", 0);
    semaphore1=sem_open("/sem1", 0); 
    if(semaphore0==(void*)-1 || semaphore1==(void*)-1){
        perror("Error opening semaphores.");
        WriteLog("Error opening semaphores.");
        exit(1);
    }

    //Create the bpm file with sizes
    bmpfile_t *bmp;
    bmp=bmp_create(width, height, depth);
    
    int OldX;
    int OldY;
    int Center = 0;
    int CordY[600];
    int CordX[600];

    int index;  
    bool flag;
    
    // Infinite loop
    while (TRUE) {

        // Get input in non-blocking mode
        int cmd = getch();

        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        } else{
            mvaddch(LINES/2, COLS/2, '0');
            refresh();

            sem_wait(semaphore1);

            //Get old center coordinates
            if(CordY[index-1]!=0 && CordX[index-1]!=0){
                OldX=CordX[index-1];
                OldY=CordY[index-1];
            }
            
            for(int i=0;i<height;i++){
                //Reset
                if(CordY[i] != 0){
                    CordY[i]=0;
                } else if(CordX[i] != 0){
                    CordX[i]=0;
                }
            }

            Center=0;    
            index=0;
            flag=false;
            
            //Find the circle center
            for(int i=0; i<width; i++){
                if(flag==true){
                    break;
                }
                for(int j=0; j<600; j++){
                    if (SMpointer->mtrx[i][j]==1){
                        CordY[index]=j;
                        CordX[index]=i;

                        //If the y coordinate is greater than the previous one, break the loop
                        if (CordY[index]>CordY[index-1]){
                            flag=true;
                            break;
                        }
                        index++;
                        break;
                    }
                }
            }
            mvaddch(floor((int)((CordY[index-1]+radius)/20)),floor((int)(CordX[index-1]/20)), '0');
            refresh();

            sem_post(semaphore0);

            //Delete old blue circle
            BlueCircle(radius, OldX, OldY, bmp, false); 
            //Draw new blue circle
            BlueCircle(radius, CordX[index-1], CordY[index-1], bmp, true);  
        }
    }

    //Close and unlink everything
    sem_close(semaphore0);
    sem_close(semaphore1);
    shmdt((void *) SMpointer);
    bmp_destroy(bmp);  

    endwin();

    WriteLog("Process B terminated.");
    close(FDlog);    
    
    return 0;
}
