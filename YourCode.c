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
    int i,j;
	
    fprintf(stderr, "\nCount is : %d\n", Game->my_count);
	
    for (i = 0; i < Game->my_count; ++i) {        
        // the location within the map array where our ant is currently
        int offset = Game->my_ants[i].row*Info->cols + Game->my_ants[i].col;
        fprintf(stderr, "\nAnt %d's offset is: %d \n", i, offset);
                
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
    
    fprintf(stderr, "makeaturn()\n");
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
    int tmpdiroffset;
    tmpdiroffset = getbestdirection(Info, posnorth, poseast, possouth, poswest);
    
    lowerdiffusionscore(Info, tmpdiroffset);
    
    if(tmpdiroffset == posnorth) dir = 'N';
    else if(tmpdiroffset ==  poseast) dir = 'E';
    else if(tmpdiroffset == possouth) dir = 'S';
    else if(tmpdiroffset == poswest) dir = 'W';
   
        
        
		
    fflush(stderr);
	return dir;
}
char determinediffusionscores(struct game_info *Info){
	fprintf(stderr, "determinediffusionscores()\n");
	int i;
	int j;
	fflush(stderr);
	for(i = 0; i < Info->rows; ++i){
        fprintf(stderr, "i %d ", i);
        fflush(stderr);
		for(j = 0; j < Info->cols; ++j){
            fprintf(stderr, "j %d\n", j);
            fflush(stderr);
			int offset = i*Info->cols + j;
			char obj = Info->map[j];
			setgetdiffusionscore(Info, i, j);	
		}
	}

	return -1;
}

int setgetdiffusionscore(struct game_info *Info, int r, int c){
    #define UP -Info->cols
    #define DOWN Info->cols
	#define LEFT -1
	#define RIGHT 1
	
    char obj;
	
    fprintf(stderr, "setgetdiffusionscore()\n");
	fprintf(stderr, "row %d, col %d\n", r, c);
	fflush(stderr);
	
    int offset = r*Info->cols + c;
	fprintf(stderr, "offset %d\n", offset);
	fflush(stderr);
	
	obj = Info->map[offset];
	fprintf(stderr, "obj %c\n", obj);
	fflush(stderr);

	
	
	if(Info->scores[offset] > -1){
		fprintf(stderr, "returning previous : %d", Info->scores[offset]);
		fflush(stderr);
        return Info->scores[offset];
    }
    char obj_north, obj_east, obj_south, obj_west;		
    int posnorth, poseast, possouth, poswest;
    int northrow, northcol, eastrow, eastcol, westrow, westcol, southrow, southcol;
	
	//square.row = r;
	//square.col = c;
	fprintf(stderr, "row %d, col %d, obj %c", r, c, obj);
	fflush(stderr);
	if(obj=='%'){ //this is water
		Info->scores[offset] = 0;
		return;
	} else if(obj=='*'){ //nomnoms!
		Info->scores[offset] =  DIFFUSION_FOOD;	
	// } else if(isNumeric(obj)){
		// Info->scores[offset] = DIFFUSION_HILL;
    // } else if(isalpha(obj)) { 
		// Info->scores[offset] = 0;
	} else {  
		fprintf(stderr,"Going to average based on neighboor"
				"'s Info->scores[offset]s ");
		fflush(stderr);
        northrow = r != 0 ? r - 1 : Info->rows - 1;
        northcol = c;
        eastrow = r;
        eastcol = c != Info->cols ? c + 1 : 0;
        westrow = r;
        westcol = c != 0 ? c - 1 : Info->cols - 1;
        southrow = r != Info->rows ? r + 1 : 0;
        southcol = c;
		fprintf(stderr,"Going to average based on neighboor"
				"'s Info->scores[offset]s ");
		fflush(stderr);
		Info->scores[offset] = (setgetdiffusionscore(Info, northrow, northcol) + 
                        setgetdiffusionscore(Info, eastrow, eastcol) + 
                        setgetdiffusionscore(Info, westrow, westcol) + 
                        setgetdiffusionscore(Info, southrow, southcol)) / 4;
	}
    fprintf(stderr,"done. score was %d: ", Info->scores[offset]);
    fflush(stderr);
}
int getbestdirection(struct game_info *Info, int posnorth, int poseast, int possouth, int poswest){
    fprintf(stderr, "getbestdirection()\n");
    fflush(stderr);
    int northdiff, eastdiff, southdiff, westdiff;
    int highest;
    northdiff = getdiffusionscore(Info, posnorth);
    eastdiff = getdiffusionscore(Info, poseast);
    southdiff = getdiffusionscore(Info, possouth);
    westdiff = getdiffusionscore(Info, poswest);
    highest =  northdiff > eastdiff ? northdiff : eastdiff;
    highest = highest > southdiff ? highest : southdiff;
    highest = highest > westdiff ? highest : westdiff;
    
    if(highest ==  northdiff) return posnorth;
    else if (highest ==  eastdiff) return poseast;
    else if (highest ==  southdiff) return possouth;
    else if (highest ==  westdiff) return poswest;
    
    return -1;
}
int getdiffusionscore(struct game_info *Info, int index){
    if(Info->scores[index] >= 0)
        return Info->scores[index];
    return - 1;
}
void lowerdiffusionscore(struct game_info *Info, int index){
    if(Info->scores[index])
        Info->scores[index] -= DIFFUSION_DECREMENT;
}
//from http://rosettacode.org/wiki/Determine_if_a_string_is_numeric#C
int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

