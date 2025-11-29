# uP2_Final_Project

## Basketball Game

- Threads to Add:
  - Move Character Thread:
    - Gets Joystick position and writes to the FIFO
  - Update Screen Thread:
    - Only move certain things based on FIFO (maybe a bool)
  - Menu Thread:
  - Selection Thread:
  - Enemy Thread:
  - DAC Timer Handler:
  - Button Handler:
  - Joystick Button Handler:
  - Game Init Thread:

## Important Notes

- Add uartstdio.h and uartstdio.c into the driverlib file
- This will help fix compiler issues with CCS
- Joystick ADC needs a faster clock or else it won't turn on
