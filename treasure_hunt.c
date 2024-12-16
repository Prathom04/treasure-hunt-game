#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <string.h>

#define GRID_SIZE 11

// Difficulty settings
#define EASY_TRAPS 5
#define MEDIUM_TRAPS 8
#define HARD_TRAPS 12

#define EASY_POWERUPS 3
#define MEDIUM_POWERUPS 5
#define HARD_POWERUPS 8

#define TREASURES 4 // Number of treasures in the game

// Function to move the cursor to a specific position on the console
void set_cursor_position(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {x, y};
    SetConsoleCursorPosition(hConsole, position);
}

// Function to display the grid
void display_grid(char grid[GRID_SIZE][GRID_SIZE],int explored[GRID_SIZE][GRID_SIZE], int player_x, int player_y, int health, int score, int time_left)
{
    set_cursor_position(0, 0); // Move the cursor to the top-left corner
    printf("Treasure Hunt Game\n");
    printf("Use W, A, S, D to move. Press Enter to explore, Space to exit.\n");
    printf("T: Treasure | X: Trap | P: Power-up | .: Unexplored | K: Secret Key\n\n");
    printf("Time Left: %d seconds | Health: %d | Score: %d\n", time_left, health, score);

    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (x == player_x && y == player_y) {
                printf("P\t"); // Player's position
            }
            else if(explored[y][x]==1){
                printf("%c\t",grid[y][x]);
            }
             else {
                // If the cell has been explored (not a '.'), display the actual content
                printf("?\t", grid[y][x]);
            }
        }
        printf("\n");
    }
}

// Function to initialize the grid with treasures, traps, power-ups, and the secret key
void initialize_grid(char grid[GRID_SIZE][GRID_SIZE], int *treasuresRemaining) {
    // Fill grid with unexplored cells
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = '?'; // Unexplored cell
        }
    }

    // Place treasures
    int treasure_count = 0;
    while (treasure_count < TREASURES) {
        int treasure_x = rand() % GRID_SIZE;
        int treasure_y = rand() % GRID_SIZE;
        if (grid[treasure_y][treasure_x] == '?') {
            grid[treasure_y][treasure_x] = 'T';
            treasure_count++;
        }
    }

    // Place the secret key
    int key_x, key_y;
    do {
        key_x = rand() % GRID_SIZE;
        key_y = rand() % GRID_SIZE;
    } while (grid[key_y][key_x] != '?');
    grid[key_y][key_x] = 'K'; // Secret key

    // Place traps and power-ups
    int traps = rand() % EASY_TRAPS + 5; // Placeholder traps
    int powerups = rand() % EASY_POWERUPS + 3;

    // Random placement for traps and power-ups
    for (int i = 0; i < traps + powerups; i++) {
        int x, y;
        do {
            x = rand() % GRID_SIZE;
            y = rand() % GRID_SIZE;
        } while (grid[y][x] != '?'); // Empty cell
        if (i < traps) {
            grid[y][x] = 'X'; // Trap
        } else {
            grid[y][x] = '+'; // Power-up
        }
    }
}

// Function to save player data to a file
void save_player_data(const char* playerName, int score, int health, int remainingTime) {
    FILE *file = fopen("player_data.txt", "a");
    if (file == NULL) {
        printf("Error opening file to save player data.\n");
        return;
    }
    fprintf(file, "Player: %s\nScore: %d\nHealth: %d\nTime Remaining: %d seconds\n\n", playerName, score, health, remainingTime);
    fclose(file);
    printf("\nPlayer data saved successfully.\n");
}

// Main game function
int main() {
    char grid[GRID_SIZE][GRID_SIZE];
    int explored[GRID_SIZE][GRID_SIZE] = {0}; //2D array to track explored cells
    int player_x = 0, player_y = 0; // Player's starting position
    int treasures_remaining = TREASURES;
    int score = 0;
    int health = 4; // Player starts with 3 health
    int found_key = 0; // Secret key state
    int difficulty = 0;
    int time_limit = 60; // Default time limit is 60 seconds
    char player_name[100]; // To store player's name

    // Get player name
    printf("Enter your name: ");
    fgets(player_name, sizeof(player_name), stdin);
    player_name[strcspn(player_name, "\n")] = '\0'; // Remove newline character at the end of the name

    // Get difficulty level from the user
    printf("Select Difficulty (1 - Easy, 2 - Medium, 3 - Hard): ");
    scanf("%d", &difficulty);
    if (difficulty == 1) {
        time_limit = 120; // 2 minutes for easy
    } else if (difficulty == 2) {
        time_limit = 90; // 1.5 minutes for medium
    } else if (difficulty == 3) {
        time_limit = 60; // 1 minute for hard
    }

    srand(time(0)); // Seed for random number generation

    // Initialize the grid and place items
    initialize_grid(grid, &treasures_remaining);

    // Display the initial grid
    display_grid(grid,explored,player_x,player_y,health,score,time_limit);

    // Game loop
    time_t start_time = time(NULL); // Start the timer
    int elapsed_time = 0; // Declare elapsed_time variable

    while (1) {
        if (health <= 0) {
            printf("\nYou ran out of health! Game over!\n");
            break;
        }

        // Check for time limit
        elapsed_time = (int)(time(NULL) - start_time);
        if (elapsed_time >= time_limit) {
            printf("\nTime's up! Game over!\n");
            break;
        }

        // Handle player input
        if (_kbhit()) { // Check if a key is pressed
            char key = _getch(); // Get the pressed key

            // If Space is pressed, exit the game
            if (key == ' ') {
                printf("\nYou pressed Space! Exiting the game...\n");
                break;
            }
            // Movement logic
            if (key == 'w' || key == 'W') { // Move up
                if (player_y > 0) player_y--;
            } else if (key == 's' || key == 'S') { // Move down
                if (player_y < GRID_SIZE - 1) player_y++;
            } else if (key == 'a' || key == 'A') { // Move left
                if (player_x > 0) player_x--;
            } else if (key == 'd' || key == 'D') { // Move right
                if (player_x < GRID_SIZE - 1) player_x++;
            } else if (key == '\r') { // Enter key to explore
                explored[player_y][player_x] = 1;
                if (grid[player_y][player_x] == 'T') { // Found a treasure
                    printf("\nYou found a treasure! +10 points.\n");
                    score += 10;
                    treasures_remaining--;
                    grid[player_y][player_x] = 'T'; // Show treasure
                    }
                    else if (treasures_remaining == 0) {
                        printf("\nCongratulations! You found all treasures!\n");
                        break;
                }
                 else if (grid[player_y][player_x] == 'X') { // Trap
                    printf("\nOh no! You stepped on a trap! Lost 1 health.\n");
                    health--;
                    grid[player_y][player_x] = 'X'; // Show trap
                }
                 else if (grid[player_y][player_x] == '+') { // Power-up
                    printf("\nYou found a power-up! +1 health.\n");
                    health += 1;
                    grid[player_y][player_x] = '+'; // Show power-up
                }
                else if(grid[player_y][player_x]== '?'){
                    printf("\nNothing is here. keep going...\n");
                    grid[player_y][player_x] = ' '; //explored places.
                }
                 else if (grid[player_y][player_x] == 'K' && !found_key) { // Secret key
                    printf("\nYou found the secret key! All traps are removed.\n");
                    found_key = 1;
                    // Remove all traps
                    for (int y = 0; y < GRID_SIZE; y++) {
                        for (int x = 0; x < GRID_SIZE; x++) {
                            if (grid[y][x] == 'X') {
                                grid[y][x] = ' '; // Remove trap
                            }
                        }
                    }
                }
            }
            // Update and display the grid
            display_grid(grid, explored, player_x, player_y, health, score, time_limit - elapsed_time);
        }
    }

    // Save player data after the game ends
    save_player_data(player_name, score, health, time_limit - elapsed_time);

    return 0;
}
