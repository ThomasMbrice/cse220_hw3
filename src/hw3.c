#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "hw3.h" 

#define DEBUG(...) fprintf(stderr, "[          ] [ DEBUG ] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, " -- %s()\n", __func__)
/*
typedef struct GameState {
    char **array;
    int rows;
    int rowlen;
} GameState;
*/
GameState* initialize_game_state(const char *filename) { // done!
    FILE *file = fopen(filename, "r");
    GameState *state = malloc(sizeof(GameState));

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
    for (int i = 0; i < rowlen; i++) {
        state->array[i] = malloc(rowlen * sizeof(char));
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

    return state;
}


GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed) {
    (void)game;
    (void)row;
    (void)col;
    (void)direction;
    (void)tiles;
    (void)num_tiles_placed;
    return NULL;
}

GameState* undo_place_tiles(GameState *game) {
    (void)game;
    return NULL;
}

void free_game_state(GameState *game) {
    if (game == NULL)
        return;
    if (game->array != NULL) {
        for (int i = 0; i < game->rows; i++) 
            free(game->array[i]);
        free(game->array);
    }
    free(game);
}


void save_game_state(GameState *game, const char *filename) { //done?
    FILE *file = fopen(filename, "w");

    for(int i = 0; i < game->rows;i++){
        for(int e= 0; e < game->rowlen; e++){
            fprintf(file, " %c", game->array[i][e]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}
