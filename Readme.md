# Państwa-Miasta Server

## How to communicate with this server 

### Use frames that look like so
'**TypeOfMessage(Data)**'

e.g. : 

- Guess(panstwo)

- Error("no more space in the lobby")

- and so on

---------------
## Gameplay


### Serwer side 

#### Server received an inquiry regarding nonexistent lobby
'NoLobby(pokoj1)'

#### Server send the remaining time of the lobby to player
'Time(15)'



### Client side 

#### Set up a name
'PlayerName'

#### Join a Lobby
'LobbyName(pokoj1)'

#### Player sends the guess
'Guess(panstwo,Malta)'
'Guess(miasto,Warszawa)'





### Both sides

#### Send just a message
'Msg(Hello)'

#### 



--------------------
## How to compile

create and move to the build directory
run:
    'cmake ..'
    'make '
    './panstwa_miasta 12345 (example)'