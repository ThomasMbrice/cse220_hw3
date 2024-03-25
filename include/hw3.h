#include <stdlib.h>

typedef struct ArrayofArrays {
    char **array;
    int **counterarray;
    int rows;
    int rowlen;
} ArrayofArrays;

typedef struct GameState{
    ArrayofArrays *arr;
    int currentindex;
} GameState;

int check_word(char *word);
GameState* initialize_game_state(const char *filename);
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* undo_place_tiles(GameState *game);
GameState* gameextender(GameState *game);
GameState* array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);


