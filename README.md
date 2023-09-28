# Networking

## TP1

Code written in C using the lib socket to create a minefield game communication between client and server. The specifications are in the file ./TP1/TP1.pdf

### How to use:

- Build the project running "make"
- Execute the server: "./bin/server <v4/v6> <Port Number> -i <path_to_board>
- Execute the client: "./bin/client <IP> <Port Number>
- Now you can execute "Commands" in the client terminal

### Commands:

- "start": initialize the table
- "reveal x y": reveal the position (x, y)
- "flag x y": mark the position (x, y) (It's just cosmetic)
- "remove_flag x y": unmark the position (x, y)
- "reset": reset all modifications to the table
- "exit": disconnect from the server