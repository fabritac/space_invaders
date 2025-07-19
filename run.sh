#!/bin/bash

# Compile the program
gcc main.c -o game.exe -lraylib -lm -lpthread -lrt -lX11 -I include/ -L lib/

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    # Run the program
    ./game.exe
else
    echo "Compilation failed."
fi
