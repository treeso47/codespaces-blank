#include <ncurses.h>
#include <stdlib.h>
//#include <time.h>
#include <unistd.h>     //only here for testing basic layout with sleep()

// Directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment *next;
} SnakeSegment;

SnakeSegment *snake = NULL;
int direction = RIGHT;
int foodX, foodY;
int score = 0;
int maxX, maxY;

int main(){
    initscr();
    noecho();
    curs_set(FALSE);    //makes cursor invisible
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    //getmaxyx(stdscr, maxY, maxX);
    //refresh();

    //Might remove later, define box
    WINDOW *a = newwin(LINES-1,COLS,1,0);
    refresh();
    box(a,0,0);
    wrefresh(a);
    nodelay(a, TRUE);
    getmaxyx(a, maxY, maxX);
    refresh();

    while(1){
        // Display score
        mvprintw(0, 2, "Score: %d", score);
        //mvprintw(maxY/2, (maxX/2)-9, "height: %d width: %d", maxX, maxY);  //testing for issues with maxX & maxY
        refresh();

        // checking text input locations, looks like must be before if for input
        // mvaddstr(6,50,"as");
        // refresh();
        // mvwaddstr(a, maxY/2,maxX/2,"as");       //maxY not working
        // wrefresh(a);

        //User Inputs
        int input = getch();
        if ((input == 'w' || input == KEY_UP) && direction != DOWN) direction = UP;
        else if ((input == 's' || input == KEY_DOWN) && direction != UP) direction = DOWN;
        else if ((input == 'a' || input == KEY_LEFT) && direction != RIGHT) direction = LEFT;
        else if ((input == 'd' || input == KEY_RIGHT) && direction != LEFT) direction = RIGHT;
        else if (input == 'q' || input == 'Q') break;
    }

    //sleep(1);
    // Game over screen
    clear();
    refresh();
    box(a,0,0);
    mvprintw(maxY / 2, (maxX / 2) - 5, "GAME OVER");
    mvprintw(maxY / 2 + 1, (maxX / 2) - 7, "Final Score: %d", score);
    // mvwprintw(a, LINES / 2 - 1, (COLS / 2) - 5, "GAME OVER");       //temporary solution
    // mvwprintw(a, LINES / 2, (COLS / 2) - 7, "Final Score: %d", score);
    //mvprintw(LINES / 2, (COLS / 2) - 5, "GAME OVER");
    wrefresh(a);
    refresh();
    nodelay(stdscr, FALSE);
    getch();        //wait for user input before closing
    //sleep(1);
    endwin();
    exit(0);
    return 0;
}