#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
/*
typedef struct GameState {
    ArrayofArrays *arr;
    int currentindex;
} GameState;
*/
/*
typedef struct ArrayofArrays {
    char **array;
    int **counterarray;
    int rows;
    int rowlen;
} ArrayofArrays;
*/

// TODO CREATE A PREVOUS SAVE ARRAY FOR UNDO FUNCTION
// will probobly have to redo entire thing with the fact that the prevois versions and 
//int versions need to be stored in a data structure with each version in parrallel nned to delete versions if going back

GameState* initialize_game_state(const char *filename) { // done!
    FILE *file = fopen(filename, "r");
    GameState *state = malloc(sizeof(GameState));

    state->arr = malloc(sizeof(ArrayofArrays));  //?

    state->currentindex = 0;

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

    state->arr[state->currentindex].rows = numofrows;
    state->arr[state->currentindex].rowlen = rowlen;

    state->arr[state->currentindex].array = malloc(numofrows * sizeof(char *));
    state->arr[state->currentindex].counterarray = calloc(numofrows, sizeof(int *));
    for (int i = 0; i < rowlen; i++) {                                      // malloc init for arrays
        state->arr[state->currentindex].array[i] = malloc(rowlen * sizeof(char));
        state->arr[state->currentindex].counterarray[i] = calloc(rowlen, sizeof(int));
    }

    int x = 0, y = 0;                                                       // add values to arrays
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            y = 0;
            x++;
        } else {
            state->arr[state->currentindex].array[x][y++] = c;
        }
    }

    for(int i = 0; i < state->arr[state->currentindex].rows; i++){
        for(int e = 0; e < state->arr[state->currentindex].rowlen; e++){
            if(state->arr[state->currentindex].array[i][e] != '.')
                state->arr[state->currentindex].counterarray[i][e]++;
        }
    }

    return state;
}


GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    long unsigned int counter = 0;
    int temprow = row, tempcol = col, word_exists_if_one = 0, index = 0;
    char *word;
    char line[1024];
    FILE *file = fopen("./tests/words.txt", "r");

    if (file == NULL) {
        perror("error");
    }

    if((row < 0) | (col < 0))
        return game;

    if(direction == 'V'){ //use rows varrible
        game = gameextender(game);
            if(game->arr[game->currentindex].rowlen < (int)(row+strlen(tiles))){
                printf("extension called \n");
            game = array_extender(game, 1, (int)(game->arr[(col+strlen(tiles))-game->currentindex].rowlen));
            }
        
        while(counter < strlen(tiles)){
            if(tiles[counter] != ' '){
            game->arr[game->currentindex].array[row][col] = tiles[counter];
            game->arr[game->currentindex].counterarray[row++][col]+= 1;
            *num_tiles_placed+=1;
            }
            counter++;
        }
        word = malloc(counter *sizeof(char));
        
        while((game->arr[game->currentindex].array[temprow][tempcol] != '.') && index < game->arr[game->currentindex].rows){
            word[index++] = game->arr[game->currentindex].array[temprow++][tempcol];
        }    
    }
    else if(direction == 'H'){               //use rowlen
        game = gameextender(game);
  
            if(game->arr[game->currentindex].rowlen < (int)(col+strlen(tiles))){
                //printf("extension called %d \n", ((col+(int)strlen(tiles)) - game->arr[game->currentindex].rowlen));
                game = array_extender(game, 0, ((col+(int)strlen(tiles)) - game->arr[game->currentindex].rowlen));
            }

         while(counter < strlen(tiles)){
            if(tiles[counter] != ' '){
            game->arr[game->currentindex].array[row][col] = tiles[counter];
            game->arr[game->currentindex].counterarray[row][col++] += 1;
            *num_tiles_placed+=1;
            }
            counter++;
        }         


        word = malloc(counter *sizeof(char));
        
        while((game->arr[game->currentindex].array[temprow][tempcol] != '.') && index < game->arr[game->currentindex].rowlen){
            word[index++] = game->arr[game->currentindex].array[temprow][tempcol++];
        }   

    }

    
    while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';
    if (strcasecmp(word, line) == 0) {      // thismight be wronge?
        word_exists_if_one = 1;
        break;
    }
}   
    

    if (word_exists_if_one != 1){
        printf("no!");
       game = undo_place_tiles(game);
    }
    
    free(word);
    fclose(file);
    return game;
}

GameState* gameextender(GameState *game){ // maybe fixed the bug
    game->arr = realloc(game->arr, ((game->currentindex+2) * 2 * 
    game->arr[game->currentindex].rowlen * game->arr[game->currentindex].rows)); //realloc arr for next entry  
    //+2 for the next and first (becuase we start counting at 0) and *2 because there are 2 arrays with theese dimensions 
    game->currentindex +=1;
    game->arr[game->currentindex].rows = game->arr[game->currentindex-1].rows;
    game->arr[game->currentindex].rowlen = game->arr[game->currentindex-1].rowlen;

    game->arr[game->currentindex].array = malloc(game->arr[game->currentindex].rows * sizeof(char *));
    game->arr[game->currentindex].counterarray = calloc(game->arr[game->currentindex].rows, sizeof(int *));
    for (int i = 0; i < game->arr[game->currentindex].rows; i++) {                                      // malloc init for arrays
        game->arr[game->currentindex].array[i] = malloc(game->arr[game->currentindex].rowlen * sizeof(char));
        game->arr[game->currentindex].counterarray[i] = calloc(game->arr[game->currentindex].rowlen, sizeof(int));
    }


    for(int i = 0;i < game->arr[game->currentindex].rows; i++){
        for(int e= 0; e < game->arr[game->currentindex].rowlen; e++){
            game->arr[game->currentindex].counterarray[i][e] = game->arr[game->currentindex-1].counterarray[i][e]; //copies couter
            game->arr[game->currentindex].array[i][e] = game->arr[game->currentindex-1].array[i][e]; // copies arr
        }
    }

    return game; 
}


GameState* undo_place_tiles(GameState *game) {
    if(game->currentindex > 0){
    free(game->arr[game->currentindex].counterarray);
    free(game->arr[game->currentindex].array);
    game->currentindex--;
    return game;
    }
    else{
        return game;
    }
}

GameState* array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index){ //resizes and puts in peroids this needs work (need to add in num array feture)
if(extend_num_of_rows_if_one == 1){    // extend the number of 'rows'
game->arr[game->currentindex].rows = game->arr[game->currentindex].rows+inc_index;  // increment value
game->arr[game->currentindex].array = realloc(game->arr[game->currentindex].array, game->arr[game->currentindex].rows* sizeof(char *));
game->arr[game->currentindex].counterarray = realloc(game->arr[game->currentindex].counterarray, game->arr[game->currentindex].rows* sizeof(int *));

for(int i = game->arr[game->currentindex].rows-inc_index; i < game->arr[game->currentindex].rows; i++){     //put values into new array slots
    for(int e = 0; e < game->arr[game->currentindex].rowlen; e++){
        game->arr[game->currentindex].array[i][e] = '.'; 
        game->arr[game->currentindex].counterarray[i][e] = 0; 
    }
}

}
else{                   // extend the length of 'rowlength'
game->arr[game->currentindex].rowlen = game->arr[game->currentindex].rowlen+inc_index;  // increment value

for(int i = 0; i < game->arr[game->currentindex].rows; i++){                            
    game->arr[game->currentindex].array[i] = realloc(game->arr[game->currentindex].array[i], game->arr[game->currentindex].rowlen * sizeof(char)); // reallloc data
    game->arr[game->currentindex].counterarray[i] = realloc(game->arr[game->currentindex].counterarray[i], game->arr[game->currentindex].rowlen * sizeof(int)); // reallloc data

}


for(int i = 0; i < game->arr[game->currentindex].rows; i++){     //put values into new array slots
    for(int e = game->arr[game->currentindex].rowlen - inc_index; e < game->arr[game->currentindex].rowlen; e++){
        game->arr[game->currentindex].array[i][e] = '.'; 
        game->arr[game->currentindex].counterarray[i][e] = 0;
    }
}

}

return game;
}

void free_game_state(GameState *game) {//?
    
    if (game == NULL)
        return;
    if (game->arr->array != NULL) {
        //for (int i = 0; i < game->rows; i++){
            //free(game->arr[game->currentindex].array[i]);
            //free(game->arr[game->currentindex].counterarray[i]);
        //}
        //free(game->arr->array);
        //free(game->arr->counterarray);
    }
    //free(game->arr);
    free(game);

}

void save_game_state(GameState *game, const char *filename) { //done?
    FILE *file = fopen(filename, "w");

    for(int i = 0; i < game->arr[game->currentindex].rows;i++){         // actual characters
        for(int e= 0; e < game->arr[game->currentindex].rowlen; e++){
            fprintf(file, "%c ", game->arr[game->currentindex].array[i][e]);
        }
        fprintf(file, " \n");
    }

    for(int i = 0; i < game->arr[game->currentindex].rows;i++){        // counter under the characters
        for(int e= 0; e < game->arr[game->currentindex].rowlen; e++){
            fprintf(file, "%d ", game->arr[game->currentindex].counterarray[i][e]);
        }
        fprintf(file, "\n ");
    }
    fclose(file);
}
