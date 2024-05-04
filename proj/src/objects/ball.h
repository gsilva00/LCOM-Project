#ifndef BALL_H
#define BALL_H

struct Ball;
typedef struct Ball ball;

ball *create_ball(char *pic[], int x, int y);

void destroy_ball(ball *bl);

int move_ball(ball *bl);

#endif