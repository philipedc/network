# Networking

## TP1

Code written in C using the lib socket to create a minefield game communication between client and server. The specification is inside tp1/ (but it's in portuguese)

### How to use:

```
make
./bin/server <v4/v6> <Port_Number> -i ./input/example_dashboard.txt
./bin/client <IP> <Port_Number>
```

### Commands:


- "start": initialize the table
- "reveal x y": reveal the position (x, y)
- "flag x y": mark the position (x, y) (It's just cosmetic)
- "remove_flag x y": unmark the position (x, y)
- "reset": reset all modifications to the table
- "exit": disconnect from the server

**These commands works only in client side!**

## TP2

Code also uses lib socket of C, but was added multithreading to serve different clients in a blogpost application.

### How to use:

```
make
./bin/server <v4/v6> <Port_Number> -i ./input/example_dashboard.txt
./bin/client <IP> <Port_Number>
```

### Commands:

- "subscribe in topic": start to listen to publications in that topic (if it don't exist, it will be created)
- "list topics": show all topics created
- "publish in topic": after enter you'll be prompted to write something until the next enter, the message will be delievered to all subscribers of topic
- "exit": quit the program
  
**These commands works only in client side!**
