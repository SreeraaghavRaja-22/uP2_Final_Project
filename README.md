# uP2_Final_Project

## Basketball Game

- Threads to Add:
  - Game Init Thread:
    - Initialize the Game
      - Draw Player Position, Hoop Position, Ball Position
  - Move Character Thread:
    - Move Character Left or Right based on Joystick (using a FIFO caused a lot of issues)
  - Gets Joystick position and writes to the FIFO
  - Update Screen Thread:
  - Only move certain things based on FIFO (maybe a bool)
  - Menu Thread:
    - Button Thread
    - Almost Done
  - Selection Thread:
    - Same as Menu Thread
  - Enemy Thread:
    - Done
  - DAC Timer Handler:
  - Button Handler:
    - Finished
  - Joystick Button Handler:
    - Optional
-Almost done
  - finish working on resetting ball
  - ending game
  - restarting game

## Important Notes

- Add uartstdio.h and uartstdio.c into the driverlib file
- This will help fix compiler issues with CCS
- Joystick ADC needs a faster clock or else it won't turn on
