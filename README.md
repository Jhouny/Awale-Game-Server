# Awale Game Server
The goal of this project is to implement an Awalé game server. The aim is to have a client/server application that allows clients to play matches, verify that the rules are correctly applied, communicate, and keep traces (scores, games played, chat etc.).

## Requirements
 - [ ] Implement the Awalé game:
    - [X] Internal representation
    - [X] How to play a move
    - [X] Count points
    - [ ] Save a game
    - [X] Print the board state

 - [ ] Design a client/server application.
    - Use a structure or enum to list all possible server commands.
    - [ ] Each client registers with a username.
    - [ ] Each client can request from the server the list of online usernames.
    - [ ] Client A can challenge client B. 
    - [ ] A Client can accept or refuse a challenge.
    - [ ] When a match is created between A and B, the server randomly decides who starts.
    - [ ] The server verifies the legality of moves.
    - [ ] Ensure it also works for multiple simultaneous games.
    - [ ] Add a feature for "observer" mode, where the server sends the board and score to C who observes the game between A and B.
    - [ ] Implement a chat option where players can exchange messages to chat (both inside and outside a game).
    - [ ] Allow a player to write a bio (e.g. 10 ASCII lines) to introduce themselves $\to$ The server should be able to display the bio of a given username.
    - [ ] Add a private mode where a player can limit the list of spectators to a list of friends.
    - [ ] Add the ability to review saved games.

- Extra features (optional):
    - [ ] Free to your imagination, player rankings (see Elo rating)
    - [ ] Tournament organization, adapting it to another game, etc.
    - [ ] Implement a web interface for the server.

## Compilation
To compile the project, use the provided Makefile. From the source directory, run:
```bash
make all
```
