void do_turn(struct game_state *Game, struct game_info *Info);
char determinediffusionscores(struct game_info *Info);
int setgetdiffusionscore(struct game_info *Info, int r, int c);
char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset);
int getdiffusionscore(struct game_info *Info, int index);
void lowerdiffusionscore(struct game_info *Info, int index);