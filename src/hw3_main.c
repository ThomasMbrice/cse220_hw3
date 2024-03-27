#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
  int num_tiles_placed = 0;
    GameState *game = initialize_game_state("./tests/boards/board04.txt");
        printf("%d\n row: %d col: %d\n", num_tiles_placed, game->rows, game->rowlen);
    game = place_tiles(game, 4, 19, 'H', "INSERT", &num_tiles_placed);
    printf("%d\n row: %d col: %d", num_tiles_placed, game->rows, game->rowlen);

    free_game_state(game);

  return 0;
}
