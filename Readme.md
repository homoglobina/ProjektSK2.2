# Państwa-Miasta Server

## How to communicate with this server without a GUI client

### Use frames that look like so
```
**TypeOfMessage(Data)**
```

e.g. : 
```
 Guess(1panstwo)

 Error("no more space in the lobby")

 and so on
```
---------------
## Gameplay


### Serwer side 

#### Server received an inquiry regarding nonexistent lobby
```
NoLobby(pokoj1)
```
#### Server sends info regarding what categories are in the game
```
Category(135)
```

Categories:
    - countries "1"
    - cities "2" // polish
    - cities "3" // important worldwide 
    - lakes "4"
    - fruit "5"
    - names "6"
    ...

#### Server send the remaining time of the lobby to player
```
Time(15)
```



### Client side 

#### Set up a name
```
PlayerName(Bartek)
```

#### Join a Lobby
```
LobbyName(pokoj1)
```

#### Change game Settings
```
LobbySettings()
```

#### Start a game 
```
LobbyStart()
```

#### Player sends the guess
```
Guess(1,Malta)
Guess(2,Warszawa)
```




### Both sides

#### Send just a message
```
Msg(Hello)
```
#### 



--------------------
## How to compile

create and move to the build directory
run:
```
    cmake ..
    make 
    ./panstwa_miasta 12345 (example)
```
