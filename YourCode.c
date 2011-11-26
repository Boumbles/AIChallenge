#include "ants.h"
#include "YourCode.h"
// The following is an example program displaying a basic ai that
// tries all possible directions and then moves in whichever one
// is not blocked by water (% characters).
//
// To see all the information contained in struct game_state and
// struct game_info check ants.h. There is also a distance function
// provided in bot.c
void updateprevious(struct game_state *Game, struct game_info *Info, int index){
	OpenLog(Game);
	fprintf(Game->logfile, "updateprevious()");
	Game->my_ants[index].prevrow = Game->my_ants[index].row;
	Game->my_ants[index].prevcol = Game->my_ants[index].col;
	CloseLog(Game);
}

void do_turn(struct game_state *Game, struct game_info *Info) {
    int i,j;
	OpenLog(Game);
    fprintf(Game->logfile, "\nCount is : %d\n", Game->my_count);
	if(Info->map){
		fprintf(Game->logfile, "Size of map is %d\n", strlen(Info->map));
		for (j = 0; j < strlen(Info->map); ++j){
			fprintf(Game->logfile, "Object on the map at %d is a %s\r", j, Info->map[j]);
		}
	}
	
	
    for (i = 0; i < Game->my_count; ++i) {        
        // the location within the map array where our ant is currently
        int offset = Game->my_ants[i].row*Info->cols + Game->my_ants[i].col;
        fprintf(Game->logfile, "\noffset is: %d \n", offset);
                
        char dir = -1;
		
		dir = makeaturn(Game, Info, i, offset);
		if(dir != -1){
			updateprevious(Game, Info, i);
			move(i, dir, Game, Info);		
		}
    }
    CloseLog(Game);
}



/*
 * Checks that we are not running into one of our own O(N)
 * */
int notanant(struct game_state *Game, struct game_info *Info, int origoffset, int destination){
	int i;
	OpenLog(Game);
	fprintf(Game->logfile, "notanant())");
	for(i = 0; i < Game->my_count; ++i){
        int compareoffset = Game->my_ants[i].row*Info->cols + Game->my_ants[i].col;
		fprintf(Game->logfile, "compareoffset = %d ; destination = %d\n", compareoffset, destination);
        if(origoffset != compareoffset){
            if(compareoffset == destination){
                CloseLog(Game);
				return 0;
				
			}
		}
		
	}
	CloseLog(Game);
	return 1;
}
/*
 * Returns 1 if the and just came from where he wants to go
 * */
int previouspos(struct game_state *Game, struct game_info *Info, int index, int newpos){
    OpenLog(Game);
    fprintf(Game->logfile, "npreviouspos())Checking previous position for index %d :  currentpos %d prepos %d newpos of %d \n", 
            index,
            Game->my_ants[index].row*Info->cols + Game->my_ants[index].col,
            Game->my_ants[index].prevrow*Info->cols + Game->my_ants[index].prevcol,
            newpos);
    
    if((Game->my_ants[index].prevrow*Info->cols + Game->my_ants[index].prevcol) == newpos){
		fprintf(Game->logfile, "Same place\n");
        CloseLog(Game);
        return 1;
	}
	fprintf(Game->logfile, "Different locations\n");
    CloseLog(Game);
    return 0;
}
/*
 * This function decrements our direction counter and forces a direction change if hte ant has been going in the same direction too long
 * */
int handlecounterandchangedirectionifnecessary(struct game_state *Game, struct game_info *Info, int index){
 Game->my_ants[index].movecounter--;
 if(Game->my_ants[index].movecounter==0){
  Game->my_ants[index].movecounter = 5;
  return -1;
 } 
 return 0;
}
/*
 * This function validates a potential direction to go in
 * */
int validateturn(struct game_state *Game, struct game_info *Info, int index, int offset, char obj, int directionasnumber, char direction){
    OpenLog(Game);
    fprintf(Game->logfile, "\nvalidate turn offset %d directionasnumber %d index %d \n", offset, directionasnumber, index);
    fprintf(Game->logfile, "the object is a %c \n", obj);
    CloseLog(Game);
	if(obj != '%' &&
		((obj == '.' && (notanant(Game, Info, offset, directionasnumber) == 1)) ||//(isalpha(obj)==0)) ||
			obj == '*') &&
		previouspos(Game, Info, index, directionasnumber) == 0){
        OpenLog(Game);
        fprintf(Game->logfile, "direction %c \n", direction);
        CloseLog(Game);
		return direction;
    }
    OpenLog(Game);
	fprintf(Game->logfile, "direction %d \n", -1);
    CloseLog(Game);
	return -1;
}
/*
 * I took the original turn decision logic and moved it here just to keep things neat and tidy. 
 * */
char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset){
        
		// defining things just so we can do less writing
		// UP and DOWN move up and down rows while LEFT and RIGHT
		// move side to side. The map is just one big array.

		#define UP -Info->cols
		#define DOWN Info->cols
		#define LEFT -1
		#define RIGHT 1
		
		#define ROW Game->my_ants[index].row
        #define COL Game->my_ants[index].col
        #define ID Game->my_ants[index].id
		
		char dir = -1;		
		char obj_north, obj_east, obj_south, obj_west;		
        int posnorth, poseast, possouth, poswest;
		
		// Now here is the tricky part. We have to account for
        // the fact that the map wraps (when you go off one edge
        // you end up on the side of the map opposite that edge).
        // This is done by checking to see if we are on the last
        // row or column and if the direction we are taking would
        // take us off the side of the map.
        //
        // For example, you can see here the West direction checks
        // to see if we are in the first column, in which case "West"
        // is a character a full row minus one from our location.
		
        if (COL != 0){
            obj_west = Info->map[offset + LEFT];
            poswest = offset + LEFT;      
        }else{
            obj_west = Info->map[offset + Info->cols - 1];
            poswest = offset + Info->cols -1;
        }

        if (COL != Info->cols - 1){
            obj_east = Info->map[offset + RIGHT];
            poseast = offset + RIGHT;
        }else{
            obj_east = Info->map[offset - Info->cols - 1];
            poseast = offset + Info->cols -1;
        }
        
        if (ROW != 0){
            obj_north = Info->map[offset + UP];
            posnorth = offset + UP;
        }else{
            obj_north = Info->map[offset + (Info->rows - 1)*Info->cols];
            posnorth = offset + (Info->rows -1)*Info->cols;
        }
        if (ROW != Info->rows - 1){
            obj_south = Info->map[offset + DOWN];
            possouth = offset + DOWN;
        }else{
            obj_south = Info->map[offset - (Info->rows - 1)*Info->cols];
            possouth = offset - (Info->rows -1)*Info->cols;
        }
		OpenLog(Game);
		//fprintf(Game->logfile, "posnorth %c
		char objects[] = { obj_north, obj_east, obj_south, obj_west };
		int directionasnumber[] = { posnorth, poseast, possouth, poswest };
		char directions[] = {'N', 'E', 'S', 'W' };
		int i;
		
		for(i = 0; i < 4 ; ++i){
		//	if(directions[i] != NULL && objects[i] != NULL  && directionasnumber[i] != NULL ){
				dir = validateturn( Game, Info, index, offset, objects[i], directionasnumber[i], directions[i] );
				if(dir != -1) break;			
		//	}
		}
		
	return dir;
}
void StartLog(struct game_state *Game){
    //FILE *ofp;
    char filename[] = "error.log";
    Game->logfile = fopen(filename, "a");
    if (Game->logfile == NULL){
        fprintf(stderr , "Can't open the file %s\n", filename);
    }
    
    fprintf(Game->logfile, "Beginning Log File\n");
    fclose(Game->logfile);
}
void OpenLog(struct game_state *Game){
    char filename[] = "error.log";
    Game->logfile = fopen(filename, "a");
    if (Game->logfile == NULL){
        fprintf(stderr , "Can't open the file %s\n", filename);
    }    
}
void CloseLog(struct game_state *Game){
    fclose(Game->logfile);
}