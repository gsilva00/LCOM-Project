#ifndef PLAYER_CONTROLLER_H
#define PLAYER_CONTROLLER_H

#include <lcom/lcf.h>

#include <stdint.h>

#include <objects/player.h>
#include <devices/timer/aux_timer.h>
#include <devices/gpu/gpu.h>

#include "../../images/objects/personagem_2_parado.xpm"
#include "../../images/objects/personagem_parado.xpm"
#include "../../images/objects/personagem_chutar_frente_1.xpm"
#include "../../images/objects/personagem_chutar_frente_2.xpm"
#include "../../images/objects/personagem_chutar_frente_3.xpm"
#include "../../images/objects/personagem_chutar_frente_4.xpm"
#include "../../images/objects/personagem_chutar_frente_5.xpm"
#include "../../images/objects/personagem_chutar_frente_6.xpm"
#include "../../images/objects/personagem_chutar_tras_1.xpm"
#include "../../images/objects/personagem_chutar_tras_2.xpm"
#include "../../images/objects/personagem_chutar_tras_3.xpm"
#include "../../images/objects/personagem_chutar_tras_4.xpm"
#include "../../images/objects/personagem_2_chutar_frente_1.xpm"
#include "../../images/objects/personagem_2_chutar_frente_2.xpm"
#include "../../images/objects/personagem_2_chutar_frente_3.xpm"
#include "../../images/objects/personagem_2_chutar_frente_4.xpm"
#include "../../images/objects/personagem_2_chutar_frente_5.xpm"
#include "../../images/objects/personagem_2_chutar_frente_6.xpm"
#include "../../images/objects/personagem_2_chutar_tras_1.xpm"
#include "../../images/objects/personagem_2_chutar_tras_2.xpm"
#include "../../images/objects/personagem_2_chutar_tras_3.xpm"
#include "../../images/objects/personagem_2_chutar_tras_4.xpm"


#define SPEED_REDUCTION_FACTOR 0.65

/** 
 * @brief Declaration of the Player Jump State Machine
 * @details This state machine toggles the ability of the player to jump. It involves a preparation state, the state where the actual jump occures, a state that resets the atributes after the action is complete (except atributes that are supposed to change) and a rest state. 
*/
typedef enum {
  PLAYER_JUMP_NONE,
  START_PLAYER_JUMP,
  BEFORE_PLAYER_JUMP,
  PLAYER_JUMP,
  AFTER_PLAYER_JUMP,
  PLAYER_JUMP_END,
} PlayerStateJump;

/** 
 * @brief Declaration of the Player Move State Machine
 * @details This state machine toggles the ability of the player to move. It involves a preparation state, the state where the actual moving occures, a state that resets the atributes after the action is complete (except atributes that are supposed to change) and a rest state. 
*/
typedef enum {
  PLAYER_MOVE_NONE,
  PLAYER_MOVE_LEFT_START,
  PLAYER_MOVE_RIGHT_START,
  PLAYER_MOVE_LEFT,
  PLAYER_MOVE_RIGHT,
  AFTER_PLAYER_MOVE_LEFT,
  AFTER_PLAYER_MOVE_RIGHT,
  PLAYER_MOVE_LEFT_END,
  PLAYER_MOVE_RIGHT_END,
} PlayerStateMove;

/** 
 * @brief Declaration of the Player Jump State Machine
 * @details This state machine toggles the player's animations when kicking. It involves a preparation state, the state where the actual kic occures, a state that resets the atributes after the action is complete (except atributes that are supposed to change) and a rest state. 
*/
typedef enum {
  PLAYER_KICK_NONE,
  PLAYER_KICK_START,
  PLAYER_KICK,
  AFTER_PLAYER_KICK,
} PlayerStateKick;

/** 
 * @brief Main function that changes the player's atributes according to the current state
 * @details This is the function that controls every one of the player's movements and atributes, depending on the state it is in.
 *
 * @param player Pointer to the player object, of type Player (struct we created).
 * @param player_state_move Pointer to the main player move state, can vary depending on the action chosen, the phase it's in, and the direction it's going
 * @param player_state_move_temporary Pointer to the auxiliary player move state. Used when an action is requested before the other finishes
 * @param player_state_jump Pointer to the main player jump state, can vary depending on the action chosen, the phase it's in, and the direction it's going
 * @param player_state_jump_temporary Pointer to the auxiliary player jump state. Used when an action is requested before the other finishes
 * @param player_state_kick Pointer to the main player kick state, can vary depending on the action chosen, the phase it's in, and the direction it's going
 * @param player_state_kick_temporary Pointer to the auxiliary player kick state. Used when an action is requested before the other finishes
 */
void move_player(Player *player, PlayerStateMove *player_state_move, PlayerStateMove *player_state_move_temporary, PlayerStateJump *player_state_jump, PlayerStateJump *player_state_jump_temporary, PlayerStateKick *player_state_kick, PlayerStateKick *player_state_kick_temporary);

/** 
 * @brief Collision detecting function between a player and a border
 * @details Analyses the player's position, and determines if it is colliding with any border
 *
 * @param player Pointer to the player object, of type Player (struct we created).
 * @return True if they are colliding, and false if not
 */
bool player_border_collision(Player *player);

/** 
 * @brief Collision detecting function between a player and another player
 * @details Analyses both players positions, and determines if the two items are colliding  
 *
 * @param player1 Pointer to the first player object, of type Player (struct we created).
 * @param player2 Pointer to the second player object, of type Player (struct we created).
 * @return True if they are colliding, and false if not
 */
bool player_player_collision(Player *player1, Player *player2);

/** 
 * @brief Function that saves the initial values of the player
 * @details Takes the current values of position and speed of the player, and saves them in variables. This function is used in the preparation state of the various player's state machine, so that these values are reset in the end. Auxiliary function of move_player()
 *
 * @param player Pointer to the player object, of type Player (struct we created)
 */
void initialize_player_values(Player *player);

/** 
 * @brief Function that prepares the player for the jump
 * @details Changes the time passed on every axis and the bounce offset value to prepare the player for its jump. This function is used in the preparation state of the player's state machine. Auxiliary function of move_player()
 *
 * @param player Pointer to the player object, of type Player (struct we created)
 */
void prepare_for_player_jump(Player *player);

/** 
 * @brief Function that changes the player's y value along the jump
 * @details Changes the player's y value according to the player's current speed values in the y axis, as well as the time spent above the ground. Auxiliary function of handle_player_jump()
 *
 * @param player Pointer to the player object, of type Player (struct we created)
 */
void change_player_y(Player *player);

/** 
 * @brief Function that performs the player's jump
 * @details Performs the player's entire jump, from the beginning to the bouncing in the end. It is the function used in the action state of the state machine. Auxiliary function of move_player()
 *
 * @param player Pointer to the player object, of type Player (struct we created).
 * @param player_state_jump Pointer to the player's current jump state, refering to the state machine PlayerStateJump.
 */
void handle_player_jump(Player *player, PlayerStateJump *player_state_jump);

/** 
 * @brief Function that resets the values of the player
 * @details Takes the values saved in the beginning by initialize_player_values(), and sets the player's values to those variables, restoring its previous state.
 *
 * @param player Pointer to the player object, of type Player (struct we created).
 */
int restart_player_movement(Player *player);

#endif
