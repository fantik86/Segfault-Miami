#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "vectors.h"

typedef enum {
  CHAR_STATE_IDLE,
  CHAR_STATE_WALKING,
  CHAR_STATE_ATTACKING,
  CHAR_STATE_WEAPON_DROPPING,
  CHAR_STATE_LAY
} character_state_t;

typedef struct {
  uint8_t walk_forward;       /* Walk forward                     */
  uint8_t walk_backward;
  uint8_t walk_left;
  uint8_t walk_right;
  uint8_t attack;             /* Attack (punch, cut, shoot, etc.) */
  uint8_t drag_drop;          /* Drag item or drop it             */
  uint8_t special;            /* Special skill of the mask        */
} controls_t;

typedef struct {
  SDL_Texture** idle;           /* Idle animation                      */
  SDL_Texture** walking;        /* Walking animation                   */
  SDL_Texture** execution_lay;  /* Character executioning someone anims with different weapons */
  SDL_Texture** drop_item;      /* Drop or throw weapon animation      */
  SDL_Texture** death_vars;     /* This is not used for animation, it's images with different kinds of death */
  SDL_Texture* lay_front;       /* Character lies on his back image    */
  SDL_Texture* lay_back;        /* Character lies on his stomach image */
  SDL_Texture* lay_wall;        /* Character sitting against the wall  */
} character_sprites_t;

typedef struct {
  uint8_t id;
  vector2_t position;
  int16_t rotation;
  character_state_t state;
  controls_t controls;
  character_sprites_t sprites;
  uint8_t combo_counter;
  uint8_t knockedDown_timeout;
  bool isAttacking;
  bool isAlive;
} player_t;

/* TODO: character_t */

controls_t createControls(uint8_t walk_forward, uint8_t walk_backward,
			  uint8_t walk_right, uint8_t walk_left, uint8_t attack,
			  uint8_t drag_drop, uint8_t special);

character_sprites_t createSprites(SDL_Renderer* renderer, char* directory);    /* Takes directory to the folder with sprites and processing it */

player_t createPlayer(uint8_t id, vector2_t position, int16_t rotation, character_state_t state, controls_t controls, character_sprites_t sprites);

void updatePlayer(player_t* player, vector2_t position, int16_t rotation, character_state_t state, uint8_t combo_counter, uint8_t knockedDown_timeout, bool isAttacking, bool isAlive);

void redrawPlayer(SDL_Renderer* renderer, player_t player);

#endif
