#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
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
int snakeLength, goalLength;    //to keep track of current and goal lengths

//function for getting time played

int time_update(time_t start){
    time_t current = time(NULL);
    int elapsed = difftime(current, start);

    return elapsed;
}

int main(){
    initscr();
    noecho();
    curs_set(FALSE);    //makes cursor invisible
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    //getmaxyx(stdscr, maxY, maxX);
    //refresh();

    //Might remove later, define box
    WINDOW *gameWin = newwin(LINES-1,COLS,1,0);
    refresh();
    box(gameWin,0,0);
    wrefresh(gameWin);
    nodelay(gameWin, TRUE);
    getmaxyx(gameWin, maxY, maxX);
    refresh();

    
    snakeLength = 3;
    goalLength = 2*(maxY+maxX);
    
    //record when started
    time_t start = time(NULL);

    while(1){
        // Display score
        mvprintw(0, 2, "Score: %d", score);
        int cur_time = time_update(start);
        mvprintw(0, COLS - 6, "%2d:%02d", cur_time/60, cur_time%60);
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
    wclear(gameWin);
    wrefresh(gameWin);
    box(gameWin,0,0);
    // mvprintw(maxY / 2, (maxX / 2) - 5, "GAME OVER");
    // mvprintw(maxY / 2 + 1, (maxX / 2) - 7, "Final Score: %d", score);
    if(snakeLength == goalLength){mvprintw(maxY / 2, (maxX / 2) - 4, "You Win");}
    else{mvprintw(maxY / 2, (maxX / 2) - 5, "GAME OVER");}
    mvwprintw(gameWin, LINES / 2, (COLS / 2) - 7, "Final Score: %d", score);
    //mvprintw(LINES / 2, (COLS / 2) - 5, "GAME OVER");
    wrefresh(gameWin);
    refresh();
    nodelay(stdscr, FALSE);
    getch();        //wait for user input before closing
    //sleep(1);
    endwin();
    exit(0);
    return 0;
}