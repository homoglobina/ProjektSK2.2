# How to communicate with this server 

### (NOT YET IMPLEMENTED) send and receive messages of specific format, we can use that since we use TCP and we are guaranteed that whole frames will be send/received
### i propose that a single frame could look like below

TYPE_OF_MESSAGE(DATA,DATA,(...))

e.g.

GUESS(panstwo,Albania)

ERROR("no more space in the lobby")

and so on


## How to compile

create and move to the build directory
run:
    cmake ..
    make 
    ./panstwa_miasta 12345 (example)