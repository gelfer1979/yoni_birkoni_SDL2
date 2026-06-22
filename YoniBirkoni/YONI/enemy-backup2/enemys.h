#ifndef ENEMYS_H
#define ENEMYS_H

#include "stdafx.h"
#include <stdbool.h>
#include "sdl/GRAPH.H"
#include "sdl/sound.h"

extern const int maxenemys;
extern const int boss;
extern const int mast;

struct z { // structure of enemy
    int x;
    int y;
    int bx;
    int by;
    int level;
    int go; // <- -> V A of enemy
    int shag;
    int stat;
    int nstat;
    int mnstat;
};

// Declarations of global variables used in enemy behavior
extern struct z enemy[];
extern int nenemys;
extern int tis1;
extern int ctis1;
extern int is1;
extern int bi;
extern int mx;
extern int my;
extern bool pdeath;
extern int nimb;
extern bool pchang;
extern int hcount;
extern int s[];
extern int score;
extern int rmax1;
extern char board[19][26];
extern int ctis;

// Declarations of local global functions
extern bool eat(int kmy, int kmx, bool player);
extern int mrand(int n);

// Helper functions for enemy logic
bool IsWallInRect(int start_row, int start_col, int height_in_cells, int width_in_cells);
bool ChooseEnemyDirection(int i);
bool IsEnemyWallCollision(int i, int go);

// Function to update enemy behavior (movement, collision, birth)
void UpdateEnemiesBehavior();

#endif // ENEMYS_H
