#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <ctype.h>

// this header is basically self-documenting
#define DIFFUSION_FOOD 	500
#define DIFFUSION_HILL 1000
#define DIFFUSION_DECREMENT 50

struct game_info {
	int loadtime;
	int turntime;
	int rows;
	int cols;
	int turns;
	int viewradius_sq;
	int attackradius_sq;
	int spawnradius_sq;
    int seed;
	char *map;
	int *scores;
	//struct square *squares;
};

struct basic_ant {
    int row;
    int col;
	int prevrow;
	int precol;	
    char player;
};

struct my_ant {
    int id;
    int row;
    int col;
	
	int prevrow;
	int prevcol;
    int movecounter;
	int destrow;
	int destcol;
	char destobject;
	char currdirection;
	int currdestination;
	char *directions;
};
struct square{	
	int score;
};

struct food {
    int row;
    int col;
};

struct hill {
    int row;
    int col;
    char player;
};


struct game_state {
    struct my_ant *my_ants;
    struct basic_ant *enemy_ants;
    struct food *food;
    struct basic_ant *dead_ants;
	struct waterspot *waterspots;
	
    int debugging;
    
	struct hill *hill;
    
    int my_count;
    int enemy_count;
    int food_count;
    int dead_count;

	FILE *logfile;
	
    int hill_count;

    int my_ant_index;
};
void spitmap(struct game_info *Info);
void spitscores(struct game_info *Info);