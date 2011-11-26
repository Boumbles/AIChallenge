int notanant(struct game_state *Game, struct game_info *Info, int origoffset, int destination);
int previouspos(struct game_state *Game, struct game_info *Info, int index, int newpos);
int handlecounterandchangedirectionifnecessary(struct game_state *Game, struct game_info *Info, int index);
int validateturn(struct game_state *Game, struct game_info *Info, int index, int offset, char obj, int pos, char direction);

void updateprevious(struct game_state *Game, struct game_info *Info, int index);
void do_turn(struct game_state *Game, struct game_info *Info);
void StartLog(struct game_state *Game);
void OpenLog(struct game_state *Game);
void CloseLog(struct game_state *Game);



char makeaturn(struct game_state *Game, struct game_info *Info, int index, int offset);





//FILE* GetLogFileStream();
