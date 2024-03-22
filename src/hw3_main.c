#include <stdio.h>
#include <stdlib.h>

#include "hw3.h"

int main(void) {
  
    GameState *game = initialize_game_state("./tests/boards/board01.txt");  

    for (int i = 0; i < game->rows; i++) {
        for (int j = 0; j < game->rowlen; j++) {
            printf("%c", game->array[i][j]);
        }
        printf("\n");
    }
  
  return 0;
}
