# Networking_Example

This is a simple networking client-server application written in C++ using ASIO library.<br>
The architecture is not mine and ***I do not take credit*** for it, the original code can be found here:<br>

https://github.com/OneLoneCoder/Javidx9/tree/master/PixelGameEngine/BiggerProjects/Networking<br>

And an in depth explanation here:<br>

Part 1: https://www.youtube.com/watch?v=2hNdkYInj4g<br>
Part 2: https://www.youtube.com/watch?v=UbjxGvrDrbw<br>
Part 3: https://www.youtube.com/watch?v=hHowZ3bWsio<br>


## Modifications
The code is not just a copy-paste of the original code, it is formatted differently, 
and some things are the way I think would be more useful for me in the future:<br>

1. The library is not only header-based
2. Not using a template for the message type
3. Not waiting in the server main for messages, but running a fixed 50 FPS loop.


## TODOs
1. Have a base class message and custom fixed format messages that are transmitted, 
instead of a vector of bytes. Maybe some polymorphic magic.
2. I need to learn more about ASIO and how to connect two different PCs instead of 
just running everything on the local machine.
3. Use this code in conjunction with the SDL2 engine app to make a graphical server-
client application or game.
