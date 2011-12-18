void do_turn(struct game_state *Game, struct game_info *Info);
char determinediffusionscores(struct game_info *Info);
int setgetdiffusionscore(struct game_info *Info, int r, int c);
char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset);
int getdiffusionscore(struct game_info *Info, int index);
void lowerdiffusionscore(struct game_info *Info, int index);
void setsurroundingsquares(struct game_info *Info, int r, int c, int squarescore, int radius);
//void setanadjacentsquaresscore(struct game_info *Info, int row, int col, int score, int distance);
void setanadjacentsquaresscore(struct game_info *Info, int centerrow, int centercol, 
                                int row, int col, int score, char dir, int radius);
int distance(int row1, int col1, int row2, int col2, struct game_info *Info);
int timeup(int maxtime, struct timeval start);