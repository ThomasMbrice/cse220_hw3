#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
  int num_tiles_placed = 0;
	  GameState *game = initialize_game_state("./tests/boards/board04.txt");
    game = place_tiles(game, 5, 19, 'V', "TREAT", &num_tiles_placed);
    printf("%d\n", num_tiles_placed);
    
    save_game_state(game, "./src/testor.txt");

    free_game_state(game);

  return 0;
}
