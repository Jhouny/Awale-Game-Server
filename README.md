# Awale Game Server
The goal of this project is to implement an Awalé game server. The aim is to have a client/server application that allows clients to play matches, verify that the rules are correctly applied, communicate, and keep traces (scores, games played, chat etc.).


## Compilation
To compile the project, use the provided Makefile. From the source directory, run:
```bash
make
```
Then to run the project you must do : 
For the server : ./bin/server --'port' (not mandatory, by default 3000)
For the client : ./bin/client 'IP adress' 'port'

## AI use

We used AI to accelerate development in parts of the project where the logic was repetitive—for example, sending messages to the server and handling the responses in each state. It also helped us debug certain sections of the code.
However, we did not rely on AI to design the project structure, define the different states, or create the communication protocol. All of that was planned and implemented by us, as shown in the document data/Prog Reseau – Projet AWALE STRUCTURE.pdf.