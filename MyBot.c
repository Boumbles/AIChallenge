#include "ants.h"

// returns the absolute value of a number; used in distance function

int abs(int x) {
    if (x >= 0)
        return x;
    return -x;
}

// returns the distance between two items on the grid accounting for map wrapping

// int distance(int row1, int col1, int row2, int col2, struct game_info *Info) {
    // int dr, dc;
    // int abs1, abs2;

    // abs1 = abs(row1 - row2);
    // abs2 = Info->rows - abs(row1 - row2);

    // if (abs1 > abs2)
        // dr = abs2;
    // else
        // dr = abs1;

    // abs1 = abs(col1 - col2);
    // abs2 = Info->cols - abs(col1 - col2);

    // if (abs1 > abs2)
        // dc = abs2;
    // else
        // dc = abs1;

    // return sqrt(pow(dr, 2) + pow(dc, 2));
// }
// sends a move to the tournament engine and keeps track of ants new location

void move(int index, char dir, struct game_state* Game, struct game_info* Info) {
    fprintf(stdout, "O %i %i %c\n", Game->my_ants[index].row, Game->my_ants[index].col, dir);
                 
   // fprintf(stderr, "move() for Index %d in direction %c\n", index, dir); 
    //fprintf(stderr, "PREVIOUS POS IS %i %i for i= %i Prior to the move\n", Game->my_ants[index].prevrow, Game->my_ants[index].prevcol, index);
    //fflush(stderr);
    switch (dir) {
        case 'N':
            if (Game->my_ants[index].row != 0)
                Game->my_ants[index].row -= 1;
            else
                Game->my_ants[index].row = Info->rows - 1;
            break;
        case 'E':
            if (Game->my_ants[index].col != Info->cols - 1)
                Game->my_ants[index].col += 1;
            else
                Game->my_ants[index].col = 0;
            break;
        case 'S':
            if (Game->my_ants[index].row != Info->rows - 1)
                Game->my_ants[index].row += 1;
            else
                Game->my_ants[index].row = 0;
            break;
        case 'W':
            if (Game->my_ants[index].col != 0)
                Game->my_ants[index].col -= 1;
            else
                Game->my_ants[index].col = Info->cols - 1;
            break;
    }
    // fprintf(stderr, 
			// "PostMove.Index %d PrevPos : %d and current pos : %d \n", 
			// index, 
			// Game->my_ants[index].prevrow*Info->cols + Game->my_ants[index].prevcol,
			// Game->my_ants[index].row*Info->cols + Game->my_ants[index].col);
	// fflush(stderr);    
}

// just a function that returns the string on a given line for i/o
// you don't need to worry about this

char *get_line(char *text) {
    char *tmp_ptr = text;
    int len = 0;

    while (*tmp_ptr != '\n') {
        ++tmp_ptr;
        ++len;
    }

    char *return_str = malloc(len + 1);
    memset(return_str, 0, len + 1);

    int i = 0;
    for (; i < len; ++i) {
        return_str[i] = text[i];
    }

    return return_str;
}

// main, communicates with tournament engine

int main(int argc, char *argv[]) {
    int action = -1;

    struct game_info Info;
    struct game_state Game;
    
    Info.map = 0;
    Info.scores = 0;
    
    Game.my_ants = 0;
    Game.enemy_ants = 0;
    Game.food = 0;
    Game.dead_ants = 0;
    Game.debugging = 1;
    Game.hill = 0;
	//freopen( "stderr.log", "w", stderr );
	while (42) {
		
        int initial_buffer = 100000;

        char *data = malloc(initial_buffer);
        memset(data, 0, initial_buffer);

        *data = '\n';

        char *ins_data = data + 1;

        int i = 0;

        while (1 > 0) {
            ++i;

            if (i >= initial_buffer) {
                initial_buffer *= 2;
                data = realloc(data, initial_buffer);
                ins_data = data + i;
                memset(ins_data, 0, initial_buffer - i);
            }

            *ins_data = getchar();

            if (*ins_data == '\n') {
                char *backup = ins_data;

                while (*(backup - 1) != '\n') {
                    --backup;
                }

                char *test_cmd = get_line(backup);

                if (strcmp(test_cmd, "go") == 0) {//this means that we are to start moving
					//fprintf(stderr, "Let's start antsing\n");
                    action = 0; 
                    free(test_cmd);
                    break;
                } else if (strcmp(test_cmd, "ready") == 0) { //if we receive this then we are allowed to setup
					//fprintf(stderr, "setup time\n");
					action = 1;
                    free(test_cmd);
                    break;
                } else if (test_cmd) free(test_cmd);
            }
            //fflush(stderr);
            ++ins_data;
        }

        if (action == 0) {//movement
            char *skip_line = data + 1;
            while (*++skip_line != '\n');
            ++skip_line;
			
            _init_map(skip_line, &Info);
			//spitmap(&Info); 
			//spitscores(&Info);
            _init_game(&Info, &Game);
            
            do_turn(&Game, &Info);
            fprintf(stdout, "go\n");
            fflush(stdout);            
        }
        else if (action == 1) {//setup
            _init_ants(data + 1, &Info);            
            Game.my_ant_index = -1;

            fprintf(stdout, "go\n");
            fflush(stdout);
        }
		//fflush(stderr);
        if(data) free(data);
    }
}
