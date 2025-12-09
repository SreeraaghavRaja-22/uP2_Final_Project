# NBA 67k

## Description of Each Thread

## Description of Game Logic

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
    - Used to Restart Program
-Almost done
  - Work on Collision Logic to make it a little better
  - Trying making refresh rate a little faster
  - Work On Scoreboard
  - Work On Start Menu WITH TITLE (NBA 67K)
  - Work on sprites

## Important Notes

- Add uartstdio.h and uartstdio.c into the driverlib file
- This will help fix compiler issues with CCS
- Joystick ADC needs a faster clock or else it won't turn on
  