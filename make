#!/bin/dash

current_time=$(date +"%Y-%m-%d %H:%M:%S")
echo "Building at $current_time"
zig cc -Wall -g -o exe main.c -lGL -lGLU -lGLEW -lglfw &&
#gcc -Wall -g -o exe main.c -lGL -lGLU -lGLEW -lglfw &&
echo "\nBuild Complete.\nRunning:\n" && ./exe && echo "\nFinished.\n"
