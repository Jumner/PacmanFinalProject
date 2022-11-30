// APSC 143 Engineering Programming Project Starter Code
// Feel free to change anything about this file, but do not remove "#include
// "colours.h".

// Make sure to include all relevant libraries
#include "colours.h"
#include <conio.h> // For instantanious output
// Please clarify with me if you have issues because I am cross compiling to
// windows and running inside wine
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

// Declarations

char **map;                // Declare a global 2d array of chars for the map
int ghosts[2][3];          // Declare a global 2d array of ints for the ghost position and direction
void loadMap(char file[]); // Load map from file and return it
void unloadMap();          // Free the memory allocated by loadMap()
void printMap();           // Print the map to the user
int winCheck();            // Returns 1 if user has won
int loseCheck();           // Returns 1 if user has lost
char getInput();           // returns a *Valid* character instantaniously
int isDirection(char input); // Returns if character is a direction (wasd)
int isWall(int x, int y,
           char dir); // Returns 1 if moving from x,y in direction dir would hit a wall
int parallel(char dir1, char dir2);    // Returns 1 if dir1 is parallel to dir2
void findPacman(int *xPos, int *yPos); // Sets xPos,yPos to pacmans position
char ghostDirection(int ghost); // Modifies ghost direction if pacman is seen and returns direction
                                // to move if wall is hit
void move(int *x, int *y, char dir); // Moves x,y 1 unit in direction dir
void movePacman(char dir);           // Moves pacman 1 unit in direction dir
void moveGhosts();                   // Handles all ghost movement
void runGame();                      // Runs the main game loop

// Function Definitions

void loadMap(char file[]) {
  int ghost = 0; // Keep track of which ghost so they can be coloured differently
  map = (char **)malloc(11 * sizeof(char *));   // Allocate enough space for 11 char pointers
  for (int i = 0; i < 11; i++) {                // For each char pointer
    map[i] = (char *)malloc(11 * sizeof(char)); // Allocate enough space for 11 chars
  }
  FILE *mapPtr = fopen(file, "r"); // Open the file passed in in read mode
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {                  // Iterate through map
      if ((x > 0 && x < 10) && (y > 0 && y < 10)) { // If its not in the outer border
        fscanf(mapPtr, "%s",
               &map[x][y]); // Scans a string so spaces are ignored (only really gets one char)
        if (map[x][y] == GHOST) {
          ghosts[ghost][0] = x; // Set ghost position
          ghosts[ghost][1] = y; // Arrays are pointers so its always passed by reference
          ghost++;              // Move on to the next ghost
          map[x][y] = DOT;      // Change G to . character
        }
      } else {            // Edge of map
        map[x][y] = WALL; // Set to wall (outer wall not included in map file)
      }
    }
  }
  fclose(mapPtr);               // Clsoe the file pointer
  for (int i = 0; i < 2; i++) { // Set the initial state of the ghosts
    ghosts[i][2] =
        ghostDirection(i); // Set the initial direction (it is initially 0 because it will not hit a
                           // wall whilst stationary and cant see the player)
  }
}

void unloadMap() {
  for (int i = 0; i < 11; i++) { // Go through each array in the 2d array
    free(map[i]);                // Free each array
  }
  free(map); // Free the outer 2d array
}

void printMap() {
  for (int y = 0; y < 11; y++) {
    for (int x = 0; x < 11; x++) {                  // Iterate through the map
      if (x == ghosts[0][0] && y == ghosts[0][1]) { // At the position of the first ghost
        colourChange(PINK);                         // First one is pink!
        printf("G "); // Print G character with a space (map[x][y] has '.' in it)
      } else if (x == ghosts[1][0] && y == ghosts[1][1]) { // At the position of the second ghost
        colourChange(BLUE);                                // Second one is blue!
        printf("G ");                   // Print G character with a space (map[x][y] has '.' in it)
      } else {                          // Not a ghost
        if (map[x][y] == PACMAN) {      // It's Pacman
          colourChange(YELLOW);         // Make him yellow
        } else if (map[x][y] == WALL) { // Walls
          colourChange(BLUE);           // Are blue!
        } else {                        // Dots
          colourChange(WHITE);          // Are white!
        }
        printf("%c ", map[x][y]); // Print the character
      }
    }
    printf("\n"); // Add a newline after each row
  }
}

int winCheck() {
  for (int y = 1; y < 10; y++) {
    for (int x = 1; x < 10; x++) { // Iterate through inside of board (no dots on the boarder)
      if (map[x][y] == DOT) {      // Is there a dot
        return 0;                  // Dot found! Game is not won
      }
    }
  }
  return 1; // No dots could be found. Game has been won!
}

int loseCheck() {
  int pacX, pacY;
  findPacman(&pacX, &pacY);                             // Put pacmans position in pacX,pacY
  for (int i = 0; i < 2; i++) {                         // For each ghost
    if (ghosts[i][0] == pacX && ghosts[i][1] == pacY) { // Ghost in the same pos as pacman
      return 1;                                         // Pacman died ;(
    }
  }
  return 0; // Pacman is not dead
}

char getInput() {
  char input = getch();      // use conio.h's getch function which doesn't wait for enter key
  if (!isDirection(input)) { // If input is not a valid direction (wasd)
    return getInput();       // Try again (not technically recursion as my HS teacher would argue)
  }
  int x, y;
  findPacman(&x, &y);        // Store pacmans position in x,y
  if (isWall(x, y, input)) { // If direction would cause him to hit a wall
    return getInput();       // Try again
  }
  return input; // Valid input, return it
}

int isWall(int x, int y, char dir) {
  move(&x, &y, dir);          // Move 1 unit in direction dir from x,y
  return (map[x][y] == WALL); // Returns 1 if there is a wall at new position
}

int isDirection(char input) {
  char dirs[] = "wasd";         // Array of valid directions
  for (int i = 0; i < 4; i++) { // Iterate through them
    if (input == dirs[i]) {     // Input equals one of the directions
      return 1;                 // Valid direction
    }
  }
  return 0; // Input didn't match a valid input
}

int parallel(char dir1, char dir2) {
  if (dir1 == 0 || dir2 == 0) { // If one is not defined
    return 0;                   // Not parallel
  }
  if (dir1 == UP || dir1 == DOWN) {         // If dir1 is vertical
    return (dir2 == UP || dir2 == DOWN);    // Returns 1 if dir2 is also vertical
  } else {                                  // Dir1 is horizontal
    return (dir2 == LEFT || dir2 == RIGHT); // Returns 1 if dir2 is also horizontal
  }
}

void findPacman(int *xPos, int *yPos) {
  for (int y = 1; y < 10; y++) {
    for (int x = 1; x < 10; x++) { // Iterate through inner board
      if (map[x][y] == PACMAN) {   // If its pacman
        *xPos = x;                 // Update the values in the xPos, yPos pointers
        *yPos = y;
        return; // Exit out (no need to keep looking)
      }
    }
  }
}

char ghostDirection(int ghost) {
  // This function is very large. I know.
  // It first extends a line of vision in 4 directions to find pacman. If pacman is found, the
  // direction of the ghost is changed. Otherwise, the longest path is picked. The ghost will move
  // in the same direction until it either spots pacman or hits a wall. To prevent it from turning
  // around after hitting a wall, the new direction is always perpendicular to the current one. This
  // wall check is done in moveGhosts

  char dirLengths[4] = {0, 0, 0, 0}; // Stores the lengths of the sightlines in all 4 directions
  int longestDir;                    // Stores the index of the longest sightline
  char dirs[] = "wasd";              // Stores the valid directions
  int points[4][2];                  // Stores the position of the ends of the 4 sightlines

  for (int i = 0; i < 4; i++) { // Iterate through all 4 directions
    if (!parallel(
            dirs[i],
            ghosts[ghost][2])) { // Find one thats perpendicular
                                 // This is because we are finding the longest perpendicular one
      longestDir = i;            // Set the temp longest direction to the perpendicular one
      break;
    }
  }

  for (int i = 0; i < 4; i++) {        // Iterate through each direction
    for (int o = 0; o < 2; o++) {      // Iterate through x and y
      points[i][o] = ghosts[ghost][o]; // Set point to ghost position
    }
    while (!isWall(points[i][0], points[i][1],
                   dirs[i])) { // Keep moving until moving would hit a wall

      move(&points[i][0], &points[i][1], dirs[i]); // Move in the current direction
      if (map[points[i][0]][points[i][1]] ==
          PACMAN) {                 // If Pacman is at point (along the sightline)
        ghosts[ghost][2] = dirs[i]; // Update the ghosts direction (so it moves right away)
        return dirs[i];             // And return it
      }
      dirLengths[i]++; // Gone another step along the sightline, so increase its length
    }

    if (dirLengths[i] > dirLengths[longestDir] && !parallel(dirs[i], ghosts[ghost][2])) {
      // If the sightline is longer and also perpendicular
      longestDir = i; // Update the longest one
    }
  }
  // Pacman not found
  return dirs[longestDir]; // Pacman not found so return the longest path perpendicular to the
                           // current direction
}

void move(int *x, int *y, char dir) {
  int xdir = 0, ydir = 0; // 2d direction vector
  // 0,0 is top left
  switch (dir) {
  case DOWN:
    ydir = 1; // Down increases y
    break;
  case UP:
    ydir = -1; // Up decreases y
    break;
  case RIGHT:
    xdir = 1; // Right increases x
    break;
  case LEFT:
    xdir = -1; // Left decreases x
    break;
  }
  *x += xdir; // Increment position by direction vector
  *y += ydir;
}

void movePacman(char dir) {
  int x, y;
  findPacman(&x, &y); // Store pacmans position in x,y
  map[x][y] = EMPTY;  // Set his current square to empty (he ate the dot)
  move(&x, &y, dir);  // Move x,y in direction dir
  map[x][y] = PACMAN; // Set his new square to pacman (overriding the dot)
}

void moveGhosts() {
  for (int i = 0; i < 2; i++) {   // For each ghost
    char dir = ghostDirection(i); // change ghost direction if pacman is spotted otherwise
                                  // return new direction to go if wall is hit
    if (isWall(ghosts[i][0], ghosts[i][1],
               ghosts[i][2])) { // If ghost hits a wall moving in the current direction
      ghosts[i][2] = dir;       // Change to the new direction
    }
    move(&ghosts[i][0], &ghosts[i][1],
         ghosts[i][2]); // Move the ghost by 1 unit in its possibly new direction
  }
}

void runGame() {
  while (true) {      // Boy I missed true and false
    printMap();       // Print the map out
    if (winCheck()) { // If player won
      colourChange(WHITE);
      printf("Congratulations! You win! Press any key to exit the game\n");
      getch();                // Wait for keypress to exit
      return;                 // Leave function, game over!
    } else if (loseCheck()) { // If player lost
      colourChange(WHITE);
      printf("Sorry, you lose. Press any key to exit the game\n");
      getch(); // Wait for keypress to exit
      return;  // Leave function, game over!
    }
    char input = getInput(); // Get a valid direction to move

    moveGhosts();      // Move the ghosts
    movePacman(input); // Move pacman in the entered direction
  }
}

int main() {
  int ghosts[2][3];   // Ghosts stores 2 arrays that each store x,y,direction (ghosts only change
                      // direction when spotting pacman or hitting walls) so its cached
  loadMap("map.txt"); // Load the map from map.txt
  runGame();          // Run the game loop
  unloadMap();        // Game over! free the memory in map
  return 0;           // Program exited successfully
}
