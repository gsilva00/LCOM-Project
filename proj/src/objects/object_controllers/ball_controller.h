#ifndef BALL_CONTROLLER_H
#define BALL_CONTROLLER_H

#include <lcom/lcf.h>

#include <stdint.h>

#include <devices/timer/aux_timer.h>

#include <objects/ball.h>
#include <objects/scoreboard.h>
#include <objects/goal.h>


#define SPEED_REDUCTION_FACTOR 0.65 /**< @brief Simulates friction - reduces the ball's speed every second it moves  */
#define BOUNCE_SPEED_REDUCTION_FACTOR 0.85 /**< @brief Simulates the energy lost in a collision with the ground - reduces the ball's speed */
#define BOUNCE_OFFSET 50 /**< @brief Simulates the energy lots in a collision with the ground - reduces the height of the bounce after the next collision  */


/** 
 * @brief Declaration of the Ball State Machine
 * @details The state machine toggles the ability of the ball to move along the ground, as well as to jump (whenever it is kicked). Both activities involve a preparation state, the state where the actual action occures, a state that resets the atributes after the action is complete (except atributes that are supposed to change) and a rest state. 
*/
typedef enum {
  STATE_NONE,
  START_JUMP_LEFT,
  BEFORE_JUMP_LEFT,
  JUMP_LEFT,
  AFTER_JUMP_LEFT,
  JUMP_END,
  START_JUMP_RIGHT,
  BEFORE_JUMP_RIGHT,
  JUMP_RIGHT,
  AFTER_JUMP_RIGHT,

  MOVE_LEFT_START,
  MOVE_RIGHT_START,
  MOVE_LEFT,
  MOVE_RIGHT,
  AFTER_MOVE,
} BallState;

/** 
 * @brief Main function that changes the ball's atributes according to the current state
 * @details This is the function that controls every one of the ball's movements and atributes, depending on the state it is in.
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 * @param ball_state Pointer to the main ball state, can vary depending on the action chosen, the phase it's in, and the direction it's going.
 * @param ball_state_temporary Pointer to the auxiliary ball state. Used when an action is requested before the other finishes
 */
void move_ball(Ball *bola, BallState *ball_state, BallState * ball_state_temporary);

/** 
 * @brief Collision detecting function between a ball and a player
 * @details Analyses the ball's position, as well as the player's, and determines if the two items are colliding  
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created)
 * @param pl Pointer to the player object, of type Player (struct we created)
 * @return True if they are colliding, and false if not
 */
bool ball_player_collision(Ball *bola, Player *pl);

/** 
 * @brief Collision detecting function between a ball and a goal
 * @details Analyses the ball's position, as well as the goal's, and determines if the two items are colliding and/or if the ball is inside the net. Given the second case, it signals there has been a goal
 *
 * @param bl Pointer to the ball object, of type Ball (struct we created).
 * @param gl Pointer to the goal object, of type Goal (struct we created).
 * @param sc Pointer to the scoreboard object, of type Scoreboard (struct we created)
 * @param ball_state Pointer to the ball's current state, referring to the state machine BallState
 * @return True if they are colliding, and false if not. In the case of an error it returns false as well
 */
bool ball_goal_collision(Ball *bl, Goal *gl, Scoreboard *sc,BallState * ball_state);

/** 
 * @brief Function that detects if a player is in a position to kick the ball
 * @details Analyses the ball's position, as well as the player's, and determines if the player is in position to kick the ball. It takes into consideration the orientation of the player, as the requirements change according to it
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 * @param pl Pointer to the player object, of type Player (struct we created).
 * @return True if they the kick is available, and false if not
 */
bool check_kicking_player(Ball *bola, Player *pl);

/** 
 * @brief Function that saves the initial values of the ball
 * @details Takes the current values of position and speed of the ball, and saves them in variables. This function is used in the preparation state of the ball's state machine, so that these values are reset in the end. Auxiliary function of move_ball().
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 */
void initialize_ball_values(Ball *bola);

/** 
 * @brief Function that prepares the ball for the jump
 * @details Changes the time passed on every axis and the bounce offset value to prepare the ball for its jump. This function is used in the preparation state of the ball's state machine. Auxiliary function of move_ball().
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 */
void prepare_for_jump(Ball *bola);

/** 
 * @brief Function that updates the ball's position after the jump
 * @details Updates the ball's position and speed values accordingly to the state of the jump it is in. It is used in the saving state of the ball's state machine. Auxiliary function of move_ball().
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 */
void update_ball_position_after_jump(Ball *bola);

/** 
 * @brief Function that performs the ball's jump
 * @details Performs the ball's entire jump, from the beginning to the bouncing in the end. It is the function used in the action state of the state machine. Auxiliary function of move_ball().
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 * @param ball_state Pointer to the ball's current state, refering to the state machine BallState.
 * @param direction Integer to indicate if the jump is going to the left (-1) or the right (1).
 */
void handle_jump(Ball *bola, BallState *ball_state, int direction);

/** 
 * @brief Function that changes the ball's y value along the jump
 * @details Changes the ball's y value according to the ball's current speed values in the y axis, as well as the time spent above the ground. Auxiliary function of handle_jump().
 *
 * @param bola Pointer to the ball object, of type Ball (struct we created).
 */
void change_y(Ball *bola);

#endif
