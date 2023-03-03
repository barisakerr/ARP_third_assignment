#include "./../include/processA_utilities.h"
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Global variables
int width=1600;
int height=600;
int depth=4;

int radius=30;

//Shared memory matrix
struct SharedMemory{
    int mtrx[1600][600]; 
};

//Semaphores
sem_t *semaphore0;
sem_t *semaphore1;

//Counter for multiple photos
int bmpCounter=0;

//Log file
int FDlog;
char LogBuffer[100];

//Get time for Log file
time_t rawtime;
struct tm *info;

//Write log file
void WriteLog(char *msg){
  time(&rawtime);
  info=localtime(&rawtime);

  sprintf(LogBuffer, msg);
  sprintf(LogBuffer + strlen(LogBuffer), asctime(info));

  if(write(FDlog, LogBuffer, strlen(LogBuffer))==-1){
    close(FDlog);
    perror("Error in writing function");
    exit(1);
    }
}

//Function to draw (cflag==True) or to delele (cflag==False) the blue circle
void BlueCircle(int x, int y, bmpfile_t *bmp, bool cflag){
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
            if(sqrt(i*i + j*j)<radius){
                /*
                * Color the pixel at the specified (x,y) position
                * with the given pixel values
                */
                bmp_set_pixel(bmp, x*20+i, y*20+j, pixel);
            }
        }
    }
}

void SaveBMP(bmpfile_t *bmp){
    //Store name and concatenate it with a counter variable for saving multiple photos
    //(sometimes it could save 2 photos at once)
    char name[50];
    snprintf(name, 50, "output/image%d.bmp", bmpCounter);
    
    bmp_save(bmp, name);
    WriteLog("bpm file saved.");
    bmpCounter++;
}

int main(int argc, char *argv[]){
    if((FDlog=open("log/processAclient.log",O_WRONLY|O_CREAT|O_APPEND, 0666))==-1){
        perror("Error opening process A Client log file.");
        exit(1);
    }

    WriteLog("Process A Client started.");

    // Utility variable to avoid trigger resize event on launch
    int first_resize=TRUE;

    // Initialize UI
    init_console_ui();

    //Take port and address as arguments
    int port=atoi(argv[1]);
    char *serverAddress=argv[2];

    //Create a socket struct for address and port
    struct sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    address.sin_addr.s_addr=inet_addr(serverAddress);

    //Create a socket
    int sckt=socket(AF_INET, SOCK_STREAM, 0);//
    if (sckt==-1){
        perror("Error creating socket");
        WriteLog("Error creating socket");
    }

    //Connect to server
    int FDclient=connect(sckt, (struct sockaddr*) &address, sizeof(address));//
    if (FDclient==-1){
        perror("Error during connection to a server");
        WriteLog("Error during connection to a server");
        close(FDclient);
    }
    
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
    semaphore0=sem_open("/sem", O_CREAT, 0666, 1);
    semaphore1=sem_open("/sem1", O_CREAT, 0666, 0); 
    if(semaphore0==(void*)-1 || semaphore1 == (void*)-1){
        perror("Error opening semaphores.");
        WriteLog("Error opening semaphores.");
        exit(1);
    }

    //Create the bpm file with sizes
    bmpfile_t *bmp;
    bmp=bmp_create(width, height, depth);

    sem_wait(semaphore0);

    //Draw the blue circle
    BlueCircle(circle.x, circle.y, bmp, true);

    // Write to the shared memory
    for(int i=0; i<width; i++) {
        for(int j=0; j<height; j++){

            //Get the pixel
            rgb_pixel_t *pixel=bmp_get_pixel(bmp, i, j);  
            
            //Set the shared memory to 1, if the pixel is blue
            if((pixel->blue==255) && (pixel->red==0) && (pixel->green==0) && (pixel->alpha==0)){
                SMpointer->mtrx[i][j]=1;
            }
        }
    }

    sem_post(semaphore1);

    // Infinite loop
    while(TRUE){
        //Coordinates of the circle
        int x=circle.x;
        int y=circle.y;

        // Get input in non-blocking mode
        int cmd=getch();

        // If user resizes screen, re-draw UI...
        if(cmd==KEY_RESIZE){
            if(first_resize){
                first_resize=FALSE;
            } else{
                reset_console_ui();
            }

        } else if(cmd == KEY_MOUSE){ //Else, if user presses print button...
            if(getmouse(&event) == OK){
                if(check_button_pressed(print_btn, &event)){
                    mvprintw(LINES - 1, 1, "Print button pressed");
                    WriteLog("Print button pressed");

                    //Save bpm image and log
                    SaveBMP(bmp);                    

                    refresh();
                    sleep(1);
                    for(int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }

        } else if(cmd==KEY_LEFT || cmd==KEY_RIGHT || cmd==KEY_UP || cmd==KEY_DOWN){ // If input is an arrow key, move circle accordingly...

            sem_wait(semaphore0);

            WriteLog("Command received.");

            //Send commands to the server
            if(send(sckt, cmd, 4, 0)==-1){
                perror("Error sending command to the server");
                WriteLog("Error sending command to the server");
                close(FDclient);
                return -1;
            }

            move_circle(cmd);
            draw_circle();

            //Delete the blue circle
            BlueCircle(x, y, bmp, false);

            //Set shared memory to 0
            for(int i=0; i<width; i++) {
                for(int j=0; j<height; j++) {
                    SMpointer->mtrx[i][j]=0;
                }
            }

            //Draw the blue circle
            BlueCircle(circle.x, circle.y, bmp, true);

            // Write to the shared memory
            for(int i=0; i<width; i++){
                for(int j=0; j<height; j++){

                    //Get the pixel
                    rgb_pixel_t *pixel=bmp_get_pixel(bmp, i, j);  
                    
                    //Set the shared memory to 1, if the pixel is blue
                    if((pixel->blue==255) && (pixel->red==0) && (pixel->green==0) && (pixel->alpha==0)){
                        SMpointer->mtrx[i][j]=1;
                    }
                }
            }
            sem_post(semaphore1);
        }
    }
    
    //Close and unlink everything
    sem_close(semaphore0);
    sem_close(semaphore1);
    sem_unlink("/sem");
    sem_unlink("/sem1");
    bmp_destroy(bmp);
    shmdt((void *) SMpointer);
    shmctl(SMid, IPC_RMID, NULL);
    close(FDclient);

    endwin();

    WriteLog("Process A terminated.");
    close(FDlog);

    return 0;
}
