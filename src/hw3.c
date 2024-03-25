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

GameState* initialize_game_state(const char *filename) { // done!
    FILE *file = fopen(filename, "r");
    GameState *state = malloc(sizeof(GameState));

    state = malloc(sizeof(GameState));  //?

    state->pastpointer = NULL;

    if (file == NULL) {
        perror("failed to  open fiile");
        exit(EXIT_FAILURE);
    }

    char c;
    int index = 0, rowlen = 0, numofrows = 0;

    c = fgetc(file);
    while (c != EOF) {
        if(c == '\n' && rowlen == 0)
            rowlen = index;
        index++;
        c = fgetc(file);
    }

    numofrows = index / rowlen;

    if(rowlen == 0){
        perror("critical failure");
        exit(EXIT_FAILURE);
    }
    
    rewind(file);                       //rewind time

    state->rows = numofrows;
    state->rowlen = rowlen;

    state->array = malloc(numofrows * sizeof(char *));
    state->counterarray = calloc(numofrows, sizeof(int *));
    for (int i = 0; i < rowlen; i++) {                                      // malloc init for arrays
        state->array[i] = malloc(rowlen * sizeof(char));
        state->counterarray[i] = calloc(rowlen, sizeof(int));
    }

    int x = 0, y = 0;                                                       // add values to arrays
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            y = 0;
            x++;
        } else {
            state->array[x][y++] = c;
        }
    }

    for(int i = 0; i < state->rows; i++){
        for(int e = 0; e < state->rowlen; e++){
            if(state->array[i][e] != '.')
                state->counterarray[i][e]++;
        }
    }

    return state;
}

GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    long unsigned int counter = 0;
    int temprow = row, tempcol = col, index = 0, one_if_failure = 0;
    *num_tiles_placed = 0;
    char *word;

    if((row < 0) | (col < 0))
        return game;

    if(direction == 'V'){ //use rows varrible
        game = gameextender(game);

            if(game->rows < (int)(row+strlen(tiles))){ 
                game = array_extender(game, 1, ((row+(int)strlen(tiles)) - game->rows));
            }

        while(counter < strlen(tiles)){// this causes seg fault
            if(!(isspace(tiles[counter]))){
            game->array[row][col] = tiles[counter];
            game->counterarray[row][col]+= 1;

            if(game->counterarray[row][col] > 5){//checks for height greater than 5
                one_if_failure = 1;
                break;
            }

            *num_tiles_placed+=1;
            }
            counter++;
            row++;
        }
    
        word = malloc(counter * sizeof(char));

        while (temprow-1 > 0 && game->array[temprow-1][tempcol] != '.'){   //go back
            temprow--;
        }

        while((game->array[temprow][tempcol] != '.') && temprow < game->rows){
            word[index] = game->array[temprow][tempcol];
            index++;
            temprow++;
            if(temprow == game->rows)
            break;
        }

    }
    else if(direction == 'H'){               //use rowlen
        game = gameextender(game);
  
            if(game->rowlen < (int)(col+strlen(tiles))){
                game = array_extender(game, 0, ((col+(int)strlen(tiles)) - game->rowlen));
            }

         while(counter < strlen(tiles)){    
            if(!isspace(tiles[counter])){
            game->array[row][col] = tiles[counter];
            game->counterarray[row][col] += 1;

            if(game->counterarray[row][col] > 5){//checks for height greater than 5
                one_if_failure = 1;
                break;
            }

            *num_tiles_placed+=1;
            }
            counter++;
            col++;
        }         

        word = malloc(counter *sizeof(char));
        
        while (tempcol-1 > 0 && game->array[temprow][tempcol-1] != '.'){   //go back
            tempcol--;
        }

        while((game->array[temprow][tempcol] != '.') && tempcol < game->rowlen){
            word[index] = game->array[temprow][tempcol];
            tempcol++;
            index++;
            if(tempcol == game->rowlen)
                break;
        }   
    }

    if ((check_word(word) == 0) | (one_if_failure == 1)){ // if it is one it exists
       printf("\nNOT A WORD: %s, oneiffailure: %d\n", word, one_if_failure);
       *num_tiles_placed = 0;
       game = undo_place_tiles(game);
    }

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->rowlen; j++) {
            printf(" %c", game->array[i][j]);
        }
        printf("\n");
    }
            printf("\n");

    
    free(word);
    return game;
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

GameState* gameextender(GameState *game){ // maybe fixed the bug
    GameState *newgame = malloc(sizeof(GameState));
    newgame->pastpointer = malloc(sizeof(*game));
    newgame->pastpointer = game;

    newgame->rows = newgame->pastpointer->rows;
    newgame->rowlen = newgame->pastpointer->rowlen;

    newgame->array = malloc(newgame->rows * sizeof(char *));
    newgame->counterarray = calloc(newgame->rows, sizeof(int *));
    for (int i = 0; i < newgame->rows; i++) {                                      // malloc init for arrays
        newgame->array[i] = malloc(newgame->rowlen * sizeof(char));
        newgame->counterarray[i] = calloc(newgame->rowlen, sizeof(int));
    }

    for(int i = 0;i < newgame->rows; i++){
        for(int e= 0; e < newgame->rowlen; e++){
            newgame->counterarray[i][e] = game->counterarray[i][e]; //copies couter
            newgame->array[i][e] = game->array[i][e]; // copies arr
        }
    }

    return newgame; 
}

GameState* undo_place_tiles(GameState *game) {
    if(game->pastpointer != NULL){
    GameState *pointer = game->pastpointer;
    free(game);
    return pointer;
    }
    else{
        return game;
    }
}

GameState* array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index){ //resizes and puts in peroids this needs work (need to add in num array feture)
if(extend_num_of_rows_if_one == 1){    // extend the number of 'rows'
game->rows = game->rows+inc_index;  // increment value
game->array = realloc(game->array, game->rows* sizeof(char *));
game->counterarray = realloc(game->counterarray, game->rows* sizeof(int *));

for(int i = game->rows - inc_index; i < game->rows; i++){                            
    game->array[i] = realloc(game->array[i], game->rowlen * sizeof(char)); // reallloc data
    game->counterarray[i] = realloc(game->counterarray[i], game->rowlen * sizeof(int)); // reallloc data

}

for(int i = game->rows-inc_index; i < game->rows; i++){     //put values into new array slots
    for(int e = 0; e < game->rowlen; e++){
        game->array[i][e] = '.'; 
        game->counterarray[i][e] = 0; 
    }
}

}
else{                   // extend the length of 'rowlength'
game->rowlen = game->rowlen+inc_index;  // increment value

for(int i = 0; i < game->rows; i++){   //                         
    game->array[i] = realloc(game->array[i], game->rowlen * sizeof(char)); // reallloc data
    game->counterarray[i] = realloc(game->counterarray[i], game->rowlen * sizeof(int)); // reallloc data

}


for(int i = 0; i < game->rows; i++){     //put values into new array slots
    for(int e = game->rowlen - inc_index; e < game->rowlen; e++){
        game->array[i][e] = '.'; 
        game->counterarray[i][e] = 0;
    }
}

}

return game;
}

void free_game_state(GameState *game) {
    if (game == NULL)
        return;
    GameState *gamer = game;
    while(gamer->pastpointer != NULL) {

        gamer = gamer->pastpointer;
        free(gamer); 
    }
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
