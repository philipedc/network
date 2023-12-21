# Networking

## TP1

Code written in C using the lib socket to create a minefield game communication between client and server. The specification is inside tp1/ (but it's in portuguese)

### How to use:

- Build the project running "make"
- Execute the server: "./bin/server <v4/v6> <Port_Number> -i <path_to_board>"
- Execute the client: "./bin/client <IP> <Port_Number>"
- Now you can execute "Commands" in the client terminal

### Commands:

- "start": initialize the table
- "reveal x y": reveal the position (x, y)
- "flag x y": mark the position (x, y) (It's just cosmetic)
- "remove_flag x y": unmark the position (x, y)
- "reset": reset all modifications to the table
- "exit": disconnect from the server

## TP2

Code also uses lib socket of C, but was added multithreading to serve different clients in a blogpost application.

### How to use:

- Build the project running "make"
- Execute the client: "./bin/client <IP> <Port_Number>"
- Now you can execute "Commands" in the client terminal

### Commands:

- "subscribe in topic": start to listen to publications in that topic (if it don't exist, it will be created)
- "list topics": show all topics created
- "publish in topic": after enter you'll be prompted to write something until the next enter, the message will be delievered all subscriber of topic
- "exit": quit the program
  
**These commands works only in client side!**
