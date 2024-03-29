#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
  int num_tiles_placed = 0;
	GameState *game = initialize_game_state("./tests/boards/board01.txt"); 
    game = place_tiles(game, 3, 2, 'H', "C", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);


    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->rowlen; j++) {
            printf(" %c", game->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    game = undo_place_tiles(game);
    

  return 0;
}
