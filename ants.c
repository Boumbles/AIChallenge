#include "ants.h"

// initializes the game_info structure on the very first turn
// function is not called after the game has started

void _init_ants(char *data, struct game_info *game_info) {
    char *replace_data = data;
    
    while (*replace_data != '\0') {
        if (*replace_data == '\n')
            *replace_data = '\0';
        ++replace_data;
    }

    while (42) {
        char *value = data;

        while (*++value != ' ');
        ++value;

        int num_value = atoi(value);

	    switch (*data) {
            case 'l':
                game_info->loadtime = num_value;
                break;

            case 't':
                if (*(data + 4) == 't')
                    game_info->turntime = num_value;
                else
                    game_info->turns = num_value;
                break;

            case 'r':
                game_info->rows = num_value;
                break;

            case 'c':
                game_info->cols = num_value;
                break;
                        
            case 'v':
                game_info->viewradius_sq = num_value;
                break;

            case 'a':
                game_info->attackradius_sq = num_value;
                break;
                    
            case 's':
                if (*(data + 1) == 'p')
                    game_info->spawnradius_sq = num_value;
                else
                    game_info->seed = num_value;
                break;

        }

        data = value;
        
        while (*++data != '\0');
        ++data;
        
        if (strcmp(data, "ready") == 0)
            break;
    }
    
}

// updates game data with locations of ants and food
// only the ids of your ants are preserved

void _init_game(struct game_info *game_info, struct game_state *game_state) {
    int map_len = game_info->rows*game_info->cols;

    int my_count = 0;
    int enemy_count = 0;
    int food_count = 0;
    int dead_count = 0;
    int hill_count = 0;
    int i, j;

    for (i = 0; i < map_len; ++i) {
        char current = game_info->map[i];

        if (current == '?' || current == '.' || current == '%')
            continue;
        else if (current == '*')
            ++food_count;
        else if (current == 'a')
            ++my_count;
        else if (current == 'A') {
            ++my_count;
            ++hill_count;
        }
        else if (isdigit(current))
            ++hill_count;
        else if (current & 0x80)
            ++dead_count;
        else if (isupper(current)){
            ++hill_count;
            ++enemy_count;
        }		
        else
            ++enemy_count;
    }

    struct my_ant *my_old = 0;
    int my_old_count = game_state->my_count;

    game_state->my_count = my_count;
    game_state->enemy_count = enemy_count;
    game_state->food_count = food_count;
    game_state->dead_count = dead_count;
    game_state->hill_count = hill_count;

    if (game_state->my_ants != 0)
        my_old = game_state->my_ants;

    if (game_state->enemy_ants != 0)
        free(game_state->enemy_ants);
    if (game_state->food != 0)
        free(game_state->food);
    if (game_state->dead_ants != 0)
        free(game_state->dead_ants);

    if (game_state->hill != 0)
        free(game_state->hill);

    game_state->my_ants = malloc(my_count*sizeof(struct my_ant));
    
	//game_info->squares = malloc(map_len*sizeof(struct square));
    
    if (enemy_count > 0)
        game_state->enemy_ants = malloc(enemy_count*sizeof(struct basic_ant));
    else
        game_state->enemy_ants = 0;
	
    if (dead_count > 0)
        game_state->dead_ants = malloc(dead_count*sizeof(struct basic_ant));
    else
        game_state->dead_ants = 0;

    if (hill_count > 0)
        game_state->hill = malloc(hill_count*sizeof(struct hill));
    else
        game_state->hill = 0;

    if (food_count > 0)
        game_state->food = malloc(food_count*sizeof(struct food));
    else
        game_state->food = 0;

    int my_count_start = my_count;

    for (i = 0; i < game_info->rows; ++i) {
        for (j = 0; j < game_info->cols; ++j) {
            char current = game_info->map[game_info->cols*i + j];
            if (current == '?' || current == '.' || current == '%')
                 continue;

            if (current == '*') {
                --food_count;

                game_state->food[food_count].row = i;
                game_state->food[food_count].col = j;
            } else if (isdigit(current)) {
                --hill_count;

                game_state->hill[hill_count].row = i;
                game_state->hill[hill_count].col = j;
                game_state->hill[hill_count].player = current - '0';
            } else if (current == 'a' || current == 'A') {
                if (isupper(current)) {
                    --hill_count;

                    game_state->hill[hill_count].row = i;
                    game_state->hill[hill_count].col = j;
                    game_state->hill[hill_count].player = current - 'A';
                }
                --my_count;

                int keep_id = -1;
                int k = 0;

                if (my_old != 0) {
                    for (; k < my_old_count; ++k) {
                        if (my_old[k].row == i && my_old[k].col == j) {
                            keep_id = my_old[k].id;
                            break;
                        }
                    }
                }

                game_state->my_ants[my_count].row = i;
                game_state->my_ants[my_count].col = j;
				
                if (keep_id == -1)
                    game_state->my_ants[my_count].id = ++game_state->my_ant_index;
                else
                    game_state->my_ants[my_count].id = keep_id;
            } else if (current & 0x80) {
                --dead_count;

                game_state->dead_ants[dead_count].row = i;
                game_state->dead_ants[dead_count].col = j;
                game_state->dead_ants[dead_count].player = (current & (~0x80));
                
                game_info->map[game_info->cols*i + j] = '!';
            } else if (isupper(current)) {
                --hill_count;

                game_state->hill[hill_count].row = i;
                game_state->hill[hill_count].col = j;
                game_state->hill[hill_count].player = current - 'A';
                --enemy_count;

                game_state->enemy_ants[enemy_count].row = i;
                game_state->enemy_ants[enemy_count].col = j;
                game_state->enemy_ants[enemy_count].player = current - 'A';

            } else {
                --enemy_count;

                game_state->enemy_ants[enemy_count].row = i;
                game_state->enemy_ants[enemy_count].col = j;
                game_state->enemy_ants[enemy_count].player = current - 'a';
            } 
        }
    }
	determinediffusionscores(game_info);
    if (my_old != 0)
        free(my_old);
}

// Updates the map.
//
//    %   = Walls       (the official spec calls this water,
//                      in either case it's simply space that is occupied)
//    .   = Land        (territory that you can walk on)
//    a   = Your Ant
// [b..z] = Enemy Ants
//    !   = Dead Ants   (disappear after one turn)
//    *   = Food
// [A..Z] = hill + ant
// [0..9] = hill
//    ?   = Unknown     (not used in latest engine version, unknowns are assumed to be land)


void _init_map(char *data, struct game_info *game_info) {
	fprintf(stderr, "game_info->map: %s\n", game_info->map);
    if (game_info->map == 0) {
        game_info->map = malloc(game_info->rows*game_info->cols);
        memset(game_info->map, '.', game_info->rows*game_info->cols);
        fprintf(stderr, "allocated map\n");
		fflush(stderr);
    } 
    fprintf(stderr, "game_info->scores : %d\n", game_info->scores);
    fflush(stderr);
	if (game_info->scores == 0 ){
		game_info->scores = malloc(game_info->rows*game_info->cols * sizeof(*game_info->scores));
		memset(game_info->scores, -1, game_info->rows*game_info->cols*sizeof(int));
		fprintf(stderr, "allocated scores\n");
		fflush(stderr);
	}
	
	fprintf(stderr, "Memory allocated. size of map : %d, size of scores : %d\n", 
			sizeof(game_info->map)/sizeof(char), sizeof(game_info->scores)/sizeof(int));
	fprintf(stderr, "rows : %d , columns : %d\n", game_info->rows, game_info->cols);
	fflush(stderr);
    int map_len = game_info->rows*game_info->cols;
    int i = 0;

    for (; i < map_len; ++i)
        if (game_info->map[i] != '%')
            game_info->map[i] = '.';

    while (*data != 0) {
        char *tmp_data = data;
        int arg = 0;
		// fprintf(stderr, "tmp_data :%s \n", tmp_data);
        while (*tmp_data != '\n') {
            if (*tmp_data == ' ') {
                *tmp_data = '\0';
                ++arg;
            }
            ++tmp_data;
        }

        char *tmp_ptr = tmp_data;
        tmp_data = data;

        tmp_data += 2;
        int jump = strlen(tmp_data) + 1;

        int row = atoi(tmp_data);
        int col = atoi(tmp_data + jump);
        char var3 = -1;
		
        if (arg > 2) {
            jump += strlen(tmp_data + jump) + 1;
            var3 = *(tmp_data + jump);
        }

        int offset = row*game_info->cols + col;
		// fprintf(stderr, "setting map spaces row : %d, column : %d, offset : %d\n", 
				// row, col, offset);
		// fprintf(stderr, "map at offset: %c\n", game_info->map[offset]);
		// fprintf(stderr, "data: %c\n", *data);
		// fflush(stderr);
		
        switch (*data) {
            case 'w':
                game_info->map[offset] = '%';
				game_info->scores[offset] = 0;
                break;
            case 'a':
                if (isdigit(game_info->map[offset]))
                    game_info->map[offset] = var3 - '0' + 'A';
                else
                   game_info->map[offset] = var3 - '0' + 'a';
                break;
            case 'd':
                game_info->map[offset] = (((unsigned char) (var3 - '0')) + 0x80);
                break;
            case 'f':
                game_info->map[offset] = '*';
				game_info->scores[offset] = DIFFUSION_FOOD;
                break;
            case 'h':
                game_info->map[offset] = var3;
				game_info->scores[offset] = DIFFUSION_HILL;
                break;
            default: 
				game_info->map[offset] = '.';
                break;
        }
		// fprintf(stderr, "%c,",game_info->map[offset]);
		// fflush(stderr);
        data = tmp_ptr + 1;
    }
	fprintf(stderr, "Done setting up map\n");
	fflush(stderr);
	  
}
void spitmap(struct game_info *Info){
	int rowcount = 0;
	int i, j;
	fprintf(stderr, "printing map\n");
	fprintf(stderr, "rows %d columns %d\n", Info->rows, Info->cols);
	fflush(stderr);
	for(i = 0; i < Info->rows; ++i){
		for(j = 0; j < Info->cols; ++j){
			int index = i*Info->cols + j;
			// fprintf(stderr, "index: %d ", index);
			// fflush(stderr);
			
			fprintf(stderr, "%c",Info->map[index]);
			fflush(stderr);
			++rowcount;
			if(rowcount >= Info->cols){
				rowcount = 0;
				fprintf(stderr, "\n");
				fflush(stderr);
			}			
		}
	}
}
void spitscores(struct game_info *Info){
	int rowcount = 0;
	int i, j;
	fprintf(stderr, "printing scores\n");
	fprintf(stderr, "rows %d columns %d\n", Info->rows, Info->cols);
	fflush(stderr);
	for(i = 0; i < Info->rows; ++i){
		for(j = 0; j < Info->cols; ++j){
			int index = i*Info->cols + j;
			// fprintf(stderr, "index: %d ", index);
			// fflush(stderr);
			
			fprintf(stderr, "%d",Info->scores[index]);
			fflush(stderr);
			++rowcount;
			if(rowcount >= Info->cols){
				rowcount = 0;
				fprintf(stderr, "\n");
				fflush(stderr);
			}			
		}
	}
}