#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "player.h"
#include "log.h"
#include "game_configuration.h"

bool isGameRunning = true;


int main() {

  setvbuf(stdout, NULL, _IONBF, 0); /* Disabling buffering to make printf work */
  
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    printf("SDL init error: %s", SDL_GetError());

  if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)))
    printf("IMG init error: %s", SDL_GetError());
  
  SDL_Window* window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Event event;
  SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT); /* TODO: Make reading resolution from file */

  SDL_Rect rect = {
    .x = 640 / 4 / 2,
    .y = 480 / 4 / 2,
    .w = 1,
    .h = 1
  };

  /* Game init start */

  controls_t controls = createControls('w', 's', 'a', 'd', 'e', 'q', 'r'); /* TODO: LMB & RMB bind */
  character_sprites_t sprites = createSprites(renderer, "images/player_sprites/char1");
  player_t player = createPlayer(0, controls, sprites); /* TODO: Make PlayerID system */
  
  /* Game init end */

  
  while (isGameRunning) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
	SDL_Quit();
	break;
      case SDL_KEYDOWN:
	switch ((char)(event.key.keysym.sym)) {
	case 'w':
	  log_info("Info test");
	  break;
	case 'a':
	  log_error("Error test");
	  break;
	case 's':
	  log_warn("Warn test");
	  break;
	case 'd':
	  log_game("Game test");
	  break;
	}
      }
    }

    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_RenderPresent(renderer);
  }
}
