# Compile

gcc src/processA.c -lncurses -lbmp -lm -o bin/processA -lrt -pthread &
gcc src/processAserver.c -lncurses -lbmp -lm -o bin/processAserver -lrt -pthread &
gcc src/processAclient.c -lncurses -lbmp -lm -o bin/processAclient -lrt -pthread &
gcc src/processB.c -lncurses -lbmp -lm -o bin/processB -lrt -pthread &
gcc src/master.c -o bin/master

echo "Files compiled."
