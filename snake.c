#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>     //only here for testing basic layout with sleep()

// Directions
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define WIDTH 80    //defines the width
#define HEIGHT 20   // and height for the game area

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
int setSize = 0;       // 1 = game area based on terminal size, 0 = game area predefined
WINDOW *gameWin;        //snake pit window
int minWidth, minHeight;    //int values for defining the minimum terminal size

void create_game_window(){
    if(setSize){
        gameWin = newwin(LINES-1,COLS,1,0);         //defines snake pit size based on the terminal size
    } else {
        gameWin = newwin(HEIGHT-1,WIDTH,1,0);       //defines snake pit size based on predefined HEIGHT and WIDTH values
    }
    box(gameWin,0,0);                               //creates border for the snake pit area
    getmaxyx(gameWin, maxY, maxX);                  //gets the height and width for the snake pit
    refresh();
    wrefresh(gameWin);
}

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
    SnakeSegment *segment = snake;              //point to front of snake
    while (segment) {                           //cycle through the subsequent objects attached to it
        mvprintw(segment->y, segment->x, "#");  // and draw each segment at it's x & y coordinates
        segment = segment->next;                // then look at next segment
    }
}

// Remove the tail segment
void remove_tail() {
    SnakeSegment *current = snake;          //looks at current front of snake

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

void term_size(){
   while(LINES < minHeight || COLS < minWidth){
        clear();
        mvprintw(LINES/2, (COLS/2)-17, "Terminal must be at least %02dhx%02dw", minHeight, minWidth);    //notifies user of terminal requirements
        mvprintw((LINES/2)+1, (COLS/2)-9, "Current: %3dhx%dw", LINES, COLS);    // and their current size
        refresh();
        if(getch() == 'q'){   
            free_memory();      //frees the memory allocated to the snake and it's segments                                  //if user presses the Spacebar
            endwin();
            exit(0);
        }
        nodelay(stdscr, FALSE);     //turns back on delay so user can see message
        napms(100);                 //wait before checking again
    }
    clear();
    nodelay(stdscr, TRUE);
}

//function for getting time played
int time_update(){
    time_t current = time(NULL);                //gets the current time
    int elapsed = difftime(current, start);     //compares it to the starting time

    return elapsed;                             //returns the difference
}

void pause_game(){
    clear();
    create_game_window();
    mvwprintw(gameWin, (maxY / 2)-1, (maxX / 2) - 6, "GAME PAUSED");
    mvwprintw(gameWin, (maxY / 2), (maxX / 2) - 12, "Press Space To Continue");    //Tells user to press the space bar to continue
    wrefresh(gameWin);
    while(1){               //wait for user input before starting
        if(getch() == ' '){                                     //if user presses the Spacebar
            break;
        }
    }
}

int main(){
    initscr();                          //initializes screen
    noecho();                           //stops the window from displaying typed input
    curs_set(FALSE);                    //makes cursor invisible
    keypad(stdscr, TRUE);               //allows arrow input from keypad
    nodelay(stdscr, TRUE);              // sets content to run without delay w/out needs input to move

    if(setSize){
        minWidth = 20;
        minHeight = 21;
    } else{
        minWidth = WIDTH;
        minHeight = HEIGHT + 1;
    }
    //check terminal size
    term_size();
    //create snake pit
    create_game_window();

    //create snake at center
    snake_head((maxX / 2)-2, maxY / 2);
    snake_head((snake->x)+1, (snake->y));
    snake_head((snake->x)+1, (snake->y));

    //Generate initial coordinates for the food item
    place_food();

    //defines initial snake length value and goal length
    snakeLength = 3;
    goalLength = maxY+maxX;

    //start screen
    mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 8, "Welcome to Snake");     //Start screen content for the game
    mvwprintw(gameWin, (maxY / 2), (maxX / 2) - 11, "Press Space To Start");    //Tells user to press the space bar to start
    wrefresh(gameWin);
    while(1){               //wait for user input before starting
        if(getch() == ' '){                                     //if user presses the Spacebar
            
            //marks start time
            start = time(NULL);                                 //initializes start time
            break;

        }
    }

    while(1){
        //clear previous content
        wclear(gameWin);                                        //clears start message from window
        clear();
        refresh();

        //if(getch() == 'p'){ pause_game();}

        //check to see if current terminal size meets requirements
        term_size();
        //recreate window if resized before start
        // & redraw border of snake pit
        create_game_window();

        //Create food item
        mvprintw(foodY, foodX, "O");

        //draws the snake
        draw_snake();

        // Display score & current play time
        mvprintw(0, 2, "Score: %d", score);     //places score
        int cur_time = time_update();      //gets current play time
        mvprintw(0, maxX - 6, "%2d:%02d", cur_time/60, cur_time%60);    //displays play time in seconds and minutes in top right
        mvprintw(0, (maxX/2) - 4, "'q' to quit");         //tell user input for ending game
        refresh();

        //User Inputs
        int input = getch();        //gets value of user key presses                                //while not going in opposite direction
        if ((input == 'w' || input == KEY_UP) && direction != DOWN) direction = UP;             //if user presses up or equivalent key
        else if ((input == 's' || input == KEY_DOWN) && direction != UP) direction = DOWN;      //if user presses down or equivalent key
        else if ((input == 'a' || input == KEY_LEFT) && direction != RIGHT) direction = LEFT;   //if user presses left or equivalent key
        else if ((input == 'd' || input == KEY_RIGHT) && direction != LEFT) direction = RIGHT;  //if user presses right or equivalent key
        else if (input == 'p') pause_game();
        else if (input == 'q' || input == 'Q') break;               //for quitting the game
        //else if (input = 'p') pause_game();

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
        if (frontX == foodX && frontY == foodY) {   //if food eaten
            score++;                                // increment score
            snakeLength++;                          // and snake length
            place_food();                           // generate new food item coordinates
        } else {                                    //if not eaten
            remove_tail();                          // snake loses it's last segment/tail
        }

        napms(100); // Delay for snake speed
    }

    // Game over screen
    wclear(gameWin);        //clear content from snake pit
    box(gameWin,0,0);       //regenerate box for snake pit
    if(snakeLength == goalLength){mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 4, "You Win");}   //if win condition met
    else{mvwprintw(gameWin, (maxY / 2) - 1, (maxX / 2) - 5, "Game Over");}                          //if win condition not met
    mvwprintw(gameWin, maxY / 2, (maxX / 2) - 7, "Final Score: %d", score);        //displays current score
    mvprintw(0, (maxX/2) - 4, "            ");      //remove 'q' to quit indicator
    mvprintw(0, maxX - 19, "Time Elapsed:");                //adds "Time Elapses: " in front of play time
    wrefresh(gameWin);          //refresh content for game window to show the new content
    refresh();                  //refresh window the show content not in snake pit
    napms(1000);    //in case user presses a key at the same time that they lose
    nodelay(stdscr, FALSE);     //add delay for user to see above text
    getch();        //wait for user input before closing
    free_memory();      //frees the memory allocated to the snake and it's segments
    endwin();
    exit(0);
    return 0;
}