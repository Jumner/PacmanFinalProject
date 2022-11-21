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
char ghostDirection(char **map, int ghosts[2][3], int ghost);
char **loadMap(char file[], int ghosts[2][3]) {
  int ghost = 0;
  char **map = (char **)malloc(11 * sizeof(char *));
  for (int i = 0; i < 11; i++) {
    map[i] = (char *)malloc(11 * sizeof(char));
  }
  FILE *mapPtr = fopen(file, "r");
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {
      if ((x > 0 && x < 10) && (y > 0 && y < 10)) {
        fscanf(mapPtr, "%s", &map[x][y]);
        if (map[x][y] == GHOST) {
          ghosts[ghost][0] = x;
          ghosts[ghost][1] = y;
          ghost++;
          map[x][y] = DOT;
        }
      } else {
        map[x][y] = WALL;
      }
    }
  }
  fclose(mapPtr);
  for (int i = 0; i < 2; i++) {
    ghosts[i][2] = ghostDirection(map, ghosts, i);
  }
  return map;
}
void unloadMap(char **map) {
  for (int i = 0; i < 11; i++) {
    free(map[i]);
  }
  free(map);
}
void printMap(char **map, int ghosts[2][3]) {
  int ghost = 0;
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {
      if (x == ghosts[0][0] && y == ghosts[0][1]) {
        colourChangeWin(PINK);
        printf("G  ");
      } else if (x == ghosts[1][0] && y == ghosts[1][1]) {
        colourChangeWin(BLUE);
        printf("G  ");
      } else if (map[x][y] == PACMAN) {
        colourChangeWin(YELLOW);
        printf("%c  ", map[x][y]);
      } else {
        colourChangeWin(WHITE);
        printf("%c  ", map[x][y]);
      }
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

int loseCheck(char **map, int ghosts[2][3]) {
  int pacX, pacY;
  findPacman(map, &pacX, &pacY);
  for (int i = 0; i < 2; i++) {
    if (ghosts[i][0] == pacX && ghosts[i][1] == pacY) {
      return 1;
    }
  }
  return 0; //
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

int parallel(char dir1, char dir2) {
  if (dir1 == 0 || dir2 == 0) {
    return 0;
  }
  if (dir1 == UP || dir1 == DOWN) {
    return (dir2 == UP || dir2 == DOWN);
  } else {
    return (dir2 == LEFT || dir2 == RIGHT);
  }
}

char ghostDirection(char **map, int ghosts[2][3], int ghost) {
  char dirLengths[4] = {0, 0, 0, 0};
  int longestDir;
  char dirs[] = "wasd";
  for (int i = 0; i < 4; i++) {
    if (!parallel(dirs[i], ghosts[ghost][2])) {
      longestDir = i;
      break;
    }
  }
  int points[4][2];
  for (int i = 0; i < 4; i++) {
    for (int o = 0; o < 2; o++) {
      points[i][o] = ghosts[ghost][o];
    }
    while (!isWall(map, points[i][0], points[i][1], dirs[i])) {
      move(&points[i][0], &points[i][1], dirs[i]);
      if (map[points[i][0]][points[i][1]] == PACMAN) {
        ghosts[ghost][2] = dirs[i];
        return dirs[i];
      }
      dirLengths[i]++;
    }
    if (dirLengths[i] > dirLengths[longestDir] &&
        !parallel(dirs[i], ghosts[ghost][2])) {
      longestDir = i;
    }
  }
  // Pacman not found
  return dirs[longestDir]; // Pacman not found
}

void moveGhosts(char **map, int ghosts[2][3]) {
  for (int i = 0; i < 2; i++) {
    char dir = ghostDirection(map, ghosts, i);
    if (isWall(map, ghosts[i][0], ghosts[i][1], ghosts[i][2])) {
      ghosts[i][2] = dir;
    }
    move(&ghosts[i][0], &ghosts[i][1], ghosts[i][2]);
  }
}

void runGame(char **map, int ghosts[2][3]) {
  while (true) {           // Boy I missed true and false
    printMap(map, ghosts); //
    if (winCheck(map)) {
      printf("Congratulations! You win! Press any key to exit the game\n");
      getch();
      return;
    } else if (loseCheck(map, ghosts)) {
      printf("Sorry, you lose. Press any key to exit the game\n");
      getch();
      return;
    }
    char input = getInput(map);

    moveGhosts(map, ghosts);
    movePacman(map, input);
  }
}

int main() {
  // colourChangeWin(PINK);
  int ghosts[2][3];
  char **map = loadMap("map.txt", ghosts);
  runGame(map, ghosts);
  unloadMap(map);
  return 0;
}
