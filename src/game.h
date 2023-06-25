#ifndef GAME_H
#define GAME_H

struct game_state {
    bool Running;
};

void GameUpdateAndRender(game_state *GameState);

#endif