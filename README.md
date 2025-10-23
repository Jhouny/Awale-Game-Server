# Awale Game Server
The goal of this project is to implement an Awalé game server. The aim is to have a client/server application that allows clients to play matches, verify that the rules are correctly applied, communicate, and keep traces (scores, games played, chat etc.).

## Requirements
 - [ ] Implement the Awalé game: internal representation, how to play a move, count points, save a game, print the board state, etc.
 - [ ] Design a client/server application. Each client registers with a username.
 - [ ] Each client can request from the server the list of online usernames.
 - [ ] Client A can challenge client B. B can accept or refuse.
 - [ ] When a match is created between A and B, the server randomly decides who starts. The server verifies the legality of moves (using the code created in step 0).
 - [ ] If you have a working version for one game, ensure it also works for multiple simultaneous games. You can add features such as listing ongoing games and an "observer" mode where the server sends the board and score to C who observes the game between A and B.
 - [ ] Implement a chat option, in addition to sending moves, players can exchange messages to chat (both inside and outside a game).
 - [ ] Allow a player to write a bio, say 10 ASCII lines to introduce themselves. The server should be able to display the bio of a given username.
 - [ ] Add a private mode, a player can limit the list of spectators to a list of friends. Implement this feature.
 - [ ] Add the ability to save played games so they can be reviewed later.
 - [ ] Free to your imagination, player rankings (Wikipedia article on Elo rating), tournament organization, adapting it to another game, etc.
