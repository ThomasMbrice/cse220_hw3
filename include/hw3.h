#include <stdlib.h>

typedef struct GameState{
    char **array;
    int **counterarray;
    int rows;
    int rowlen;
    struct GameState *pastpointer;
} GameState;
/*
int check_word(char *word);
int check_for_2_letter(GameState *game);
int check_word_OPPO(char direction, int row, int col, GameState *game);
int oppo_check(int row, int col, int size, GameState *game, char direction);
GameState* gameextender(GameState *game);
void array_extender(GameState *game, int extend_num_of_rows_if_one, int inc_index);
*/
GameState* place_tiles(GameState *game, int row, int col, char direction, const char *tiles, int *num_tiles_placed);
GameState* initialize_game_state(const char *filename);
GameState* undo_place_tiles(GameState *game);
void free_game_state(GameState *game);
void save_game_state(GameState *game, const char *filename);