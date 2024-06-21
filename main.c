#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "player.h"
#include "log.h"
#include "game_configuration.h"

#define PLAYER_STEP_LENGTH 1

bool isGameRunning = true;

int main() {

  setvbuf(stdout, NULL, _IONBF, 0); /* Disabling buffering to make printf work properly */
  log_error("test%d\n", 2);
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    printf("SDL init error: %s", SDL_GetError());

  if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)))
    printf("IMG init error: %s", SDL_GetError());
  
  SDL_Window* window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Event event;
  SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT); /* TODO: Make reading resolution from file */

  /* Game init start */

  controls_t controls = createControls('w', 's', 'a', 'd', 'e', 'q', 'r'); /* TODO: LMB & RMB bind */
  character_sprites_t sprites = createSprites(renderer, "images/player_sprites/char1");
  player_t player = createPlayer(0, vector2(0, 0), -90, CHAR_STATE_IDLE, controls, sprites); /* TODO: Make PlayerID system */

  uint8_t controls_forward = player.controls.walk_forward;
  uint8_t controls_backward = player.controls.walk_backward;
  uint8_t controls_left = player.controls.walk_left;
  uint8_t controls_right = player.controls.walk_right;

  char current_key = (char)(event.key.keysym.sym);
    
  /* Game init end */

  while (isGameRunning) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
	IMG_Quit();
	SDL_Quit();
	return 0;
	break;
      }
	if (event.type == SDL_KEYUP) {
	  current_key = (char)(event.key.keysym.sym);
	  if (current_key == controls_forward ||
	      current_key == controls_backward ||
	      current_key == controls_left ||
	      current_key == controls_right) {
	    updatePlayerState(&player, CHAR_STATE_IDLE);
	  }
	  if (current_key == controls_forward) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_FORWARD, 0);
	  } else if (current_key == controls_backward) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_BACKWARD, 0);
	  }
	  if (current_key == controls_left) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_RIGHT, 0);
	  } else if (current_key == controls_right) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_LEFT, 0);
	  }
	} else if (event.type == SDL_KEYDOWN) {
	  current_key = (char)(event.key.keysym.sym);
	  if ((current_key == controls_forward ||
	     current_key == controls_backward ||
	     current_key == controls_left ||
	       current_key == controls_right) && player.state == CHAR_STATE_IDLE) {
	    updatePlayerState(&player, CHAR_STATE_WALKING);
	  }
          if (current_key == controls_forward) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_FORWARD, PLAYER_STEP_LENGTH);
	  } else if (current_key == controls_backward) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_BACKWARD, PLAYER_STEP_LENGTH);
	  }
	  if (current_key == controls_left) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_RIGHT, PLAYER_STEP_LENGTH);
	  } else if (current_key == controls_right) {
	    changeDownedControls(&(player.downed_controls), CONTROLS_WALK_LEFT, PLAYER_STEP_LENGTH);
	  }
	}
      }
    
    SDL_RenderClear(renderer);

    redrawPlayer(renderer, &player);
    
    SDL_RenderPresent(renderer);

    player.position.y -= player.downed_controls.walk_forward;
    player.position.y += player.downed_controls.walk_backward;
    player.position.x += player.downed_controls.walk_right;
    player.position.x -= player.downed_controls.walk_left;
  }
}
