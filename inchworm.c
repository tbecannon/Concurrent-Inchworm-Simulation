//Thomas Becannon
// Nov-3-2025 Last modified


#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>
#include <semaphore.h>

typedef struct Inchworm {
    int head[2];
    int body[3][2];
    int tail[2];
    bool scrunched;
    int direction;
    
    void(*return_)(const struct Inchworm*);
    void(*set_head)(struct Inchworm*, int, int);
    void(*set_body)(struct Inchworm*, int, int, int);
    void(*set_tail)(struct Inchworm*, int, int);
    void(*set_scrunched)(struct Inchworm*, bool);  
    void(*set_direction)(struct Inchworm*, int); 
} Inchworm;

void set_head(Inchworm* worm_instance, int x, int y) {
    worm_instance->head[0] = x;
    worm_instance->head[1] = y;
}

void set_body(Inchworm* worm_instance, int part, int x, int y) {
    worm_instance->body[part][0] = x;
    worm_instance->body[part][1] = y;
}

void set_tail(Inchworm* worm_instance, int x, int y) {
    worm_instance->tail[0] = x;
    worm_instance->tail[1] = y;
}

void set_scrunched(Inchworm* worm_instance, bool isScrunched) {
    worm_instance->scrunched = isScrunched;
}

void set_direction(Inchworm* worm_instance, int direction) {
    worm_instance->direction = direction;
}

//global variables 60x60 matrix and semaphore
char **screenMatrix;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//update worm on the matrix given worm struct instance
void updateScreenMatrix(Inchworm* worm_instance) {
    // head of worm in matrix gets @
    screenMatrix[worm_instance->head[0]][worm_instance->head[1]] = '@';
    
    //always print the middle body part because it is the body part that gets
    // printed in a scrunched position.
    screenMatrix[worm_instance->body[1][0]][worm_instance->body[1][1]] = '#';

    // if the worm is not scrunched
    if (worm_instance->scrunched == false) {
        //print the first and the last body part
        screenMatrix[worm_instance->body[0][0]][worm_instance->body[0][1]] = '#';
        screenMatrix[worm_instance->body[2][0]][worm_instance->body[2][1]] = '#';
    }
    // print the tail
    screenMatrix[worm_instance->tail[0]][worm_instance->tail[1]] = '#';
}

// function to "remove" worm from the matrix
void clearWormFromMatrix(Inchworm* worm_instance) {
    screenMatrix[worm_instance->head[0]][worm_instance->head[1]] = ' ';
    screenMatrix[worm_instance->body[1][0]][worm_instance->body[1][1]] = ' ';
    screenMatrix[worm_instance->body[0][0]][worm_instance->body[0][1]] = ' ';
    screenMatrix[worm_instance->body[2][0]][worm_instance->body[2][1]] = ' ';
    screenMatrix[worm_instance->tail[0]][worm_instance->tail[1]] = ' ';
}

//Descrunch functions to handle the movement of worm as it goes 
//from scrunched to extended, depending on the direction!
void descrunchD0(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    myWorm->set_head(myWorm, myWorm->head[0], myWorm->head[1]+2);
    myWorm->set_tail(myWorm, myWorm->head[0], myWorm->head[1]-4);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0], myWorm->tail[1]+(i+1));
    }
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD1(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0]-(1+i), myWorm->tail[1]+(1+i));
    }
    myWorm->set_head(myWorm, myWorm->tail[0]-4, myWorm->tail[1]+4);
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD2(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    myWorm->set_head(myWorm, myWorm->head[0]-2, myWorm->head[1]);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->head[0]+(1+i), myWorm->head[1]);
    }
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD3(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    myWorm->set_head(myWorm, myWorm->tail[0]-4, myWorm->tail[1]-4);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0]-(1+i), myWorm->tail[1]-(1+i));
    }
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD4(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    myWorm->set_head(myWorm, myWorm->head[0], myWorm->head[1]-2);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->head[0], myWorm->head[1]+(1+i));
    }
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD5(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0]+(1+i), myWorm->tail[1]-(1+i));
    }
    myWorm->set_head(myWorm, myWorm->tail[0]+4, myWorm->tail[1]-4);
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD6(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0]+(1+i), myWorm->tail[1]);
    }
    myWorm->set_head(myWorm, myWorm->head[0]+2, myWorm->head[1]);
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

void descrunchD7(Inchworm* myWorm) {
    pthread_mutex_lock(&lock);
    clearWormFromMatrix(myWorm);
    myWorm->set_scrunched(myWorm, false);
    for (int i = 0; i < 3; i++) {
        myWorm->set_body(myWorm, i, myWorm->tail[0]+i+1, myWorm->tail[1]+i+1);
    }
    myWorm->set_head(myWorm, myWorm->tail[0]+4, myWorm->tail[1]+4);
    updateScreenMatrix(myWorm);
    pthread_mutex_unlock(&lock);
    usleep(300000);
}

//displays changes to the screen
void *thread_toScreen(void* arg) {
    while(1) {
        pthread_mutex_lock(&lock);
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                move(i, j);
                printw("%c", screenMatrix[i][j]);
            }
        }
        refresh();
        pthread_mutex_unlock(&lock);
        usleep(200000);
    }
    return NULL;
}

// manages the internal state of one inchworm
void *thread_inchwormStateManager(void* arg) {
    
    srand((unsigned int)time(NULL) + (unsigned long)pthread_self());
    // create inchworm and set up methods
    Inchworm myWorm;
    myWorm.set_head = set_head;
    myWorm.set_body = set_body;
    myWorm.set_tail = set_tail;
    myWorm.set_scrunched = set_scrunched;
    myWorm.set_direction = set_direction;

    // place inchworm on screen relative to others
        for (int j = 10; j < 60; j=j+12) {
            if (screenMatrix[j][20] == ' ') {
                myWorm.set_head(&myWorm, j, 20);
                screenMatrix[j][20] = '@';
                for (int x = 0; x < 3; x++) {
                    myWorm.set_body(&myWorm, x, j, 20+(x+1));
                    screenMatrix[j][20+(x+1)] = '#';
                }
                myWorm.set_tail(&myWorm, j, 20+4);
                screenMatrix[j][20+4] ='#';
                break;
            }
        }
    sleep(1);
    myWorm.set_direction(&myWorm, 4);
    myWorm.set_scrunched(&myWorm, false);

    // time to decide where it will move
    while(true) {

        pthread_mutex_lock(&lock);
        clearWormFromMatrix(&myWorm);

        int randomNum = rand() % 4;

        int sideCheck = 0;

        // if within 10 positions of a side
        if (myWorm.head[0] - 10 <= 0 || myWorm.head[0] + 10 >= 60 ||
            myWorm.head[1] - 10 <= 0 || myWorm.head[1] + 10 >= 60) {
                randomNum = 2;
                sideCheck = 1;

        }

        int foundWormRow = -999;
        int foundWormCol = -999;
        int breaker = 0;
        if (sideCheck == 0) {
        // detect near by worms
            for (int r = 0; r < 5; ++r) {
                for (int c = 0; c < 5; ++c) {
                    // quadrant 1
                    if (screenMatrix[myWorm.head[0] - r][myWorm.head[1] + c] != ' ') {
                        foundWormRow = myWorm.head[0] - r;
                        foundWormCol = myWorm.head[1] + c;

                        if (myWorm.direction == 0 || myWorm.direction == 1) {
                            randomNum = 3;
                        }
                        else if (myWorm.direction == 2 || myWorm.direction == 3) {
                            randomNum = 2;
                        }
                        else {
                            randomNum = 1;
                        }
                        breaker = 1;
                        break;
                    }   
                    // quadrant 2
                    if (screenMatrix[myWorm.head[0] - r][myWorm.head[1] - c] != ' ') {
                        foundWormRow = myWorm.head[0] - r;
                        foundWormCol = myWorm.head[1] - c;

                        if (myWorm.direction == 2 || myWorm.direction == 3) {
                            randomNum = 3;
                        }
                        else if (myWorm.direction == 4 || myWorm.direction == 5) {
                            randomNum = 2;
                        }
                        else {
                            randomNum = 1;
                        }
                        breaker = 1;
                        break;
                    }
                    // quadrant 3
                    if (screenMatrix[myWorm.head[0] + r][myWorm.head[1] - c] != ' ') {
                        foundWormRow = myWorm.head[0] + r;
                        foundWormCol = myWorm.head[1] + c;

                        if (myWorm.direction == 4 || myWorm.direction == 5) {
                            randomNum = 3;
                        }
                        else if (myWorm.direction == 6 || myWorm.direction == 7) {
                            randomNum = 2;
                        }
                        else {
                            randomNum = 1;
                        }
                        breaker = 1;
                        break;
                    }
                    // quadrant 4
                    if (screenMatrix[myWorm.head[0] + r][myWorm.head[1] + c] != ' ') {
                        foundWormRow = myWorm.head[0] + r;
                        foundWormCol = myWorm.head[1] - c;
                        if (myWorm.direction == 6 || myWorm.direction == 7) {
                            randomNum = 3;
                        }
                        else if (myWorm.direction == 0 || myWorm.direction == 1) {
                            randomNum = 2;
                        }
                        else {
                            randomNum = 1;
                        }
                        breaker = 1;
                        break;
                    }
                }
                if (breaker == 1) {
                    break;
                }
            }

            //decide where to move based on where the nearest worm piece is (if worm in radius)

        }


        //modify direction if randomNum is 2 or 3
        if (randomNum == 2) {
            if (myWorm.direction == 7) {
                myWorm.direction = 0;
            }
            else {
                myWorm.direction++;
            }
        }
        else if (randomNum == 3) {
            if (myWorm.direction == 0) {
                myWorm.direction = 7;
            }
            else {
                myWorm.direction--;
            }
        }
        

        //movement handling...
        //0 degrees
        if (myWorm.direction == 0) {
            
            //rotating from 315 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]-1);
                myWorm.set_head(&myWorm, myWorm.head[0]-2, myWorm.head[1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 45 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]-1);
                myWorm.set_head(&myWorm, myWorm.head[0]+2, myWorm.head[1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            if (randomNum < 2) {
                //scrunch
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]-1, myWorm.tail[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD0(&myWorm);
        }

        //45 degrees
        if (myWorm.direction == 1) {
            
            //rotating from 0 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0], myWorm.tail[1]+1);
                myWorm.set_head(&myWorm, myWorm.tail[0]-1, myWorm.tail[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 90 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]-1, myWorm.tail[1]);
                myWorm.set_head(&myWorm, myWorm.body[1][0], myWorm.body[1][1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //continuing straight
            if (randomNum < 2) {
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.head[0]+2, myWorm.head[1]-2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
        descrunchD1(&myWorm);
        }

        //90 degrees
        if (myWorm.direction == 2) {

            // rotating from 45 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_head(&myWorm, myWorm.head[0], myWorm.head[1]-2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]+1);
                myWorm.set_tail(&myWorm, myWorm.head[0]+2, myWorm.head[1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 135 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_head(&myWorm, myWorm.head[0], myWorm.head[1]+2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]-1);
                myWorm.set_tail(&myWorm, myWorm.head[0]+2, myWorm.head[1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            if (randomNum < 2) {
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD2(&myWorm);
        }

        //135 degrees
        if (myWorm.direction == 3) {

            //rotating from 90 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]-1, myWorm.tail[1]);
                myWorm.set_head(&myWorm, myWorm.tail[0]-1, myWorm.tail[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 180 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0], myWorm.tail[1]-1);
                myWorm.set_head(&myWorm, myWorm.body[1][0]-1,myWorm.body[1][1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //continuing straight
            if (randomNum < 2) {
                clearWormFromMatrix(&myWorm);
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.head[0]+2, myWorm.head[1]+2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]+1, myWorm.head[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD3(&myWorm);
        }

        //180 degrees
        if (myWorm.direction == 4) {

            // rotating from 135 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]-1, myWorm.tail[1]-1);
                myWorm.set_head(&myWorm, myWorm.tail[0], myWorm.tail[1]-2);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 225 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.head[0]-2, myWorm.head[1]+2);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]+1, myWorm.tail[1]-1);
                myWorm.set_head(&myWorm, myWorm.tail[0], myWorm.tail[1]-2);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            if (randomNum < 2) {
                //scrunch
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]-1, myWorm.tail[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD4(&myWorm);
        }

        //225 degrees
        if (myWorm.direction == 5) {
            
            //rotating from 180 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_head(&myWorm, myWorm.head[0]+1, myWorm.head[1]+3);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]);
                myWorm.set_tail(&myWorm, myWorm.head[0]-1, myWorm.head[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 270 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]+1, myWorm.tail[1]);
                myWorm.set_head(&myWorm, myWorm.body[1][0], myWorm.body[1][1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //continuing straight
            if (randomNum < 2) {
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.head[0]-2, myWorm.head[1]+2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD5(&myWorm);
        }

        //270 degrees
        if (myWorm.direction == 6) {
            
            //rotating from 225 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_head(&myWorm, myWorm.head[0], myWorm.head[1]+2);
                myWorm.set_tail(&myWorm, myWorm.head[0]-2, myWorm.head[1]);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 315 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]+1, myWorm.tail[1]+1);
                myWorm.set_head(&myWorm, myWorm.tail[0]+2, myWorm.tail[1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            if (randomNum < 2) {
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.body[1][0], myWorm.body[1][1]);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD6(&myWorm);
        }

        //315 degrees
        if (myWorm.direction == 7) {
            
            //rotating from 270 degrees
            if (randomNum == 2) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0]+1, myWorm.tail[1]);
                myWorm.set_head(&myWorm, myWorm.tail[0]+1, myWorm.tail[1]+1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //rotating from 0 degrees
            if (randomNum == 3) {
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_body(&myWorm, 1, myWorm.tail[0], myWorm.tail[1]+1);
                myWorm.set_head(&myWorm, myWorm.body[1][0]+1, myWorm.body[1][1]);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }

            //continuing straight

            if (randomNum < 2) {
                // pthread_mutex_lock(&lock);
                clearWormFromMatrix(&myWorm);
                //scrunch
                myWorm.set_scrunched(&myWorm, true);
                myWorm.set_tail(&myWorm, myWorm.head[0]-2, myWorm.head[1]-2);
                myWorm.set_body(&myWorm, 1, myWorm.head[0]-1, myWorm.head[1]-1);
                updateScreenMatrix(&myWorm);
                pthread_mutex_unlock(&lock);
                usleep(300000);
            }
            descrunchD7(&myWorm);
        }

        usleep(300000); // 0.2 seconds per move        
    }
    return NULL;
}

int main() {

    //initialize the 4 threads
    pthread_t screenThread;
    pthread_t wormThread1, wormThread2, wormThread3, wormThread4;

    //set up semaphore
    void *thread_result;

   
    //initalize screen and matrix
    initscr();
   
    screenMatrix = malloc(60*sizeof(char*));
    for (int i = 0; i < 60; i++) {
        screenMatrix[i] = malloc(60*sizeof(char));
        for (int j = 0; j < 60; j++) {
            screenMatrix[i][j] = ' ';
        }
    }    
    
    //create screen and worm thread using thread functions
    pthread_create(&screenThread, NULL, thread_toScreen, NULL);
    pthread_create(&wormThread1, NULL, thread_inchwormStateManager, NULL);
    pthread_create(&wormThread2, NULL, thread_inchwormStateManager, NULL);
    pthread_create(&wormThread3, NULL, thread_inchwormStateManager, NULL);

    //join them
    pthread_join(screenThread, NULL);
    pthread_join(wormThread1, NULL);
    pthread_join(wormThread2, NULL);
    pthread_join(wormThread3, NULL);  

    endwin();
    pthread_mutex_destroy(&lock);
    // free memory
     for (int i = 0; i < 60; i++) {
         free(screenMatrix[i]);
     }
    free(screenMatrix);
    return EXIT_SUCCESS;
}