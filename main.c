#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"
#include "unistd.h"
#include "time.h"

#define MAX(a, b) ((a)>(b) ? (a) : (b))
#define MIN(a, b) ((a)>(b) ? (b) : (a))

// Source - https://stackoverflow.com/a/42500322
// Posted by MD XF, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-22, License - CC BY-SA 3.0
#ifdef _WIN32
#include <conio.h>
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
#endif

#define BOARD_WIDTH  50
#define BOARD_HEIGHT 50

#define SKIN 3

#if (SKIN == 1)
#define S_ALIVE "▪"
#define S_DEAD  "▫"
#elif (SKIN == 2)
#define S_ALIVE "◼"
#define S_DEAD  "◻"
#elif (SKIN == 3)
#define S_ALIVE "🟩"
#define S_DEAD  "🟫"
#else // Default skin
#define S_ALIVE "■"
#define S_DEAD  "□"
#endif

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    bool is_alive;
    bool next_state;
} Cell;

typedef struct {
    Cell cells[BOARD_WIDTH][BOARD_HEIGHT];
    int width;
    int height;
} Board;

int get_num_alive_neighbours(Board *pBoard, int x, int y)
{
    int n_alive = 0;
    for (int j = MAX(0, y - 1); j < MIN(pBoard->height, y + 2); j++) {
        for (int i = MAX(0, x - 1); i < MIN(pBoard->width, x + 2); i++) {
            if ((i == x) && (j == y)) continue;
            n_alive += pBoard->cells[i][j].is_alive;
        }
    }
    return n_alive;
}

bool handle_rules(bool is_alive, int n_alive)
{
    bool ret = is_alive;
    if (is_alive) {
        // overpopulation
        if (n_alive > 3) ret = false;
        // underpopulation/loneliness
        else if (n_alive < 2) ret = false;
    } else {
        // reproduction
        if (n_alive == 3) ret = true; // TODO: Check the rule
    }
    return ret;
}

void generate(Board *pBoard)
{
    srand(time(NULL));
    for (int j = 0; j < pBoard->height; j++) {
        for (int i = 0; i < pBoard->width; i++) {
            pBoard->cells[i][j].is_alive = rand() % 2;
        }
    }
}

void iterate(Board *pBoard)
{
    for (int j = 0; j < pBoard->height; j++) {
        for (int i = 0; i < pBoard->width; i++) {
            pBoard->cells[i][j].next_state = handle_rules(
                    pBoard->cells[i][j].is_alive,
                    get_num_alive_neighbours(pBoard, i, j)
            );
        }
    }
    for (int j = 0; j < pBoard->height; j++) {
        for (int i = 0; i < pBoard->width; i++) {
            pBoard->cells[i][j].is_alive = pBoard->cells[i][j].next_state;
        }
    }
}

void show(Board *pBoard)
{
    for (int j = 0; j < pBoard->height; j++) {
        for (int i = 0; i < pBoard->width; i++) {
            printf(pBoard->cells[i][j].is_alive ? S_ALIVE : S_DEAD);
#if (SKIN != 3)
            printf(" ");
#endif
        }
        printf("\n");
    }
}

void main()
{
    Board board;
    board.width  = BOARD_WIDTH;
    board.height = BOARD_HEIGHT;

    generate(&board);

    for (;;) {
        clrscr();
        show(&board);
        usleep(1000 * 1000);
        iterate(&board);
    }
}
