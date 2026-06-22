#include "stdafx.h"
#include <stdlib.h>
#include "enemys.h"

int mrand(int n)
{
    if (n == 0) return 0;
    if (n > 2) return rand() / rmax;
    else return rand() / rmax1;
}

// Define maxenemys, boss, and mast constants if needed (they are declared extern in the header,
// but defined in YonyDlg.cpp. However, to avoid duplicate definition issues, we just use the extern values).

static int floor_div(int a, int b) {
    int res = a / b;
    int rem = a % b;
    if (rem != 0 && ((a < 0) ^ (b < 0))) {
        res--;
    }
    return res;
}

static int get_crossed_grid_index(int curr, int next, int offset) {
    int curr_cell = floor_div(curr - offset, 30);
    int next_cell = floor_div(next - offset, 30);
    if (curr_cell != next_cell) {
        if (next > curr) {
            return next_cell * 30 + offset;
        } else {
            return curr_cell * 30 + offset;
        }
    }
    return -1;
}

bool IsWallInRect(int start_row, int start_col, int height_in_cells, int width_in_cells) {
    for (int r = start_row; r < start_row + height_in_cells; r++) {
        for (int c = start_col; c < start_col + width_in_cells; c++) {
            if (r >= 0 && r < 19 && c >= 0 && c < 26) {
                if (board[r][c] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

static bool IsEnemyCollidingWithOtherEnemies(int i, int next_x, int next_y) {
    int w_i = (enemy[i].level >= 70) ? 90 : ((enemy[i].level < 20) ? 30 : 60);
    int h_i = w_i;

    for (int j = 0; j < nenemys; j++) {
        if (j == i) continue;
        if (enemy[j].stat > 0) continue; // dead or waiting for rebirth

        int w_j = (enemy[j].level >= 70) ? 90 : ((enemy[j].level < 20) ? 30 : 60);
        int h_j = w_j;

        // Check bounding box overlap
        bool collisionX = next_x < enemy[j].x + w_j && next_x + w_i > enemy[j].x;
        bool collisionY = next_y < enemy[j].y + h_j && next_y + h_i > enemy[j].y;

        if (collisionX && collisionY) {
            // Check if they were already overlapping at the start of the frame:
            bool currX = enemy[i].bx < enemy[j].bx + w_j && enemy[i].bx + w_i > enemy[j].bx;
            bool currY = enemy[i].by < enemy[j].by + h_j && enemy[i].by + h_i > enemy[j].by;
            if (currX && currY) {
                // They were already overlapping. Allow movement to let them separate.
                continue;
            }
            return true;
        }
    }
    return false;
}

static bool IsPathBlocked(int i, int start_row, int start_col, int height_in_cells, int width_in_cells) {
    // Check board walls
    if (IsWallInRect(start_row, start_col, height_in_cells, width_in_cells)) {
        return true;
    }

    // Convert cell rect to pixels
    int rect_x_min = 10 + start_col * 30;
    int rect_x_max = 10 + (start_col + width_in_cells) * 30;
    int rect_y_min = 30 + start_row * 30;
    int rect_y_max = 30 + (start_row + height_in_cells) * 30;

    // Check other active enemies
    for (int j = 0; j < nenemys; j++) {
        if (j == i) continue;
        if (enemy[j].stat > 0) continue; // dead or spawning

        int w_j = (enemy[j].level >= 70) ? 90 : ((enemy[j].level < 20) ? 30 : 60);
        int h_j = w_j;

        bool overlapX = enemy[j].x < rect_x_max && enemy[j].x + w_j > rect_x_min;
        bool overlapY = enemy[j].y < rect_y_max && enemy[j].y + h_j > rect_y_min;

        if (overlapX && overlapY) {
            // Again, if they were already overlapping at the start of the frame, ignore to let them separate
            int w_i = (enemy[i].level >= 70) ? 90 : ((enemy[i].level < 20) ? 30 : 60);
            int h_i = w_i;
            bool currX = enemy[i].bx < enemy[j].bx + w_j && enemy[i].bx + w_i > enemy[j].bx;
            bool currY = enemy[i].by < enemy[j].by + h_j && enemy[i].by + h_i > enemy[j].by;
            if (currX && currY) {
                continue;
            }
            return true;
        }
    }

    return false;
}

bool ChooseEnemyDirection(int i) {
    int x1 = (enemy[i].x - 10) / 30;
    int y1 = (enemy[i].y - 30) / 30;
    int x2 = x1 - 1; if (x2 < 0) x2 = 0;
    int x3 = x1 + 1; if (x3 > 25) x3 = 25;
    int y2 = y1 - 1; if (y2 < 0) y2 = 0;
    int y3 = y1 + 1; if (y3 > 18) y3 = 18;
    int n = 0;

    // enemy number 7 eat
    if (enemy[i].level == 6) {
        if (board[y1][x1] > 1) {
            if (eat(y1, x1, false) == true) return true;
        }
        // set position on field
        // if enemy 7 see eat, follow on eat :)
        if (enemy[i].go < 3) { // ==1,2
            if (board[y2][x1] > 1) n += 1;
            if (board[y3][x1] > 1) n += 2;
            if (n == 0) {
                if (board[y1][x2] > 1) { n = 4; enemy[i].go = 1; }
                if (board[y1][x3] > 1) { n = 4; enemy[i].go = 2; }
            }
            if (n > 0) {
                if (n == 3) enemy[i].go = rand() / rmax1 + 3;
                if (n == 2) enemy[i].go = 4;
                if (n == 1) enemy[i].go = 3;
            }
        } else { // ==3,4
            if (board[y1][x2] > 1) n += 1;
            if (board[y1][x3] > 1) n += 2;
            if (n == 0) {
                if (board[y2][x1] > 1) { n = 4; enemy[i].go = 3; }
                if (board[y3][x1] > 1) { n = 4; enemy[i].go = 4; }
            }
            if (n > 0) {
                if (n == 3) enemy[i].go = rand() / rmax1 + 1;
                if (n == 2) enemy[i].go = 2;
                if (n == 1) enemy[i].go = 1;
            }
        }
    } else {
        // Unified pathfinding logic for all other enemy sizes (30x30, 60x60, 90x90)
        int enemyWidth = (enemy[i].level >= 70) ? 90 : ((enemy[i].level < 20) ? 30 : 60);
        int cell_w = enemyWidth / 30;
        int cell_h = enemyWidth / 30;
        int n1;

        if (enemy[i].go < 3) { // ==1,2 (horizontal)
            // Check upward and downward corridors
            if (!IsPathBlocked(i, y1 - 1, x1, 1, cell_w)) n += 1;
            if (!IsPathBlocked(i, y1 + cell_h, x1, 1, cell_w)) n += 2;

            if (enemy[i].go == 1 && IsPathBlocked(i, y1, x1 - 1, cell_h, 1)) {
                if (n == 3) enemy[i].go = mrand(2) + 3;
                else enemy[i].go = n + 2;
            } else if (enemy[i].go == 2 && IsPathBlocked(i, y1, x1 + cell_w, cell_h, 1)) {
                if (n == 3) enemy[i].go = mrand(2) + 3;
                else if (n > 0) enemy[i].go = n + 2;
                else enemy[i].go = 1;
            } else {
                if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1 + 2; }
                else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 4; }
                else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 3; }
            }
        } else { // ==3,4 (vertical)
            // Check left and right corridors
            if (!IsPathBlocked(i, y1, x1 - 1, cell_h, 1)) n += 1;
            if (!IsPathBlocked(i, y1, x1 + cell_w, cell_h, 1)) n += 2;

            if (enemy[i].go == 3 && IsPathBlocked(i, y1 - 1, x1, 1, cell_w)) {
                if (n == 3) enemy[i].go = mrand(2) + 1;
                else if (n > 0) enemy[i].go = n;
                else enemy[i].go = 4;
            } else if (enemy[i].go == 4 && IsPathBlocked(i, y1 + cell_h, x1, 1, cell_w)) {
                if (n == 3) enemy[i].go = mrand(2) + 1;
                else if (n > 0) enemy[i].go = n;
                else enemy[i].go = 3;
            } else {
                if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1; }
                else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 2; }
                else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 1; }
            }
        }
    }
    return false;
}

bool IsEnemyWallCollision(int i, int go) {
    int enemyWidth = (enemy[i].level >= 70) ? 90 : ((enemy[i].level < 20) ? 30 : 60);
    int enemyHeight = enemyWidth;
    int shag = enemy[i].shag;

    int next_x = enemy[i].x;
    int next_y = enemy[i].y;
    bool wall_collision = false;

    if (go == 1) { // Left
        if (enemy[i].x <= 10) return false;
        next_x = enemy[i].x - shag;
        int col = (next_x - 10) / 30;
        int row_start = (enemy[i].y - 30) / 30;
        int row_end = (enemy[i].y + enemyHeight - 1 - 30) / 30;
        wall_collision = IsWallInRect(row_start, col, row_end - row_start + 1, 1);
    }
    else if (go == 2) { // Right
        int limit = (enemy[i].level >= 70) ? 700 : ((enemy[i].level < 20) ? 760 : 730);
        if (enemy[i].x >= limit) return false;
        next_x = enemy[i].x + shag;
        int col = (next_x + enemyWidth - 1 - 10) / 30;
        int row_start = (enemy[i].y - 30) / 30;
        int row_end = (enemy[i].y + enemyHeight - 1 - 30) / 30;
        wall_collision = IsWallInRect(row_start, col, row_end - row_start + 1, 1);
    }
    else if (go == 3) { // Up
        if (enemy[i].y <= 30) return false;
        next_y = enemy[i].y - shag;
        int row = (next_y - 30) / 30;
        int col_start = (enemy[i].x - 10) / 30;
        int col_end = (enemy[i].x + enemyWidth - 1 - 10) / 30;
        wall_collision = IsWallInRect(row, col_start, 1, col_end - col_start + 1);
    }
    else if (go == 4) { // Down
        int limit = (enemy[i].level >= 70) ? 510 : ((enemy[i].level < 20) ? 570 : 540);
        if (enemy[i].y >= limit) return false;
        next_y = enemy[i].y + shag;
        int row = (next_y + enemyHeight - 1 - 30) / 30;
        int col_start = (enemy[i].x - 10) / 30;
        int col_end = (enemy[i].x + enemyWidth - 1 - 10) / 30;
        wall_collision = IsWallInRect(row, col_start, 1, col_end - col_start + 1);
    }

    if (wall_collision) return true;

    return IsEnemyCollidingWithOtherEnemies(i, next_x, next_y);
}

static bool ApplyEnemyMovementStep(int i) {
    int next_x = enemy[i].x;
    int next_y = enemy[i].y;
    if (enemy[i].go == 1) next_x -= enemy[i].shag;
    else if (enemy[i].go == 2) next_x += enemy[i].shag;
    else if (enemy[i].go == 3) next_y -= enemy[i].shag;
    else if (enemy[i].go == 4) next_y += enemy[i].shag;

    bool crossed_intersection = false;
    if (enemy[i].go == 1 || enemy[i].go == 2) {
        int crossed_x = get_crossed_grid_index(enemy[i].x, next_x, 10);
        if (crossed_x != -1 && (enemy[i].y - 30) % 30 == 0) {
            enemy[i].x = crossed_x;
            crossed_intersection = true;
        }
    } else {
        int crossed_y = get_crossed_grid_index(enemy[i].y, next_y, 30);
        if (crossed_y != -1 && (enemy[i].x - 10) % 30 == 0) {
            enemy[i].y = crossed_y;
            crossed_intersection = true;
        }
    }
    return crossed_intersection;
}

static void MoveEnemy(int i) {
    switch (enemy[i].go) {
        case 1:
            enemy[i].x -= enemy[i].shag; break;
        case 2:
            enemy[i].x += enemy[i].shag; break;
        case 3:
            enemy[i].y -= enemy[i].shag; break;
        case 4:
            enemy[i].y += enemy[i].shag; break;
    }
    
    // Screen boundary wrap-around
    if (enemy[i].level > 70) {
        if (enemy[i].x < 10) enemy[i].x = 760;
        else if (enemy[i].y < 30) enemy[i].y = 570;
        if (enemy[i].x >= 700) enemy[i].x = 10;
        else if (enemy[i].y > 510) enemy[i].y = 30;
    } else if (enemy[i].level < 20) {
        if (enemy[i].x < 10) enemy[i].x = 760;
        else if (enemy[i].y < 30) enemy[i].y = 570;
        if (enemy[i].x > 760) enemy[i].x = 10;
        else if (enemy[i].y > 570) enemy[i].y = 30;
    } else {
        if (enemy[i].x < 10) enemy[i].x = 730;
        else if (enemy[i].x >= 730) enemy[i].x = 10;
        if (enemy[i].y < 30) enemy[i].y = 540;
        else if (enemy[i].y > 540) enemy[i].y = 30;
    }
}

void UpdateEnemiesBehavior() {
    int i;
    
    tis1++;
    if (tis1 >= ctis1) {
        tis1 = 0;
        is1 += 30;
        if (is1 > 30) is1 = 0;
    } // enemy picture change
    
    for (i = 0; i < nenemys; i++) {
        // clear old enemy in backbuffer 
        if (enemy[i].level < 20) {
            putimagetoscreen(enemy[i].bx, enemy[i].by, enemy[i].bx + 30, enemy[i].by + 30,
                             enemy[i].bx, enemy[i].by, bi, 1);
        } else if (enemy[i].level < 70) {
            putimagetoscreen(enemy[i].bx, enemy[i].by, enemy[i].bx + 60, enemy[i].by + 60,
                             enemy[i].bx, enemy[i].by, bi, 1);
        } else {
            putimagetoscreen(enemy[i].bx, enemy[i].by, enemy[i].bx + 90, enemy[i].by + 90,
                             enemy[i].bx, enemy[i].by, bi, 1);
        }

        // save enemy old position
        enemy[i].bx = enemy[i].x;
        enemy[i].by = enemy[i].y;

        // if enemys count < maxenemy-1 and enemy is master - start birth proc
        if (enemy[i].level >= 20) {
            if (enemy[i].stat == 0) { // if master in wait queue, decrease wait queue counter
                enemy[i].nstat--;
                // if enemy wait queue counter equal to zero - start birth faze
                if ((enemy[i].nstat <= 0) && (tis1 == 0)) {
                    if (nenemys < maxenemys - 1) {
                        enemy[i].stat = -1;
                    }
                    enemy[i].nstat = 0;
                }
            } else { // enemy birth faze
                if (tis1 % 2 == 0) enemy[i].nstat++;
                if ((enemy[i].nstat == 15) && (tis1 % 2 == 0)) {
                    // boss birth
                    // set boss characteristics
                    int mastx = enemy[i].x + 15;
                    int masty = enemy[i].y + 30;
                    mastx -= ((mastx - 10) % 30);
                    masty -= (masty % 30);
                    enemy[nenemys].x = mastx;
                    enemy[nenemys].y = masty;
                    enemy[nenemys].bx = mastx;
                    enemy[nenemys].by = masty;
                    if (enemy[i].level >= 70) {
                        enemy[nenemys].level = 20;
                        enemy[nenemys].nstat = boss;
                    } else {
                        enemy[nenemys].level = 4;
                        enemy[nenemys].nstat = 0;
                    }
                    enemy[nenemys].stat = 0;
                    enemy[nenemys].mnstat = 4;
                    enemy[nenemys].go = 4; // enemy ->
                    enemy[nenemys].shag = 2;
                    nenemys++;
                }
                if (enemy[i].nstat > 25) { // if enemy birth faze is ended - start wait faze
                    // start wait faze
                    if (enemy[i].level < 70) {
                        enemy[i].nstat = boss;
                    } else {
                        enemy[i].nstat = mast;
                    }
                    enemy[i].stat = 0;
                }
            }
        }

        if (enemy[i].stat <= 0) {
            // if monster status>0, death or wait...
            // collision detection
            int enemyWidth;  // enemy width
            int enemyHeight; // enemy height
            if (enemy[i].level >= 70) {
                enemyWidth = 90;
                enemyHeight = 90;
            } else if (enemy[i].level < 20) {
                enemyWidth = 30;
                enemyHeight = 30;
            } else {
                enemyWidth = 60;
                enemyHeight = 60;
            }

            bool collisionX = mx < enemy[i].x + enemyWidth && mx + 30 > enemy[i].x;
            bool collisionY = my < enemy[i].y + enemyHeight && my + 30 > enemy[i].y;

            if (collisionX && collisionY && !pdeath && !nimb) { // collision
                if ((enemy[i].level >= 20) || ((enemy[i].level != 6) && !pchang)) { // if player must death
                    // player death
                    ctis /= 3;
                    pdeath = true;
                    pchang = false;
                    hcount--;
                    stopbuf(s[0]);
                    stopbuf(s[2]);
                    stopbuf(s[3]);
                    stopbuf(s[4]);
                    stopbuf(s[10]);
                    setbufpos(s[0], 0);
                    setbufpos(s[2], 0);
                    setbufpos(s[3], 0);
                    setbufpos(s[4], 0);
                    setbufpos(s[10], 0);
                    playbuf(s[5 + mrand(3)], false);
                } else if (pchang) { // enemy death if pchang and enemy level <20
                    playbuf(s[8], false);
                    score = score + enemy[i].level + 10;
                    enemy[i].stat = 1;
                    tis1 = 1;
                }
            }

            bool crossed_intersection = ApplyEnemyMovementStep(i);

            bool is_blocked = IsEnemyWallCollision(i, enemy[i].go);

            if (crossed_intersection || is_blocked) {
                if (ChooseEnemyDirection(i)) return;
            }

            if (!IsEnemyWallCollision(i, enemy[i].go)) {
                MoveEnemy(i);
            }
        }
    }
}
