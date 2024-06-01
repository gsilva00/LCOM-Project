#ifndef PLAYER_H
#define PLAYER_H

#include <lcom/lcf.h>

/**
 * @struct Player
 * @brief Implementation of the Player as an object usable in game
 * 
 * @var Player::x
 * Value of the Player's position in the x axis
 * 
 * @var Player::y
 * Value of the Player's position in the y axis
 * 
 * @var Player::width
 * Value of the Player's image width
 * 
 * @var Player::height
 * Value of the Player's image height
 * 
 * @var Player::xspeed
 * Value of the Player's speed in the x axis
 * 
 * @var Player::yspeed
 * Value of the Player's speed in the y axis
 * 
 * @var Player::map
 * Pointer to the Player's xpm map
 * 
 * @var Player::orientation
 * Variable of the orientation of the player
 * 
 * @var Player::img
 * Value of the Player's img
 */
typedef struct {
  int x, y;
  int width, height;
  int xspeed, yspeed;
  uint8_t *map;
  bool orientation; 
  xpm_image_t img;
} Player;

/** 
 * @brief Constructor of the Player "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a Player instance, effectively initializing the object
 *
 * @param pic XPM map used to make the Player's image
 * @param x Value of the Player's position in the x axis
 * @param y Value of the Player's position in the y axis
 * @param width Value of the Player's image width
 * @param height Value of the Player's image height
 * @param xspeed Value of the Player's speed in the x axis
 * @param yspeed Value of the Player's speed in the y axis
 * @param orientation Variable of the orientation of the player
 * @return Pointer to the Player struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
Player *create_player(xpm_map_t pic, int x, int y, int width, int height, int xspeed, int yspeed, bool orientation);

/** 
 * @brief Destructor of the Player "class"
 * @details Frees the memory allocated for the Player instance and its buffer
 *
 * @param pl Pointer to the Player struct to be deallocated. If pl is NULL, no operation is performed
 */
void destroy_player(Player *pl);

/** 
 * @brief Function that moves the player to the left
 * @details This function realocates the Player to the left of the field. It is used whenever the game is reset, either by beginning a new game or when some scores a goal
 *
 * @param pl Pointer to the Player struct to be reallocated. If pl is NULL, no operation is performed
 */
void move_player1_to_center(Player *pl);

/** 
 * @brief Function that moves the player to the right
 * @details This function realocates the Player to the right of the field. It is used whenever the game is reset, either by beginning a new game or when some scores a goal
 *
 * @param pl Pointer to the Player struct to be reallocated. If pl is NULL, no operation is performed
 */
void move_player2_to_center(Player *pl);

#endif
