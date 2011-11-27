int notanant(struct game_state *Game, struct game_info *Info, int origoffset, int destination);
int ispreviousposition(struct game_state *Game, struct game_info *Info, int index, int newpos);
int handlecounterandchangedirectionifnecessary(struct game_state *Game, struct game_info *Info, int index);
char validateturn(struct game_state *Game, struct game_info *Info, int index, int offset, char obj, int pos, char direction);
void setdestination(struct game_state *Game, int index, int dstcol, int dstrow);
//void updateprevious(struct game_state *Game, struct game_info *Info, int index);
void do_turn(struct game_state *Game, struct game_info *Info);
void StartLog(struct game_state *Game);
void OpenLog(struct game_state *Game);
void CloseLog(struct game_state *Game);
int anotherantmovingtothisspot(struct game_state *Game, int dir);
int anotherantmovinginthisdirection(struct game_state *Game, int index, char dir);

char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset);