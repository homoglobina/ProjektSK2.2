# How to communicate with this server 

# Frames that look like bellow
TYPE_OF_MESSAGE(DATA)

e.g.

GUESS(panstwo)

ERROR("no more space in the lobby")

and so on

# Proposed frames

Player_Name(Adam)  # for sending user's name to the server when first connecting

Error(InvalidName) # Server sending errors regarding error with username propositions
Error(TakenName)
Msg(helllo new player)






## How to compile

create and move to the build directory
run:
    cmake ..
    make 
    ./panstwa_miasta 12345 (example)