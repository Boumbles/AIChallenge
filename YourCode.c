#include "ants.h"
#include "YourCode.h"
// The following is an example program displaying a basic ai that
// tries all possible directions and then moves in whichever one
// is not blocked by water (% characters).
//
// To see all the information contained in struct game_state and
// struct game_info check ants.h. There is also a distance function
// provided in bot.c

void do_turn(struct game_state *Game, struct game_info *Info) {
    //gettimeofday(&Info->currtime, NULL);
    
    int i,j;	
    for (i = 0; i < Game->my_count; ++i) {    
        if(timeup(Info->turntime, Info->turnstart) == 1) break;
            
        // the location within the map array where our ant is currently
        int offset = Game->my_ants[i].row*Info->cols + Game->my_ants[i].col;
                
        char dir = -1;
		
		dir = makeaturn(Game, Info, i, offset);
		if(dir != -1){            			
			fflush(stderr);
			move(i, dir, Game, Info);		
		}
    }
	fflush(stderr);    
}
char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset){
    
    #define UP -Info->cols
    #define DOWN Info->cols
    #define LEFT -1
    #define RIGHT 1
    
    #define ROW Game->my_ants[index].row
    #define COL Game->my_ants[index].col
    #define ID Game->my_ants[index].id
    
    char dir = -1;		
    //char obj_north, obj_east, obj_south, obj_west;		
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
       // obj_west = Info->map[offset + LEFT];
        poswest = offset + LEFT;      
    }else{
       // obj_west = Info->map[offset + Info->cols - 1];
        poswest = offset + Info->cols -1;
    }

    if (COL != Info->cols - 1){
        //obj_east = Info->map[offset + RIGHT];
        poseast = offset + RIGHT;
    }else{
        //obj_east = Info->map[offset - Info->cols - 1];
        poseast = offset + Info->cols -1;
    }
    
    if (ROW != 0){
        //obj_north = Info->map[offset + UP];
        posnorth = offset + UP;
    }else{
       // obj_north = Info->map[offset + (Info->rows - 1)*Info->cols];
        posnorth = offset + (Info->rows -1)*Info->cols;
    }
    if (ROW != Info->rows - 1){
       // obj_south = Info->map[offset + DOWN];
        possouth = offset + DOWN;
    }else{
        //obj_south = Info->map[offset - (Info->rows - 1)*Info->cols];
        possouth = offset - (Info->rows -1)*Info->cols;
    }
    int tmpdiroffset;
    tmpdiroffset = getbestdirection(Info, posnorth, poseast, possouth, poswest);
    //if(Info->scores[index])
    Info->scores[offset] -= DIFFUSION_DECREMENT;
	fprintf(stderr, "bestoffset %d\n", tmpdiroffset);
    fflush(stderr);
    //lowerdiffusionscore(Info, tmpdiroffset);
    
    if(tmpdiroffset == posnorth) dir = 'N';
    else if(tmpdiroffset ==  poseast) dir = 'E';
    else if(tmpdiroffset == possouth) dir = 'S';
    else if(tmpdiroffset == poswest) dir = 'W';
    
        
        
	if(tmpdiroffset != -1)
        Info->scores[tmpdiroffset] = 0;
    else 
        Info->scores[offset] = 0;
    //fflush(stderr);
	return dir;
}
void determinediffusionscores(struct game_info *Info){
	int i;
	int j;
	fflush(stderr);
	for(i = 0; i < Info->rows; ++i){
        if(timeup(Info->turntime, Info->turnstart) == 1) break;        
        for(j = 0; j < Info->cols; ++j){
           int offset = i*Info->cols + j;
			char obj = Info->map[j];
            if(Info->scores[offset] != DIFFUSION_WATER)
                setgetdiffusionscore(Info, i, j);	
		}
	}
	return;
}

void setgetdiffusionscore(struct game_info *Info, int r, int c){
    if(timeup(Info->turntime, Info->turnstart) == 1) return;
        
    #define UP -Info->cols
    #define DOWN Info->cols
	#define LEFT -1
	#define RIGHT 1
	
    char obj;
	
  	
    int offset = r*Info->cols + c;
		
	obj = Info->map[offset];
	fflush(stderr);
	
	if(obj=='%'){ //this is water
		Info->scores[offset] = DIFFUSION_WATER;
		return;
	} else if(obj=='*'){ //nomnoms!
		Info->scores[offset] =  DIFFUSION_FOOD;	
		setsurroundingsquares(Info, r, c, DIFFUSION_FOOD, DIFFUSION_MAX_DISTANCE_NOM);
	} else if(isdigit(obj)){
		if(obj=='0'){
			Info->scores[offset] = DIFFUSION_WATER;
		} else {			
			Info->scores[offset] = DIFFUSION_HILL;
			setsurroundingsquares(Info, r, c, DIFFUSION_HILL, DIFFUSION_MAX_DISTANCE_HILL);
		}
    } else if(isalpha(obj)) {
		Info->scores[offset] = DIFFUSION_ANT_AVOID;
        //if it isn't one of our own ants we should avoid it(until we  get some combat logic)
        if(obj != 'a' && obj != 'A') setsurroundingsquares(Info, r, c, DIFFUSION_ANT_AVOID, DIFFUSION_MAX_DISTANCE_ANT);
	} else if(Info->scores[offset] > -1 || Info->scores[offset] == DIFFUSION_WATER){
	} else if (Info->scores[offset] == DIFFUSION_ANT_AVOID) {
		Info->scores[offset] = DIFFUSION_EXPLORE - DIFFUSION_DECREMENT;
	}else{
		Info->scores[offset] = DIFFUSION_EXPLORE;
	}
	
 	//return Info->scores[offset];
}

void setsurroundingsquares(struct game_info *Info, int r, int c, int squarescore, int radius){
    if(timeup(Info->turntime, Info->turnstart) == 1) return;
        
	char obj_north, obj_east, obj_south, obj_west;		
    int posnorth, poseast, possouth, poswest;
    int northrow, northcol, eastrow, eastcol, westrow, westcol, southrow, southcol;
	int diffusionvalue = squarescore - DIFFUSION_DECREMENT;
	
	northrow = r != 0 ? r - 1 : Info->rows - 1;
	northcol = c;
    eastrow = r;
    eastcol = c != Info->cols ? c + 1 : 0;
    westrow = r; 
    westcol = c != 0 ? c - 1 : Info->cols - 1;
    southrow = r != Info->rows ? r + 1 : 0;
    southcol = c;
    setanadjacentsquaresscore(Info, r, c, northrow, northcol, squarescore, 'N', radius);
    setanadjacentsquaresscore(Info, r, c, eastrow, eastcol, squarescore, 'E', radius);
    setanadjacentsquaresscore(Info, r, c, southrow, southcol, squarescore, 'S', radius);
    setanadjacentsquaresscore(Info, r, c, westrow, westcol, squarescore, 'W', radius);
}
void setanadjacentsquaresscore(struct game_info *Info, int centerrow, int centercol, 
                               int row, int col, int score, char dir, int radius){
    //gettimeofday(&Info->currtime, NULL);
    if(timeup(Info->turntime, Info->turnstart) == 1) return;
    
    int rowcoloffset = row*Info->cols + col;
    int centeroffset = centerrow*Info->cols + centercol;
    if(Info->scores[rowcoloffset] > DIFFUSION_EXPLORE && //square is set for exploring
        Info->scores[centeroffset] > -1){
        return;
    }
    if(Info->scores[centeroffset] == DIFFUSION_ANT_AVOID && 
        Info->scores[rowcoloffset] != -1 && 
        Info->scores[rowcoloffset] < DIFFUSION_EXPLORE){ //square is near an ant
        return;
    }
    // fprintf(stderr, "setanadjacentsquaresscore() center row: %d, center col: %d "
                    // " row: %d, col:%d, centerscore:%d, dir:%c\n",
                    // centerrow, centercol, row, col, score, dir);
    // fflush(stderr);
    
    int northrow, northcol, eastrow, eastcol, westrow, westcol, southrow, southcol;
    int disttocenter = distance(centerrow, centercol, row, col, Info);
    //int disttoprev = distance(prevrow, prevcol, row, col, Info);
    // fprintf(stderr, "distance between points = %d\n", disttocenter);
    // fflush(stderr);
    if(disttocenter <= radius && disttocenter > 0){    
        if(Info->scores[centeroffset] == DIFFUSION_ANT_AVOID)
            Info->scores[rowcoloffset] = DIFFUSION_ANT_AVOID + disttocenter * DIFFUSION_DECREMENT;
        else Info->scores[rowcoloffset] = score - (disttocenter + 1 ) * DIFFUSION_DECREMENT;        
        northrow = row != 0 ? row - 1 : Info->rows - 1;
        northcol = col;
        eastrow = row;
        eastcol = col != Info->cols ? col + 1 : 0;
        westrow = row; 
        westcol = col != 0 ? col - 1 : Info->cols - 1;
        southrow = row != Info->rows ? row + 1 : 0;
        southcol = col;
        if(dir != 'S') setanadjacentsquaresscore(Info, centerrow, centercol, northrow, northcol, score, dir, radius);
        if(dir != 'W') setanadjacentsquaresscore(Info, centerrow, centercol, eastrow, eastcol, score, dir, radius);
        if(dir != 'N') setanadjacentsquaresscore(Info, centerrow, centercol, southrow, southcol, score, dir, radius);
        if(dir != 'E') setanadjacentsquaresscore(Info, centerrow, centercol, westrow, westcol, score, dir, radius);
        
        return;        
    }
    //fprintf(stderr, "was too far / close\n");
    //fflush(stderr);
}

int getbestdirection(struct game_info *Info, int posnorth, int poseast, int possouth, int poswest, int radius){
    //fprintf(stderr, "getbestdirection()\n");
    //fflush(stderr);
    int northdiff, eastdiff, southdiff, westdiff;
    int highest;
    northdiff = Info->scores[posnorth];
    eastdiff = Info->scores[poseast];
    southdiff = Info->scores[possouth];
    westdiff = Info->scores[poswest];
	
	
    highest =  northdiff > eastdiff ? northdiff : eastdiff;
    highest = highest > southdiff ? highest : southdiff;
    highest = highest > westdiff ? highest : westdiff;
    // fprintf(stderr, "northdiff %d, eastdiff %d, southdiff %d, westdiff %d" 
					// " highest %d.", northdiff, eastdiff, southdiff, westdiff, 
					// highest);
	// fflush(stderr);
    if(highest ==  northdiff) return posnorth;
    else if (highest ==  eastdiff) return poseast;
    else if (highest ==  southdiff) return possouth;
    else if (highest ==  westdiff) return poswest;
    fprintf(stderr, "no best\n");
    fflush(stderr);
    return -1;
}

/* int getdiffusionscore(struct game_info *Info, int index){
    if(Info->scores[index] >= 0)
        return Info->scores[index];
    return - 1;
} */
void lowerdiffusionscore(struct game_info *Info, int index){
	//fprintf(stderr, "\nlowering diffusion of %d \n", index);
    if(Info->scores[index])
        Info->scores[index] -= DIFFUSION_DECREMENT;
	//fprintf(stderr, "new score is %d\n" , Info->scores[index]);
	fflush(stderr);
}

int getoffset(int r, int c, struct game_info *Info){
   // fprintf(stderr, "getoffset()\n");
    //fflush(stderr);
	return r*Info->cols + c;
}

// returns the distance between two items on the grid accounting for map wrapping

int distance(int row1, int col1, int row2, int col2, struct game_info *Info) {
    int dr, dc;
    int abs1, abs2;
    // fprintf(stderr, "distance()\n");
    // fflush(stderr);
    abs1 = abs(row1 - row2);
    abs2 = Info->rows - abs(row1 - row2);

    if (abs1 > abs2)
        dr = abs2;
    else
        dr = abs1;

    abs1 = abs(col1 - col2);
    abs2 = Info->cols - abs(col1 - col2);

    if (abs1 > abs2)
        dc = abs2;
    else
        dc = abs1;
    // fprintf(stderr, "leaving distance()\n");
    // fflush(stderr);
    
    return sqrt(pow(dr, 2) + pow(dc, 2));
}
int timeup(int maxtime, struct timeval start){
    struct timeval now;
    gettimeofday(&now, NULL);
    float runtime = now.tv_usec - start.tv_usec;
    if(runtime/1000 > maxtime * .75) {
        fprintf(stderr, "time's up\n");
        fflush(stderr);
        return 1;
    }
    
    return 0;
}