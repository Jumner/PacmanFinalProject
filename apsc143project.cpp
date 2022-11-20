// APSC 143 Engineering Programming Project Starter Code
// Feel free to change anything about this file, but do not remove "#include
// "colours.h".

// Make sure to include all relevant libraries
#include "colours.h"
#include <conio.h>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

// You don't need to use these, but they help make your code look more organized
#define PACMAN 'P'
#define GHOST 'G'
#define DOT '.'
#define WALL 'W'
#define EMPTY ' '
#define UP 'w'
#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'

// Below are the required functions for this program, but you will need to
// create your own as well. Make sure to specify the required parameters, if
// any.
char **loadMap(char file[]) {
  char **map = (char **)malloc(11 * sizeof(char *));
  for (int i = 0; i < 11; i++) {
    map[i] = (char *)malloc(11 * sizeof(char));
  }
  FILE *mapPtr = fopen(file, "r");
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {
      if ((x > 0 && x < 10) && (y > 0 && y < 10)) {
        fscanf(mapPtr, "%s", &map[x][y]);
      } else {
        map[x][y] = WALL;
      }
    }
  }
  return map;
}
void printMap(char **map) {
  int ghost = 0;
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {
      switch (map[x][y]) {
      case PACMAN:
        colourChangeWin(YELLOW);
        break;
      case GHOST:
        if (ghost == 0) {
          colourChangeWin(PINK);
          ghost++;
        } else {
          colourChangeWin(BLUE);
        }
        break;
      default:
        colourChangeWin(WHITE);
        break;
      }
      printf("%c  ", map[x][y]);
    }
    printf("\n");
  }
}
int winCheck(char **map) {
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {
      if (map[x][y] == DOT) {
        return 0;
      }
    }
  }
  return 1;
}
int loseCheck(char **map) {
  return 0; //
}
void move(int *x, int *y, char dir) {
  int xdir = 0, ydir = 0;
  switch (dir) {
  case DOWN:
    ydir = 1;
    break;
  case UP:
    ydir = -1;
    break;
  case RIGHT:
    xdir = 1;
    break;
  case LEFT:
    xdir = -1;
    break;
  }
  *x += xdir;
  *y += ydir;
}
int isWall(char **map, int x, int y, char dir) {
  move(&x, &y, dir);
  return (map[x][y] == WALL); //
}

void findPacman(char **map, int *xPos, int *yPos) {
  for (int y = 1; y < 10; y++) {
    for (int x = 1; x < 10; x++) {
      if (map[x][y] == PACMAN) {
        *xPos = x;
        *yPos = y;
        return;
      }
    }
  }
}

int isDirection(char input) {
  char dirs[] = "wasd";
  for (int i = 0; i < 4; i++) {
    if (input == dirs[i]) {
      return 1;
    }
  }
  return 0;
}

char getInput(char **map) {
  char input = getch(); //
  if (!isDirection(input)) {
    return getInput(map);
  }
  int x, y;
  findPacman(map, &x, &y);
  if (isWall(map, x, y, input)) {
    return getInput(map);
  }
  return input;
}

void movePacman(char **map, char dir) {
  int x, y;
  findPacman(map, &x, &y);
  map[x][y] = EMPTY;
  move(&x, &y, dir);
  map[x][y] = PACMAN;
}

void runGame(char **map) {
  while (true) {   // Boy I missed true and false
    printMap(map); //
    if (winCheck(map)) {
      printf("Congratulations! You win! Press any key to exit the game\n");
      getch();
      return;
    } else if (loseCheck(map)) {
      printf("Sorry, you lose. Press any key to exit the game\n");
      getch();
    }
    char input = getInput(map);
    movePacman(map, input);
  }
}

int main() {
  // colourChangeWin(PINK);
  char **map = loadMap("map.txt");
  runGame(map);
  return 0;
}
