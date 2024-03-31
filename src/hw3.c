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
        perror("Failed\n");
        exit(EXIT_FAILURE);
    }

    GameState *state = malloc(sizeof(GameState));
    if (state == NULL) {
        perror("failed memory\n");
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
        perror("critical failure: unvalid board dimensions\n");
        exit(EXIT_FAILURE);
    }

    rewind(file);

    state->rows = numofrows;
    state->rowlen = rowlen;

    state->array = malloc(numofrows * sizeof(char *));
    state->counterarray = malloc(numofrows * sizeof(int *));
    if (state->array == NULL || state->counterarray == NULL) {
        perror("memory allocation failed\n");
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
    int temprow = row, tempcol = col, index = 0, ifnotzerotrue = 0, 
    temprowforoverwrite = row, tempcolforoverwrite = col, temprowforother= row, tempcolforother = col;
    *num_tiles_placed = 0;
    char word[255+strlen(tiles)], overwriteword[255+strlen(tiles)];
    int ticker = check_for_2_letter(game);

    if (ticker == 3 && strlen(tiles) < 2) { // Board is empty
        printf("\nWORD TOO SMALL FOR FIRST ROT: \n");
        *num_tiles_placed = 0;
        return game; // FIRST WORD IS TOO SMALL
    }

    if ((row < 0) || (col < 0) || (tiles == NULL) || (game->rowlen <= col) || (game->rows <= row)) {
        printf("Invalid initialization\n");
        *num_tiles_placed = 0;
        return game;
    }

    if (direction == 'V') { // Use rows variable
        game = gameextender(game);
        if ((size_t)game->rows < (row + strlen(tiles))) { 
            array_extender(game, 1, (row + strlen(tiles)) - game->rows);
        }
        while (counter < strlen(tiles)) {
            if (!isspace(tiles[counter])) {
                game->array[row][col] = tiles[counter];
                if (game->counterarray[row][col] != 0) { //  overrunning 
                    ifnotzerotrue++;
                }
                game->counterarray[row][col] += 1;

                if (game->counterarray[row][col] > 5) { // Check for height greater than 5
                    printf("GREATER THAN 5 \n");
                    *num_tiles_placed = 0;
                    game = undo_place_tiles(game);
                    return game;
                }

                *num_tiles_placed += 1;
            }
            counter++;
            row++;
        }

        while (temprow - 1 >= 0 && game->counterarray[temprow - 1][tempcol] != 0) {
            temprow--;
            counter++;
        }

        overwriteword[0] = '\0';

        if (game->pastpointer != NULL) {
            int index2 = 0, temprow2 = temprowforoverwrite;
            while ((temprow2 < game->pastpointer->rows) && (game->pastpointer->array[temprow2][tempcol] != '.')) {
                overwriteword[index2] = game->pastpointer->array[temprow2][tempcol];
                index2++;
                temprow2++;
            }
        }

        index = 0;
        word[0] = '\0';
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
                    game = undo_place_tiles(game);
                    return game;
                }

                *num_tiles_placed += 1;
            }
            counter++;
            col++;
        }

        while (tempcol - 1 >= 0 && game->counterarray[temprow][tempcol - 1] != 0) {
            tempcol--;
            counter++;
        }

        overwriteword[0] = '\0';

        if (game->pastpointer != NULL) {
            int index2 = 0, tempcol2 = tempcolforoverwrite;
            while ((tempcol2 < game->pastpointer->rowlen) && (game->pastpointer->array[temprow][tempcol2] != '.')) {
                overwriteword[index2] = game->pastpointer->array[temprow][tempcol2];
                index2++;
                tempcol2++;
            }
        }

        index = 0; // record word place
        word[0] = '\0';

        while ((tempcol < game->rowlen) && (game->array[temprow][tempcol] != '.')) {
            word[index] = game->array[temprow][tempcol];
            tempcol++;
            index++;
        }
        word[index] = '\0';
        overwriteword[index] = '\0';
    }

/*
        for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->rowlen; j++) {
            printf(" %c", game->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
*/
    if (check_word(word) == 0) { // Word dne
        printf("\nNOT A WORD: %s \n", word);
        *num_tiles_placed = 0;
        game = undo_place_tiles(game);
    } 
    else if (check_word(overwriteword) == 1 && strcmp(word, overwriteword) == 0){ //&& strcmp(word, overwriteword) == 0) { // word overwritten!!
        printf("OVERWRITE: %s word %s\n", overwriteword, word);
        *num_tiles_placed = 0;
        game = undo_place_tiles(game);
    }
    else if(ticker != 3 && oppo_check(temprowforother, tempcolforother, strlen(tiles), game, direction) != 0){
        printf("OPPOCHECK TRiggered: %s\n", word);
        *num_tiles_placed = 0;
        game = undo_place_tiles(game);
    }
    
    return game;
}

int oppo_check(int row, int col, int size, GameState *game, char direction) {
    int adjcount = 0, index, check_word_indicator_zero_if_false = 1;

    if (direction == 'H') {
        index = col;
        while (index < game->rowlen) {
            if (row - 1 >= 0) { // Check upper
                if (game->counterarray[row - 1][index] != 0){
                    adjcount++;
                    if(check_word_indicator_zero_if_false == 1 && check_word_OPPO(direction, row, index, game) == 0)
                        check_word_indicator_zero_if_false = 0;
                }
            }
            if (row + 1 < game->rows) { // Check lower
                if (game->counterarray[row + 1][index] != 0){
                    adjcount++;
                    if(check_word_indicator_zero_if_false == 1 && check_word_OPPO(direction, row, index, game) == 0)
                        check_word_indicator_zero_if_false = 0;
                }
            }
            index++; // Increment index
        }
        if (col - 1 >= 0) { // Check left
            if (game->counterarray[row][col - 1] != 0){
                adjcount++;
            }
        }
        if (col + size < game->rowlen) { // Check right
            if (game->counterarray[row][col + size] != 0){
                adjcount++;
            }
        }
    }   
    else if (direction == 'V') {
        index = row;
        while (index < game->rows) {
            if (col - 1 >= 0) { // Check left
                if (game->counterarray[index][col - 1] != 0){
                    adjcount++;
                    if(check_word_indicator_zero_if_false == 1 && check_word_OPPO(direction, index, col, game) == 0)
                        check_word_indicator_zero_if_false = 0;
                }
            }
            if (col + 1 < game->rowlen) { // Check right
                if (game->counterarray[index][col + 1] != 0){
                    adjcount++;
                    if(check_word_indicator_zero_if_false == 1 && check_word_OPPO(direction, index, col, game) == 0)
                        check_word_indicator_zero_if_false = 0;
                }
            }
            index++; // Increment index
        }
        if (row - 1 >= 0) { // Check upper
            if (game->counterarray[row - 1][col] != 0){
                adjcount++;
            }
        }
        if (row + size < game->rows) { // Check lower
            if (game->counterarray[row + size][col] != 0){
                adjcount++;
            }
        }
    }
    
    if (adjcount == 0)
        return 3; // Non-adjacent
    else if(check_word_indicator_zero_if_false == 0){
        return 2; //adjecent word isnt valid
    }
    else{
        return 0; // No error
    }
}

int check_word_OPPO(char direction, int row, int col, GameState *game){
    char word[game->rows*game->rowlen];
    int counter = 1, temprow = row, tempcol = col, index = 0;
    if(direction == 'V'){ // check opposite so hor

    while (tempcol - 1 >= 0 && game->counterarray[row][tempcol - 1] != 0) {
            tempcol--;
            counter++;
        }
    while (col + 1 < game->rowlen && game->counterarray[row][col + 1] != 0) {
            col++;
            counter++;
        }
    word[0] = '\0';

    for(int i = tempcol; i <= col; i++)
        word[index++] = game->array[temprow][i];

    word[index] = '\0';
    }
    else if(direction == 'H'){ // CHECK OPPOSITE!!! so vert

        while (temprow - 1 >= 0 && game->counterarray[temprow-1][col] != 0) {
            temprow--;
            counter++;
        }
    while (row + 1 < game->rows && game->counterarray[row+1][col] != 0) {
            row++;
            counter++;
        }

    word[0] = '\0';

    for(int i = temprow; i <= row; i++)
        word[index++] = game->array[i][tempcol];

    word[index] = '\0';
    }
    //printf("%s\n", word);
    if(check_word(word) == 1){
        return 1;                               // word is VALID
    } 
    else
        return 0;                               // word is invalid

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
    if  (file == NULL || word == NULL) {
        printf("word check error \n");
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
            fclose(file);
            return 1; // found
        }
    }
    fclose(file);
    return 0; // not found

}

GameState* gameextender(GameState *game) {
   GameState *newgame = malloc(sizeof(GameState));
    if (newgame == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    newgame->rows = game->rows;
    newgame->pastpointer = game;
    newgame->rowlen = game->rowlen;

    newgame->array = malloc(newgame->rows * sizeof(char *));
    newgame->counterarray = malloc(newgame->rows * sizeof(int *));
    if (newgame->array == NULL || newgame->counterarray == NULL) {
        printf("mem allocation fail\n");
        free(newgame->array);
        free(newgame->counterarray);
        free(newgame);
        return game;
    }

    for (int i = 0; i < newgame->rows; i++) {
        newgame->array[i] = malloc(newgame->rowlen * sizeof(char));
        newgame->counterarray[i] = malloc(newgame->rowlen * sizeof(int));
        if (newgame->array[i] == NULL || newgame->counterarray[i] == NULL) {
        printf("mem allocation fail\n");
            for (int j = 0; j < i; j++) {
                free(newgame->array[j]);
                free(newgame->counterarray[j]);
            }
            free(newgame->array);
            free(newgame->counterarray);
            free(newgame);
            return game;
        }
        memcpy(newgame->array[i], game->array[i], newgame->rowlen * sizeof(char));
        memcpy(newgame->counterarray[i], game->counterarray[i], newgame->rowlen * sizeof(int));
    }

    return newgame;
}

void array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index){ 
if (extend_num_of_rows_if_one == 1) { // Extend rows
    game->rows += inc_index; 

    char **temp_array = realloc(game->array, game->rows * sizeof(char *));
    int **temp_counter_array = realloc(game->counterarray, game->rows * sizeof(int *));

    if (temp_array == NULL || temp_counter_array == NULL) {
        printf("CRITICAL ERROR\n");
        return;
    } else {
        game->array = temp_array;
        game->counterarray = temp_counter_array;

        for (int i = game->rows - inc_index; i < game->rows; i++) {
            game->array[i] = malloc(game->rowlen * sizeof(char));
            game->counterarray[i] = malloc(game->rowlen * sizeof(int));
            for (int j = 0; j < game->rowlen; j++) {
                game->array[i][j] = '.';
                game->counterarray[i][j] = 0;
            }
        }
    }
} else { 
    game->rowlen += inc_index; // rowlen

    for (int i = 0; i < game->rows; i++) {
        char *temp_row_array = realloc(game->array[i], game->rowlen * sizeof(char));
        int *temp_row_counter_array = realloc(game->counterarray[i], game->rowlen * sizeof(int));

        if (temp_row_array == NULL || temp_row_counter_array == NULL) {
                    printf("CRITICAL ERROR\n");
                    return;
        } else {
            game->array[i] = temp_row_array;
            game->counterarray[i] = temp_row_counter_array;

            for (int j = game->rowlen - inc_index; j < game->rowlen; j++) {
                game->array[i][j] = '.';
                game->counterarray[i][j] = 0;
            }
        }
    }
}
}

GameState* undo_place_tiles(GameState *game) {//god help me
    if (game->pastpointer != NULL|| game == NULL) {
        for (int i = 0; i < game->rows; i++) {
            free(game->array[i]);
            free(game->counterarray[i]);
        }
        free(game->array);
        free(game->counterarray);
        GameState *state = game->pastpointer;
        free(game);
        return state;
    } else {
        return game; 
    }
}

void free_game_state(GameState *game) {
    if (game != NULL) {
        for (int i = 0; i < game->rows; i++) {
            free(game->array[i]);
            free(game->counterarray[i]);
        }
        free(game->array);
        free(game->counterarray);

        free_game_state(game->pastpointer);

        free(game);
    }
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