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

In the `/src` folder, there are five files: processA, processB and processAclient processAserver and master. 

When the user run the program they will see three possible choices; 1)Normal Mode, 2)Server Mode, 3)Client Mode user need to select one of them by entering 1,2 or 3. Normal mode is same with the second ARP assignment. 

The second mode of the program had been provided by creating the processAclient the aim of the second mode is, the client establishing a connection to an application with similar functionality that is running on another machine within the network. For that purpose when the user enter 2 the Server Mode will be open and the user will see "Insert port:" and the user needs to enter port number, then the program start to work. 

The third mode of the program had been provided by creating the processAserver the aim of the third mode is, the server is establishing a connection to an application with similar functionality that is running on another machine within the network. For that purpose when the user enter 3 the Client Mode will be open and the user needs to enter the address and the port respectively after that the program starts to work.

After program starts to work two consoles will appear: process A and process B. The user can control the pivot (green cross), which is inside of the process A window console, by using the right, left, upper and down buttons on the keyboard. Depends on user inputs, the object will move in the console. There is also a blue button (P button) for printing an image (multiple numbered images can be saved), which is saved as `.bmp` file into the folder `/output`. The image represents the position of the pivot in process A, represented by a blue circle. 

Inside of the master file child process creation and processA and processB console creations had been done. By using the WriteLog function to writing the log file had been provided. The modes had been defined here. When the user will close the processA and processB window the selections become visible again and the user can select the new mode, this feature created by using do loop inside of the master. 

Each process stores its status in a `.log` file into the folder `/log`.





