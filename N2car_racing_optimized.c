#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <stdlib.h>

/* Game constants */
#define CAR_WIDTH 30
#define CAR_HEIGHT 50
#define ROAD_WIDTH 200
#define OBSTACLE_WIDTH 30
#define OBSTACLE_HEIGHT 40
#define MAX_OBSTACLES 5

/* Structure for player car */
struct Car {
    int x, y;
    int lane;
    int oldX, oldY; /* For erasing */
};

/* Structure for obstacles */
struct Obstacle {
    int x, y;
    int active;
    int lane;
    int oldY; /* For erasing */
};

/* Global variables */
struct Car player;
struct Obstacle obstacles[MAX_OBSTACLES];
int score = 0;
int oldScore = -1;
int gameOver = 0;
int roadY = 0;
int oldRoadY = 0;
int gameSpeed = 5;
int maxX, maxY;
int roadLeft, roadRight;

/* Function prototypes */
void initGame(void);
void drawRoadOnce(void);
void updateRoadLines(void);
void drawCar(int x, int y, int color);
void eraseCar(int x, int y);
void drawObstacle(int x, int y);
void eraseObstacle(int x, int y);
void updateObstacles(void);
void checkCollisions(void);
void spawnObstacle(void);
void displayScore(void);
void gameOverScreen(void);
int getLaneX(int lane);

int main() {
    int gdriver = DETECT, gmode;
    char key;
    
    /* Initialize graphics */
    initgraph(&gdriver, &gmode, "C:\\TURBOC3\\BGI");
    
    maxX = getmaxx();
    maxY = getmaxy();
    
    /* Initialize game */
    initGame();
    
    /* Draw static elements once */
    drawRoadOnce();
    drawCar(player.x, player.y, YELLOW);
    displayScore();
    
    /* Game loop */
    while (!gameOver) {
        /* Check for keyboard input */
        if (kbhit()) {
            key = getch();
            
            /* Move car left */
            if ((key == 'a' || key == 'A' || key == 75) && player.lane > 0) {
                eraseCar(player.x, player.y);
                player.lane--;
                player.x = getLaneX(player.lane);
                drawCar(player.x, player.y, YELLOW);
            }
            /* Move car right */
            if ((key == 'd' || key == 'D' || key == 77) && player.lane < 2) {
                eraseCar(player.x, player.y);
                player.lane++;
                player.x = getLaneX(player.lane);
                drawCar(player.x, player.y, YELLOW);
            }
            /* Quit game */
            if (key == 27) { /* ESC key */
                gameOver = 1;
            }
        }
        
        /* Update road animation */
        roadY += gameSpeed;
        if (roadY > 40) {
            roadY = 0;
        }
        updateRoadLines();
        
        /* Update game objects */
        updateObstacles();
        checkCollisions();
        
        /* Randomly spawn obstacles - reduced frequency */
        if (rand() % 50 == 0) {
            spawnObstacle();
        }
        
        /* Increase difficulty */
        if (score > 0 && score % 100 == 0 && score != oldScore) {
            if (gameSpeed < 10) {
                gameSpeed++;
            }
            oldScore = score;
        }
        
        /* Update score display only if changed */
        if (score != oldScore && score % 10 == 0) {
            displayScore();
            oldScore = score;
        }
        
        /* Small delay */
        delay(30);
    }
    
    /* Show game over screen */
    gameOverScreen();
    
    getch();
    closegraph();
    return 0;
}

void initGame() {
    int i;
    
    /* Calculate road boundaries */
    roadLeft = maxX / 2 - ROAD_WIDTH / 2;
    roadRight = maxX / 2 + ROAD_WIDTH / 2;
    
    /* Initialize player car */
    player.lane = 1;
    player.x = getLaneX(player.lane);
    player.y = maxY - 100;
    player.oldX = player.x;
    player.oldY = player.y;
    
    /* Initialize obstacles */
    for (i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
        obstacles[i].oldY = -100;
    }
    
    score = 0;
    oldScore = -1;
    gameOver = 0;
    gameSpeed = 5;
}

void drawRoadOnce() {
    int i;
    
    /* Draw grass on sides */
    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    bar(0, 0, roadLeft, maxY);
    bar(roadRight, 0, maxX, maxY);
    
    /* Draw road */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(roadLeft, 0, roadRight, maxY);
    
    /* Draw road edges */
    setcolor(WHITE);
    line(roadLeft, 0, roadLeft, maxY);
    line(roadRight, 0, roadRight, maxY);
}

void updateRoadLines() {
    int i;
    
    /* Erase old lane dividers */
    setcolor(DARKGRAY);
    for (i = oldRoadY; i < maxY; i += 40) {
        line(roadLeft + ROAD_WIDTH / 3, i, roadLeft + ROAD_WIDTH / 3, i + 20);
        line(roadLeft + 2 * ROAD_WIDTH / 3, i, roadLeft + 2 * ROAD_WIDTH / 3, i + 20);
    }
    
    /* Draw new lane dividers */
    setcolor(YELLOW);
    for (i = roadY; i < maxY; i += 40) {
        line(roadLeft + ROAD_WIDTH / 3, i, roadLeft + ROAD_WIDTH / 3, i + 20);
        line(roadLeft + 2 * ROAD_WIDTH / 3, i, roadLeft + 2 * ROAD_WIDTH / 3, i + 20);
    }
    
    oldRoadY = roadY;
}

void drawCar(int x, int y, int color) {
    /* Simple rectangle-based car - no floodfill */
    
    /* Main body */
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    bar(x, y + 10, x + CAR_WIDTH, y + CAR_HEIGHT);
    
    /* Car top (window) */
    setcolor(LIGHTBLUE);
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(x + 5, y + 15, x + CAR_WIDTH - 5, y + 30);
    
    /* Wheels */
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    /* Left wheels */
    bar(x + 2, y + 12, x + 8, y + 18);
    bar(x + 2, y + CAR_HEIGHT - 8, x + 8, y + CAR_HEIGHT - 2);
    /* Right wheels */
    bar(x + CAR_WIDTH - 8, y + 12, x + CAR_WIDTH - 2, y + 18);
    bar(x + CAR_WIDTH - 8, y + CAR_HEIGHT - 8, x + CAR_WIDTH - 2, y + CAR_HEIGHT - 2);
}

void eraseCar(int x, int y) {
    /* Erase with road color */
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    bar(x - 2, y + 10, x + CAR_WIDTH + 2, y + CAR_HEIGHT + 2);
}

void drawObstacle(int x, int y) {
    drawCar(x, y, RED);
}

void eraseObstacle(int x, int y) {
    eraseCar(x, y);
}

void updateObstacles() {
    int i;
    
    for (i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            /* Erase old position */
            eraseObstacle(obstacles[i].x, obstacles[i].oldY);
            
            /* Update position */
            obstacles[i].oldY = obstacles[i].y;
            obstacles[i].y += gameSpeed;
            
            /* Deactivate if off screen */
            if (obstacles[i].y > maxY) {
                eraseObstacle(obstacles[i].x, obstacles[i].y);
                obstacles[i].active = 0;
                score += 10;
            } else {
                /* Draw at new position */
                drawObstacle(obstacles[i].x, obstacles[i].y);
            }
        }
    }
}

void checkCollisions() {
    int i;
    
    for (i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            /* Optimized collision detection - check lane first */
            if (obstacles[i].lane == player.lane) {
                /* Then check Y overlap */
                if (obstacles[i].y + OBSTACLE_HEIGHT >= player.y &&
                    obstacles[i].y <= player.y + CAR_HEIGHT) {
                    
                    /* Then precise X check */
                    if (obstacles[i].x + OBSTACLE_WIDTH >= player.x &&
                        obstacles[i].x <= player.x + CAR_WIDTH) {
                        gameOver = 1;
                        return; /* Exit early on collision */
                    }
                }
            }
        }
    }
}

void spawnObstacle() {
    int i, j;
    int lane;
    int occupiedLanes[3] = {0, 0, 0}; /* Track which lanes have obstacles near top */
    int availableLanes[3];
    int availableCount = 0;
    int canSpawn = 0;
    
    /* Check which lanes are occupied near the top of screen */
    for (i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active && obstacles[i].y < 100) {
            occupiedLanes[obstacles[i].lane] = 1;
        }
    }
    
    /* Count available lanes and store them */
    for (i = 0; i < 3; i++) {
        if (occupiedLanes[i] == 0) {
            availableLanes[availableCount] = i;
            availableCount++;
        }
    }
    
    /* Only spawn if at least 2 lanes are free (keep 1 lane always open) */
    if (availableCount < 2) {
        return;
    }
    
    /* Find inactive obstacle slot */
    for (i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            /* Choose randomly from available lanes only */
            lane = availableLanes[rand() % availableCount];
            obstacles[i].lane = lane;
            obstacles[i].x = getLaneX(lane);
            obstacles[i].y = -OBSTACLE_HEIGHT;
            obstacles[i].oldY = obstacles[i].y;
            obstacles[i].active = 1;
            canSpawn = 1;
            break;
        }
    }
}

int getLaneX(int lane) {
    int laneWidth = ROAD_WIDTH / 3;
    return roadLeft + lane * laneWidth + (laneWidth - CAR_WIDTH) / 2;
}

void displayScore() {
    char scoreText[50];
    char speedText[50];
    
    /* Erase old text area */
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    bar(5, 5, 250, 80);
    
    /* Draw new text */
    setcolor(WHITE);
    sprintf(scoreText, "Score: %d", score);
    outtextxy(10, 10, scoreText);
    
    sprintf(speedText, "Speed: %d", gameSpeed);
    outtextxy(10, 30, speedText);
    
    outtextxy(10, 50, "A/D or Arrows");
    outtextxy(10, 65, "ESC - Quit");
}

void gameOverScreen() {
    char finalScore[50];
    
    cleardevice();
    
    /* Draw final road */
    drawRoadOnce();
    
    /* Draw crashed car */
    drawCar(player.x, player.y, RED);
    
    /* Draw explosion effect */
    setcolor(YELLOW);
    circle(player.x + CAR_WIDTH/2, player.y + CAR_HEIGHT/2, 30);
    circle(player.x + CAR_WIDTH/2, player.y + CAR_HEIGHT/2, 35);
    setcolor(RED);
    circle(player.x + CAR_WIDTH/2, player.y + CAR_HEIGHT/2, 25);
    
    /* Game over text */
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, BLACK);
    bar(150, 180, 450, 280);
    
    setcolor(RED);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(200, 200, "GAME OVER!");
    
    setcolor(YELLOW);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    sprintf(finalScore, "Final Score: %d", score);
    outtextxy(220, 230, finalScore);
    
    setcolor(WHITE);
    outtextxy(180, 260, "Press any key...");
}
