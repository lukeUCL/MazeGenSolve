#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "graphics.h"
#include <stdbool.h>
#include <limits.h>

#define WIDTH 20
#define HEIGHT 20
#define cellSize 20


struct point {
    int x;
    int y;
};

// Define the directions and their corresponding offsets
const int dx[4] = { 0, 1, 0, -1 };
const int dy[4] = { -1, 0, 1, 0 };


int maze[WIDTH][HEIGHT];
struct point offsets[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
void shuffle(int *array, size_t n) {
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void makeEmpty(int x, int y) {
    setColour(white);
    fillRect(x, y, cellSize, cellSize);
}

void makeWall(int x, int y, int w, int h) {
    setColour(black);
    fillRect(x, y, w, h);
}

void carve_maze(int x, int y) {
    maze[x][y] = 1; // Mark the current cell as a path

    int indices[4] = {0, 1, 2, 3};
    shuffle(indices, 4);
    for (int i = 0; i < 4; i++) {
        struct point p = offsets[indices[i]];
        int dx = x + 2 * p.x;  // Move two cells in the desired direction
        int dy = y + 2 * p.y;

        if (dx >= 0 && dx < WIDTH && dy >= 0 && dy < HEIGHT && !maze[dx][dy]) {
            maze[x + p.x][y + p.y] = 1;  // Mark the intermediate cell as a path
            carve_maze(dx, dy);
        }
    }
}

void draw_maze() {
    int i, j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            int x = i * cellSize;
            int y = j * cellSize;
            if (maze[i][j]) {
                makeEmpty(x, y);
            } else {
                makeWall(x, y, cellSize, cellSize);
            }
        }
    }
    // Add entrance and exit
    setColour(green);
    fillRect(0, 0, cellSize, cellSize);
    setColour(red);
    fillRect((WIDTH - 2) * cellSize, (HEIGHT - 2) * cellSize, cellSize, cellSize);
}

typedef struct {
    int x;
    int y;
    int distance;
} Cell;

struct Predecessor {
    int x, y;
} pred[WIDTH][HEIGHT];


// comparison for pq
int compare_cells(const void *a, const void *b) {
    return ((Cell *)a)->distance - ((Cell *)b)->distance;
}

// bounds checking
bool isValid(int x, int y) {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

void draw_path(int startX, int startY, int endX, int endY) {
    int x = endX, y = endY;
    while (!(x == startX && y == startY)) {
        struct Predecessor p = pred[x][y];
        

        setColour(blue);  
        fillRect(x * cellSize, y * cellSize, cellSize, cellSize);

        
        x = p.x;
        y = p.y;
    }
}


void solve_maze() {
    bool visited[WIDTH][HEIGHT] = {false};
    int distance[WIDTH][HEIGHT];

    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++) {
            distance[i][j] = INT_MAX;
            pred[i][j].x = -1;  // Initialize the pred array with invalid values
            pred[i][j].y = -1;
        }

    distance[0][0] = 0;

    Cell pq[WIDTH * HEIGHT];
    int pqSize = 0;

    pq[pqSize++] = (Cell){0, 0, 0};

    while (pqSize) {
        // Pop the cell with the shortest distance from the priority queue
        qsort(pq, pqSize, sizeof(Cell), compare_cells);
        Cell current = pq[--pqSize];

        if (visited[current.x][current.y]) continue;
        visited[current.x][current.y] = true;

        // Draw the cell for visualization
        setColour(yellow);  
        fillRect(current.x * cellSize, current.y * cellSize, cellSize, cellSize);
        sleep(10);  

        // If we reached the exit
        if (current.x == WIDTH - 2 && current.y == HEIGHT - 2) {
            draw_path(0, 0, WIDTH - 2, HEIGHT - 2);
            return;
        }

        for (int i = 0; i < 4; i++) {
            int newX = current.x + offsets[i].x;
            int newY = current.y + offsets[i].y;

            if (isValid(newX, newY) && !visited[newX][newY] && maze[newX][newY] == 1) {  // Check if it's a path
                int newDistance = current.distance + 1;
                if (newDistance < distance[newX][newY]) {
                    distance[newX][newY] = newDistance;
                    pred[newX][newY] = (struct Predecessor){current.x, current.y};
                    pq[pqSize++] = (Cell){newX, newY, newDistance};
                }
            }
        }
    }

    printf("No path found!\n");
}

// void print_pred_array() {
//     for (int i = 0; i < WIDTH; i++) {
//         for (int j = 0; j < HEIGHT; j++) {
//             printf("pred[%d][%d] = (%d, %d)\n", i, j, pred[i][j].x, pred[i][j].y);
//         }
//     }
// }



int main(int argc, char *argv[]) {
    srand(time(NULL));
    setWindowSize(WIDTH * cellSize, HEIGHT * cellSize);  // Set the canvas size
    carve_maze(0, 0);
    draw_maze();
    solve_maze();

    return 0;
}

