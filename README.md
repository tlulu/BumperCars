# Bumper Cars

![alt text](https://github.com/tlulu/BumperCars/blob/master/screenshot.png)

## Compilation:
```
$ cd A5/
$ ./premake4 gmake
$ make
$ ./A5
```

## Game rules:
The objective of the game is to score as many goals in your opponent's goal as possible.
The time limit is 30 seconds. After that, a winner is decided and the game must be restarted.

- Control the first car with arrow keys
- Control second car with "wasd" keys

## For debugging:
Camera position can be moved with:
Key 1: moves right
Key 2: moves left
Key 3: moves up
Key 4: moves down
Key 5: moves into the screen
Key 6: moves out of the screen

Light position can be moved with:
Key 7: moves right
Key 8: moves left
Key 9: moves up
Key 0: moves down
Key -: moves into the screen
Key +: moves out of the screen

## Objectives completed:
1. Modelling the scene
2. UI is added - Keyboard used for moving game objects
3. Static collision is implemented
4. Dynamic collision is implemented
5. Bump mapping is added
6. Reflection is implemented using stencil buffer
7. Synchronized sound is added
8. Reflection map is implemented through a skybox