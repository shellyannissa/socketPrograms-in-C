#!/bin/bash

# Get the folder name as input
read -p "Enter the folder name: " folder_name

# Get the current working directory
current_dir=$(dirname "$0")

# Compile and execute the server.c file in a new integrated terminal
osascript -e 'tell application "Terminal" to do script "cd '$current_dir/$folder_name'; gcc server.c -o server; ./server"'

# Open a new integrated terminal and execute the client.c file
osascript -e 'tell application "Terminal" to do script "cd '$current_dir/$folder_name'; gcc client.c -o client; ./client 192.168.1.5"'