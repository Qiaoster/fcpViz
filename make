#!/bin/dash

current_time=$(date +"%Y-%m-%d %H:%M:%S")
echo "Building at $current_time"
zig cc -Wall -g -o build/exe src/main.c -lm -lGL -lGLU -lGLEW -lglfw &&
#gcc -Wall -g -o exe main.c -lm -lGL -lGLU -lGLEW -lglfw &&
echo "\nBuild Complete.\nRunning:\n" && build/exe && echo "\nFinished.\n"
