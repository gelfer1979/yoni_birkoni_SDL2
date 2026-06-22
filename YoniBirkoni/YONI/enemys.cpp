#include "stdafx.h"
#include <stdlib.h>
#include "enemys.h"

// Define maxenemys, boss, and mast constants if needed (they are declared extern in the header,
// but defined in YonyDlg.cpp. However, to avoid duplicate definition issues, we just use the extern values).

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
            // --------position change
            int n = 0;
            int x1 = (enemy[i].x - 10) / 30;
            int y1 = (enemy[i].y - 30) / 30;

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

            if (((enemy[i].x - 10) % 30 == 0) && (enemy[i].y % 30 == 0)) {
                bool alr = false;
                // set ch values
                int x2 = x1 - 1; if (x2 < 0) x2 = 0;
                int x3 = x1 + 1; if (x3 > 25) x3 = 25;
                int y2 = y1 - 1; if (y2 < 0) y2 = 0;
                int y3 = y1 + 1; if (y3 > 18) y3 = 18;
                
                // enemy number 7 eat
                if (enemy[i].level == 6) {
                    if (board[y1][x1] > 1) {
                        if (eat(y1, x1, false) == true) return;
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
                            alr = true;
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
                            alr = true;
                            if (n == 3) enemy[i].go = rand() / rmax1 + 1;
                            if (n == 2) enemy[i].go = 2;
                            if (n == 1) enemy[i].go = 1;
                        }
                    }
                } else if (enemy[i].level >= 70) {
                    alr = true;
                    int n1;
                    if (enemy[i].go < 3) { // ==1,2
                        if ((board[y2][x1] != 1) && (board[y2][x3] != 1) && (board[y2][x3 + 1] != 1)) n += 1;
                        if ((board[y3 + 2][x1] != 1) && (board[y3 + 2][x3] != 1) && (board[y3 + 2][x3 + 1] != 1)) n += 2;
                        if ((enemy[i].go == 1) && ((board[y1][x2] == 1) || (board[y3][x2] == 1) || (board[y3 + 1][x2] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else enemy[i].go = n + 2;
                        } else if ((enemy[i].go == 2) && (((board[y1][x3 + 2] == 1) || (board[y3][x3 + 2] == 1)) || (board[y3 + 1][x3 + 2] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else if (n > 0) enemy[i].go = n + 2;
                            else enemy[i].go = 1;
                        } else {
                            if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1 + 2; }
                            else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 4; }
                            else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 3; }
                        }
                    } else { // ==3,4
                        if ((board[y1][x2] != 1) && (board[y3][x2] != 1) && (board[y3 + 1][x2] != 1)) n += 1;
                        if ((board[y1][x3 + 2] != 1) && (board[y3][x3 + 2] != 1) && (board[y3 + 1][x3 + 2] != 1)) n += 2;
                        if ((enemy[i].go == 3) && ((board[y2][x1] == 1) || (board[y2][x3] == 1) || (board[y2][x3 + 1] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 1;
                            else if (n > 0) enemy[i].go = n;
                            else enemy[i].go = 4;
                        } else if ((enemy[i].go == 4) && ((board[y3 + 2][x1] == 1) || (board[y3 + 2][x3] == 1) || (board[y3 + 2][x3 + 1] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 1;
                            else if (n > 0) enemy[i].go = n;
                            else enemy[i].go = 3;
                        } else {
                            if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1; }
                            else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 2; }
                            else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 1; }
                        }
                    }
                } else if (enemy[i].level >= 20) {
                    alr = true;
                    int n1;
                    if (enemy[i].go < 3) { // ==1,2
                        if ((board[y2][x1] != 1) && (board[y2][x3] != 1)) n += 1;
                        if ((board[y3 + 1][x1] != 1) && (board[y3 + 1][x3] != 1)) n += 2;
                        if ((enemy[i].go == 1) && ((board[y1][x2] == 1) || (board[y3][x2] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else enemy[i].go = n + 2;
                        } else if ((enemy[i].go == 2) && ((board[y1][x3 + 1] == 1) || (board[y3][x3 + 1] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else if (n > 0) enemy[i].go = n + 2;
                            else enemy[i].go = 1;
                        } else {
                            if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1 + 2; }
                            else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 4; }
                            else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 3; }
                        }
                    } else { // ==3,4
                        if ((board[y1][x2] != 1) && (board[y3][x2] != 1)) n += 1;
                        if ((board[y1][x3 + 1] != 1) && (board[y3][x3 + 1] != 1)) n += 2;
                        if ((enemy[i].go == 3) && ((board[y2][x1] == 1) || (board[y2][x3] == 1))) {
                            if (n == 3) enemy[i].go = mrand(2) + 1;
                            else if (n > 0) enemy[i].go = n;
                            else enemy[i].go = 4;
                        } else if ((enemy[i].go == 4) && ((board[y3 + 1][x1] == 1) || (board[y3 + 1][x3] == 1))) {
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
                
                // easy enemy move
                if (!alr) {
                    int n1;
                    if (enemy[i].go < 3) { // ==1,2
                        if (board[y2][x1] != 1) n += 1;
                        if (board[y3][x1] != 1) n += 2;
                        if ((enemy[i].go == 1) && (board[y1][x2] == 1)) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else enemy[i].go = n + 2;
                        } else if ((enemy[i].go == 2) && (board[y1][x3] == 1)) {
                            if (n == 3) enemy[i].go = mrand(2) + 3;
                            else if (n > 0) enemy[i].go = n + 2;
                            else enemy[i].go = 1;
                        } else {
                            if (n == 3) { n1 = mrand(3); if (n1 > 0) enemy[i].go = n1 + 2; }
                            else if (n == 2) { n1 = mrand(2); if (n1 > 0) enemy[i].go = 4; }
                            else { n1 = mrand(n); if (n1 > 0) enemy[i].go = 3; }
                        }
                    } else { // ==3,4
                        if (board[y1][x2] != 1) n += 1;
                        if (board[y1][x3] != 1) n += 2;
                        if ((enemy[i].go == 3) && (board[y2][x1] == 1)) {
                            if (n == 3) enemy[i].go = mrand(2) + 1;
                            else if (n > 0) enemy[i].go = n;
                            else enemy[i].go = 4;
                        } else if ((enemy[i].go == 4) && (board[y3][x1] == 1)) {
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
            }
            
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
    }
}
