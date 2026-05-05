#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>     //only here for testing basic layout with sleep()

// Directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define WIDTH 30    //trying to define size
#define HEIGHT 10

typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment *next;
} SnakeSegment;

SnakeSegment *snake = NULL;
int direction = RIGHT;          //starting direction for snake
int foodX, foodY;               //coordinates for food item
int score = 0;                  //player score int value
int maxX, maxY;                 //the outermost x and y values
int snakeLength, goalLength;    //to keep track of current and goal lengths
time_t start;

// Add a new head to the snake
void snake_head(int x, int y) {
    SnakeSegment *new_head = malloc(sizeof(SnakeSegment));  //allocates memory for new snake front
    if (!new_head) {        //if not possible prints stderr information
        endwin();
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    new_head->x = x;        //defines the x and y coordinate
    new_head->y = y;        // for new front of snake
    new_head->next = snake; //defines old front as next segment
    snake = new_head;       //and the new segment as the front
}

void draw_snake(){
    // Draw snake segments at current location
    SnakeSegment *segment = snake;
    while (segment) {
        mvprintw(segment->y, segment->x, "#");
        segment = segment->next;
    }
}

// Remove the tail segment
void remove_tail() {
    SnakeSegment *current = snake;          //looks at current front of snake
    //if (!current || !current->next) return; //if the snakeSegment points to nothing or it has no next segment

    while (current->next->next) {   //checks to see if there is a segment two locations behind the current
        current = current->next;    //if so check again with next segment
    }
    free(current->next);            //otherwise free the memory that the next segment uses 
    current->next = NULL;           // and removes it
}

// Generate random food position
void place_food() {
    foodX = rand() % (maxX - 2) + 1;    //sets the x to a location in bounds
    foodY = rand() % (maxY - 3) + 2;    //and same with the y coordinate
}

// Check if snake collides with itself
int self_collision() {
    int x = snake -> x;                                     //gets the x and y coordinates
    int y = snake -> y;                                     // of the snake's head
    SnakeSegment *segment = snake->next;
    while (segment) {                                       //compares each segment of the snake with the location of the head
        if (segment->x == x && segment->y == y) return 1;   // if one matches it returns a 1 to cause game over
        segment = segment->next;                            //  else keeps checking to end
    }
    return 0;
}

// Free memory allocated to snake
void free_memory() {
    while (snake) {
        SnakeSegment *current = snake;
        snake = snake->next;
        free(current);
    }
}

//function for getting time played
int time_update(){
    time_t current = time(NULL);                //gets the current time
    int elapsed = difftime(current, start);     //compares it to the starting time

    return elapsed;                             //returns the difference
}

int main(){
    initscr();                          //initializes screen
    noecho();                           //stops the window from displaying typed input
    curs_set(FALSE);                    //makes cursor invisible
    keypad(stdscr, TRUE);               //allows arrow input from keypad
    nodelay(stdscr, TRUE);              // sets content to run without delay w/out needs input to move
    //getmaxyx(stdscr, maxY, maxX);
    //refresh();

    /*if(LINES < 21){
        resizeterm(21,COLS);
    }
    if(COLS < 20){
        resizeterm(LINES,20);
    }*/

    if(LINES < 21 || COLS < 20){
        mvprintw(LINES/2, (COLS/2)-17, "Terminal must be at least 21hx20w");
        mvprintw((LINES/2)+1, (COLS/2)-9, "Current: %3dhx%dw", LINES, COLS);
        refresh();
        nodelay(stdscr, FALSE);
        getch();
        endwin();
        exit(0);
        //printf("Terminal must be at least 20x20");
        return 1;
    }

    // define snake pit and create border
    WINDOW *gameWin = newwin(LINES-1,COLS,1,0);     //defines snake pit size
    refresh();
    box(gameWin,0,0);                               //creates border for the snake pit area
    wrefresh(gameWin);
    //nodelay(gameWin, TRUE);
    getmaxyx(gameWin, maxY, maxX);
    refresh();

    //create snake at center
    snake_head((maxX / 2)-2, maxY / 2);
    snake_head((snake->x)+1, (snake->y));
    snake_head((snake->x)+1, (snake->y));

    snakeLength = 3;
    goalLength = maxY+maxX;
    /*snakeLength = goalLength - 2;   //this and next for testing win condition
    score = goalLength - 5;         //and seeing what the winning value would be
    while(snakeLength < (goalLength /5)){  //for testing, remove later
        snake_head((snake->x)+1, (snake->y));
        snakeLength++;
    }*/

    //start screen
    mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 8, "Welcome to Snake");     //Start screen content for the game
    mvwprintw(gameWin, (maxY / 2), (maxX / 2) - 10, "Press Space To Start");    //Tells user to press the space bar to start
    wrefresh(gameWin);
    while(1){               //wait for user input before starting
        if(getch() == ' '){                                     //if user presses the Spacebar
            wclear(gameWin);                                        //clears window
            //Generate initial coordinates for the food item
            place_food();
            
            //record when started
            /*time_t*/ start = time(NULL);                          //initializes start time
            break;
        }
    }

    while(1){
        //clear previous content
        clear();
        refresh();

        //redraw border of snake pit
        box(gameWin,0,0);
        wrefresh(gameWin);

        //Create food item
        mvprintw(foodY, foodX, "O");

        draw_snake();

        // Display score & current play time
        mvprintw(0, 2, "Score: %d", score);     //places score
        int cur_time = time_update();      //gets current play time
        mvprintw(0, COLS - 6, "%2d:%02d", cur_time/60, cur_time%60);    //displays play time in seconds and minutes in top right
        refresh();

        //User Inputs
        int input = getch();        //gets value of user key presses                                //while not going in opposite direction
        if ((input == 'w' || input == KEY_UP) && direction != DOWN) direction = UP;             //if user presses up or equivalent key
        else if ((input == 's' || input == KEY_DOWN) && direction != UP) direction = DOWN;      //if user presses down or equivalent key
        else if ((input == 'a' || input == KEY_LEFT) && direction != RIGHT) direction = LEFT;   //if user presses left or equivalent key
        else if ((input == 'd' || input == KEY_RIGHT) && direction != LEFT) direction = RIGHT;  //if user presses right or equivalent key
        else if (input == 'q' || input == 'Q') break;               //for quitting the game

        // Calculate new location for front of snake
        int frontX = snake->x;      //gets current x of snake head
        int frontY = snake->y;      //gets y of current snake head
        if (direction == UP) frontY--;              //adjusts y coordinte for new head location if going up
        else if (direction == DOWN) frontY++;       // or down
        else if (direction == LEFT) frontX--;       // and adjusts the x coordinate when going left
        else if (direction == RIGHT) frontX++;      // or right

        // Check for game end conditions
        // ( left edge,        right edge,          top,            bottom,      collides with self,        win condition      )
        if (frontX <= 0 || frontX >= maxX - 1 || frontY <= 1 || frontY >= maxY || self_collision() || snakeLength == goalLength) {
            break; // go to Game over/Won screen
        }

        // Generate new front of head
        snake_head(frontX, frontY);

        // Check if food has been eaten
        if (frontX == foodX && frontY == foodY) {   //if food eaten the snake does lose last segment
            score++;                //increment score
            snakeLength++;          // and snake length
            place_food();           // generate new food item coordinates
        } else {
            remove_tail();          // if not eaten the snake loses it's last segment/tail
        }

        napms(100); // Delay for snake speed
    }

    //sleep(1);
    // Game over screen
    wclear(gameWin);        //clear content from snake pit
    //wrefresh(gameWin);      //refresh content for snake pit
    box(gameWin,0,0);       //regenerate box for snake pit
    // mvprintw(maxY / 2, (maxX / 2) - 5, "GAME OVER");
    // mvprintw(maxY / 2 + 1, (maxX / 2) - 7, "Final Score: %d", score);
    if(snakeLength == goalLength){mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 4, "You Win");}   //if win condition met
    else{mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 5, "Game Over");}                          //if win condition not met
    mvwprintw(gameWin, maxY / 2, (maxX / 2) - 7, "Final Score: %d", score);        //displays current score
    //mvprintw(LINES / 2, (COLS / 2) - 5, "GAME OVER");
    mvprintw(0, COLS - 19, "Time Elapsed:");                //adds "Time Elapses: " in front of play time
    wrefresh(gameWin);          //refresh content for game window to show the new content
    refresh();                  //refresh window the show content not in snake pit
    napms(500);    //in case user presses a key at the same time that they lose
    nodelay(stdscr, FALSE);     //add delay for user to see above text
    getch();        //wait for user input before closing
    //sleep(1);
    free_memory();
    endwin();
    exit(0);
    return 0;
}