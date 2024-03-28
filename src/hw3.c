#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <ctype.h> 

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
/*
typedef struct GameState{
    char **array;
    int **counterarray;
    int rows;
    int rowlen;
    GameState pastpointer;
} GameState;
*/

GameState* initialize_game_state(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    GameState *state = malloc(sizeof(GameState));
    if (state == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    state->pastpointer = NULL;

    char c;
    int index = 0, rowlen = 0, numofrows = 0;

    c = fgetc(file);
    while (c != EOF) {
        if (c == '\n' && rowlen == 0) {
            rowlen = index;
        }
        if (c != '\n') {
            index++;
        }
        c = fgetc(file);
    }

    numofrows = index / rowlen;

    if (rowlen == 0 || numofrows == 0) {
        perror("Critical failure: Invalid board dimensions");
        exit(EXIT_FAILURE);
    }

    rewind(file);

    state->rows = numofrows;
    state->rowlen = rowlen;

    state->array = malloc(numofrows * sizeof(char *));
    state->counterarray = malloc(numofrows * sizeof(int *));
    if (state->array == NULL || state->counterarray == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numofrows; i++) {
        state->array[i] = malloc(rowlen * sizeof(char));
        state->counterarray[i] = malloc(rowlen * sizeof(int));
        if (state->array[i] == NULL || state->counterarray[i] == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    }

    int x = 0, y = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            y = 0;
            x++;
        } else {
            state->array[x][y++] = c;
        }
    }

    for (int i = 0; i < numofrows; i++) {
        for (int j = 0; j < rowlen; j++) {
            if (state->array[i][j] != '.') {
                state->counterarray[i][j] = 1;
            } else {
                state->counterarray[i][j] = 0;
            }
        }
    }

    fclose(file);
    return state;
}


GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    long unsigned int counter = 0;
    int temprow = row, tempcol = col, index = 0, ifnotzerotrue = 0, temprowforoverwrite = row, tempcolforoverwrite = col;
    *num_tiles_placed = 0;
    char *word = NULL, *overwriteword = NULL;

    if (check_for_2_letter(game) == 3 && strlen(tiles) < 2) { // Board is empty
        printf("\nWORD TOO SMALL FOR FIRST ROT: \n");
        *num_tiles_placed = 0;
        return game; // FIRST WORD IS TOO SMALL
    }

    if ((row < 0) || (col < 0) || (tiles == NULL) || (game->rowlen <= col) || (game->rows <= row)) {
        printf("Invalid initialization condition\n");
        *num_tiles_placed = 0;
        return game;
    }

    // Dynamically extend the board if necessary
    if (direction == 'V') { // Use rows variable
        game = gameextender(game);
        if ((size_t)game->rows < (row + strlen(tiles))) { 
            array_extender(game, 1, (row + strlen(tiles)) - game->rows);
        }
        // Placing tiles vertically
        while (counter < strlen(tiles)) {
            if (!isspace(tiles[counter])) {
                game->array[row][col] = tiles[counter];
                if (game->counterarray[row][col] != 0) { // Check for overrunning 
                    ifnotzerotrue++;
                }
                game->counterarray[row][col] += 1;

                if (game->counterarray[row][col] > 5) { // Check for height greater than 5
                    printf("GREATER THAN 5 \n");
                    *num_tiles_placed = 0;
                    game = game->pastpointer;
                    return game;
                }

                *num_tiles_placed += 1;
            }
            counter++;
            row++;
        }

        // Go back and record overwritten word
        while (temprow - 1 >= 0 && game->counterarray[temprow - 1][tempcol] != 0) {
            temprow--;
            counter++;
        }

        // Dynamically allocate memory for overwritten word
        overwriteword = malloc((counter + 1) * sizeof(char));
        if (overwriteword == NULL) 
            printf("failure in overwrite\n");

        // Record the overwritten word
        if (game->pastpointer != NULL) {
            int index2 = 0, temprow2 = temprowforoverwrite;
            while ((temprow2 < game->pastpointer->rows) && (game->pastpointer->array[temprow2][tempcol] != '.')) {
                overwriteword[index2] = game->pastpointer->array[temprow2][tempcol];
                index2++;
                temprow2++;
            }
        }

        // Record the word being placed
        index = 0;
        word = malloc((counter + 1) * sizeof(char));
        if (word == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        while ((temprow < game->rows) && (game->array[temprow][tempcol] != '.')) {
            word[index] = game->array[temprow][tempcol];
            index++;
            temprow++;
        }
        word[index] = '\0';
        overwriteword[index] = '\0';
    }
    else if (direction == 'H') { // Use rowlen variable
        game = gameextender(game);
        if ((size_t)game->rowlen < (col + strlen(tiles))) {
            array_extender(game, 0, (col + strlen(tiles)) - game->rowlen);
        }
        // Placing tiles horizontally
        while (counter < strlen(tiles)) {
            if (!isspace(tiles[counter])) {
                game->array[row][col] = tiles[counter];
                if (game->counterarray[row][col] == 0) { // Check for overrunning 
                    ifnotzerotrue++;
                }
                game->counterarray[row][col] += 1;

                if (game->counterarray[row][col] > 5) { // Check for height greater than 5
                    printf("GREATER THAN 5 \n");
                    *num_tiles_placed = 0;
                    game = game->pastpointer;
                    return game;
                }

                *num_tiles_placed += 1;
            }
            counter++;
            col++;
        }

        // Go back and record overwritten word
        while (tempcol - 1 >= 0 && game->counterarray[temprow][tempcol - 1] != 0) {
            tempcol--;
            counter++;
        }

        // Dynamically allocate memory for overwritten word
        overwriteword = malloc((counter + 1) * sizeof(char));
        if (overwriteword == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        // Record the overwritten word
        if (game->pastpointer != NULL) {
            int index2 = 0, tempcol2 = tempcolforoverwrite;
            while ((tempcol2 < game->pastpointer->rowlen) && (game->pastpointer->array[temprow][tempcol2] != '.')) {
                overwriteword[index2] = game->pastpointer->array[temprow][tempcol2];
                index2++;
                tempcol2++;
            }
        }

        // Record the word being placed
        index = 0;
        word = malloc((counter + 1) * sizeof(char));
        if (word == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        while ((tempcol < game->rowlen) && (game->array[temprow][tempcol] != '.')) {
            word[index] = game->array[temprow][tempcol];
            tempcol++;
            index++;
        }
        word[index] = '\0';
        overwriteword[index] = '\0';
    }

    // Check if the word exists and handle overwrite condition
    if (check_word(word) == 0) { // Word does not exist
        printf("\nNOT A WORD: %s \n", word);
        *num_tiles_placed = 0;
        game = game->pastpointer;
    } else if (check_word(overwriteword) == 1 && strcmp(word, overwriteword) == 0) { // Word exists and it's being overwritten
        printf("OVERWRITE: %s word %s\n", overwriteword, word);
        *num_tiles_placed = 0;
        game = game->pastpointer;
    }

    // Print the updated game state
    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->rowlen; j++) {
            printf(" %c", game->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    // Free dynamically allocated memory
    free(word);
    free(overwriteword);

    return game;
}


int check_for_2_letter(GameState *game){
    int counter = 0;
    for(int i = 0; i < game->rows;i++){
        for(int e = 0; e<game->rowlen;e++){
            if(game->counterarray[i][e] != 0)
            counter++;
        }
    }
    if((counter == 1) | (counter == 2))
    return 1;   //first word needs to be greater than 2
    if(counter == 0)
    return 3;   // board is emptey

    return 0;   //board is correct
}

int check_word(char *word) {
    FILE *file = fopen("./tests/words.txt", "r");
    if (file == NULL) {
        perror("Error: ");
        return 0;   //not found
    }
    char line[1024];

    rewind(file);

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        char upperLine[1024];
        for (int i = 0; line[i]; i++) {
            upperLine[i] = toupper((char)line[i]);
        }
        upperLine[strlen(line)] = '\0';

        if (strcmp(word, upperLine) == 0) {
            return 1; // found
        }
    }

    return 0; // not found

}

GameState* gameextender(GameState *game) {
    // Create a new game state
    GameState *newgame = malloc(sizeof(GameState));
    if (newgame == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Initialize the new game state
    newgame->pastpointer = game;
    newgame->rows = game->rows;
    newgame->rowlen = game->rowlen;

    // Allocate memory for array and counterarray pointers
    newgame->array = malloc(newgame->rows * sizeof(char *));
    newgame->counterarray = malloc(newgame->rows * sizeof(int *));
    if (newgame->array == NULL || newgame->counterarray == NULL) {
        perror("Memory allocation failed");
        // Free allocated memory before returning NULL
        free(newgame->array);
        free(newgame->counterarray);
        free(newgame);
        return NULL;
    }

    // Copy the contents of the array and counterarray from the original game state
    for (int i = 0; i < newgame->rows; i++) {
        newgame->array[i] = malloc(newgame->rowlen * sizeof(char));
        newgame->counterarray[i] = malloc(newgame->rowlen * sizeof(int));
        if (newgame->array[i] == NULL || newgame->counterarray[i] == NULL) {
            perror("Memory allocation failed");
            // Free allocated memory before returning NULL
            for (int j = 0; j < i; j++) {
                free(newgame->array[j]);
                free(newgame->counterarray[j]);
            }
            free(newgame->array);
            free(newgame->counterarray);
            free(newgame);
            return NULL;
        }
        // Copy the contents of the array and counterarray from the original game state
        memcpy(newgame->array[i], game->array[i], newgame->rowlen * sizeof(char));
        memcpy(newgame->counterarray[i], game->counterarray[i], newgame->rowlen * sizeof(int));
    }

    return newgame;
}

GameState* undo_place_tiles(GameState *game) {//god help me
    if (game->pastpointer != NULL) {
        return game->pastpointer; 
    } else {
        return game; 
    }
}

void array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index){ //resizes and puts in peroids this needs work (need to add in num array feture)
if (extend_num_of_rows_if_one == 1) { // Extend the number of 'rows'
    game->rows += inc_index; // Increment the number of rows

    // Reallocate memory for the array of pointers
    char **temp_array = realloc(game->array, game->rows * sizeof(char *));
    int **temp_counter_array = realloc(game->counterarray, game->rows * sizeof(int *));

    if (temp_array == NULL || temp_counter_array == NULL) {
        // Handle memory allocation failure
        // Print an error message or take appropriate action
        // It's important not to proceed further if memory allocation fails
    } else {
        game->array = temp_array;
        game->counterarray = temp_counter_array;

        // Initialize the newly allocated rows
        for (int i = game->rows - inc_index; i < game->rows; i++) {
            game->array[i] = malloc(game->rowlen * sizeof(char));
            game->counterarray[i] = malloc(game->rowlen * sizeof(int));
            for (int j = 0; j < game->rowlen; j++) {
                game->array[i][j] = '.';
                game->counterarray[i][j] = 0;
            }
        }
    }
} else { // Extend the length of 'rowlength'
    game->rowlen += inc_index; // Increment the row length

    // Reallocate memory for each row individually
    for (int i = 0; i < game->rows; i++) {
        char *temp_row_array = realloc(game->array[i], game->rowlen * sizeof(char));
        int *temp_row_counter_array = realloc(game->counterarray[i], game->rowlen * sizeof(int));

        if (temp_row_array == NULL || temp_row_counter_array == NULL) {
            // Handle memory allocation failure
            // Print an error message or take appropriate action
            // It's important not to proceed further if memory allocation fails
            // You may need to free the previously allocated memory before exiting
        } else {
            game->array[i] = temp_row_array;
            game->counterarray[i] = temp_row_counter_array;

            // Initialize the newly allocated columns
            for (int j = game->rowlen - inc_index; j < game->rowlen; j++) {
                game->array[i][j] = '.';
                game->counterarray[i][j] = 0;
            }
        }
    }
}
}

void free_game_state(GameState *game) {
    
    if (game != NULL)
    free(game);
}

void save_game_state(GameState *game, const char *filename) { //done?
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for(int i = 0; i < game->rows;i++){         // actual characters
        for(int e= 0; e < game->rowlen; e++){
            fprintf(file, "%c", game->array[i][e]);
        }
        fprintf(file, "\n");
    }
    
    for(int i = 0; i < game->rows;i++){        // counter under the characters
        for(int e= 0; e < game->rowlen; e++){
            fprintf(file, "%d", game->counterarray[i][e]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}