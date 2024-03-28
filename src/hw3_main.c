#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
  int num_tiles_placed = 0;
	GameState *game = initialize_game_state("./tests/boards/board02.txt");
    game = place_tiles(game, 4, 5, 'H', "BRAMBLEBERRY", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = undo_place_tiles(game);
    game = place_tiles(game, 4, 5, 'V', "BRAMBLEBERRY", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 5, 2, 'H', "BEA ABLE", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 5, 1, 'H', "CL", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 5, 6, 'H', "ANCE", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 5, 7, 'H', "NC", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 3, 8, 'V', "EX ELLENCE", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 10, 3, 'H', "AB RU CATOR", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 8, 3, 'H', "EM EL ISHMENT", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = undo_place_tiles(game);
    game = place_tiles(game, 8, 3, 'H', "EM AL ONURIDAE", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    game = place_tiles(game, 5, 1, 'H', "AB", &num_tiles_placed);
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
