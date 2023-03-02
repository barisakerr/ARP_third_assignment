# ARP_third_assignment

## Project made by:

Claudio Tomaiuolo S5630055

Barış Aker S5430437

## ncurses library installation
To install the ncurses library, simply open a terminal and type the following command:
```console
sudo apt-get install libncurses-dev
```
## libbitmap library installation
Download this repository: https://github.com/draekko/libbitmap.

Navigate to the root directory of the folder in the console and run the following commands.

For making the configuration
```console
./configure 
```

For compiling
```console
make
```

For installing
```console
sudo make install
```

After the installation, check if the library has been installed navigating to `/usr/local/lib`, where you are supposed to find the `libbmp.so` file.

The last step is to open the `.bashrc` into the terminal:
```console
nano .bashrc
```
and add `export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"`.
Now reload your `.bashrc`:
```console
source .bashrc
```

## Run the code
Navigate to root directory of the project.

Make the files executable:
```console
chmod +x *.sh
```
Compile the files:
```console
./compile.sh
```
Then, run the code with:
```console
./run.sh
```

## Explaination 

The aim of the Project is to create separated processes which are in communication with each other through a shared memory. 

In the `/src` folder, there are three files: processA, processB and master. 

When the user runs the program, two consoles will appear: process A and process B. The user can control the pivot (green cross), which is inside of the process A window console, by using the right, left, upper and down buttons on the keyboard. Depends on user inputs, the object will move in the console. There is also a blue button (P button) for printing an image (multiple numbered images can be saved), which is saved as `.bmp` file into the folder `/output`. The image represents the position of the pivot in process A, represented by a blue circle. 
During these processes happening, the user can follow the object movements by 0s continuously drawn on the process B window console.
The process A and process B are in connection with each other through a shared memory and the usage of two semaphores. 

Inside of the master file there is child process creation and processA and processB console creations.

Each process stores its status in a `.log` file into the folder `/log`.
