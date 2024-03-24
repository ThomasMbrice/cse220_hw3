#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
      
  int num_tiles_placed = 0;
   GameState *game = initialize_game_state("./tests/boards/board01.txt"); 
    game = place_tiles(game, 2, 3, 'V', "T PMAN", &num_tiles_placed);
    game = place_tiles(game, 2, 5, 'V', "P TAL", &num_tiles_placed);
    game = place_tiles(game, 6, 1, 'H', "SN I", &num_tiles_placed);

    for (int i = 0; i < game->arr[game->currentindex].rows; i++) {
        for (int j = 0; j < game->arr[game->currentindex].rowlen; j++) {
            printf(" %c", game->arr[game->currentindex].array[i][j]);
        }
        printf("\n");
    }

        printf("\n");

    for (int i = 0; i < game->arr[game->currentindex].rows; i++) {
        for (int j = 0; j < game->arr[game->currentindex].rowlen; j++) {
            printf(" %d", game->arr[game->currentindex].counterarray[i][j]);
        }
        printf("\n");
    }
    printf("\n num tiles placed %d \n", num_tiles_placed);
  
  return 0;
}
