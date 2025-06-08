#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int WIDTH = 60;
const int HEIGHT = 20;
const int GAP = 8;

int score;
bool gameOver;
int birdY;
int velocity;

struct Pipe {
    int x;
    int gapY;
};

vector<Pipe> pipes;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void draw() {
    COORD cursorPos = {0, 0};
    SetConsoleCursorPosition(hConsole, cursorPos);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            bool printed = false;

            if (x == 5) {
                if (y == birdY - 2) { cout << " __ "; x += 3; printed = true; }
                else if (y == birdY - 1) { cout << "/-/o\\"; x += 4; printed = true; }
                else if (y == birdY)     { cout << "\\_\\-/"; x += 4; printed = true; }
            }

            if (!printed) {
                for (auto& p : pipes) {
                    if (x == p.x) {
                        if (y < p.gapY || y >= p.gapY + GAP) {
                            cout << "|";
                            printed = true;
                            break;
                        }
                    }
                }
            }

            if (!printed) cout << " ";
        }
        cout << "\n";
    }

    for (int i = 0; i < WIDTH; i++) cout << "_";
    cout << "\n";
    for (int i = 0; i < WIDTH; i++) cout << "/";
    cout << "\n";

    cout << "\nScore: " << score << "\n";

    // Debug info, uncomment if needed
    // cout << "Bird Y: " << birdY << " Velocity: " << velocity << "\n";
}

void logic() {
    velocity += 1;
    birdY += velocity;

    // Clamp bird position so bird top doesn't go above 0
    if (birdY < 2) {
        birdY = 2;
        velocity = 0;
    }

    // If bird bottom goes below ground, game over
    if (birdY > HEIGHT - 1) {
        gameOver = true;
        return;
    }

    for (auto& p : pipes) {
        p.x--;

        // Check collision when pipe is at bird X position
        if (p.x == 5) {
            // Bird occupies lines birdY-2 to birdY (3 lines)
            // If bird top or middle or bottom is NOT inside pipe gap → collision
            if (birdY - 2 < p.gapY || birdY >= p.gapY + GAP) {
                gameOver = true;
                return;
            } else {
                score++;
            }
        }
    }

    // Remove pipes that went off screen
    if (!pipes.empty() && pipes[0].x < -1) {
        pipes.erase(pipes.begin());
    }

    // Add new pipes spaced out
    if (pipes.empty() || pipes.back().x < WIDTH - 20) {
        Pipe p;
        p.x = WIDTH - 1;
        p.gapY = rand() % (HEIGHT - GAP - 3) + 2;
        pipes.push_back(p);
    }
}

void resetGame() {
    score = 0;
    birdY = HEIGHT / 2;
    velocity = 0;
    gameOver = false;
    pipes.clear();
}

int main() {
    srand(time(0));
    char playAgain;

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    do {
        resetGame();

        while (!gameOver) {
            draw();

            if (_kbhit()) {
                char c = _getch();
                if (c == ' ' || c == 'w' || c == 'W') {
                    velocity = -2;
                }
            }

            logic();
            Sleep(100);
        }

        cursorInfo.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        cout << "\nDo you want to play again? (Y/N): ";
        cin >> playAgain;
        playAgain = tolower(playAgain);

        if (playAgain == 'y') {
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(hConsole, &cursorInfo);
        }

    } while (playAgain == 'y');

    cout << "Thanks for playing!\n";
    return 0;
}