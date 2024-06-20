#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <SDL2/SDL_image.h>
#include "player.h"
#include "log.h"
#include "vectors.h"
#include "game_configuration.h"

struct dirent *de;
DIR *dr;

char* nrwcat(char* str1, const char* str2) { /* No-rewrite concatenation function that doesn't rewrite any of it's operands. */
  char* result = (char*)malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(result, str1);
  strcat(result, str2);

  return result;
}

char* appendtodir(char* dir, const char* str2) {
    char* result;
    size_t dir_len = strlen(dir);
    size_t str2_len = strlen(str2);

    // Определяем, есть ли необходимость добавить разделитель '/'
    if (dir[dir_len - 1] == '/' && str2[0] == '/') {
        // Случай, когда последний символ dir - '/' и первый символ str2 - '/'
        result = (char*)malloc(dir_len + str2_len);
        memcpy(result, dir, dir_len - 1);
        memcpy(result + dir_len - 1, str2, str2_len + 1); // +1 для '\0'
    } else if (dir[dir_len - 1] != '/' && str2[0] != '/') {
        // Случай, когда последний символ dir - не '/' и первый символ str2 - не '/'
        result = (char*)malloc(dir_len + str2_len + 2); // +2 для '/' и '\0'
        memcpy(result, dir, dir_len);
        result[dir_len] = '/';
        memcpy(result + dir_len + 1, str2, str2_len + 1); // +1 для '\0'
    } else {
        // Все остальные случаи
        result = (char*)malloc(dir_len + str2_len + 1); // +1 для '\0'
        memcpy(result, dir, dir_len);
        memcpy(result + dir_len, str2, str2_len + 1); // +1 для '\0'
    }

    return result;
}

uint8_t dir_count_files(const char* directory) {
  uint8_t counter = 0;
  dr = opendir(directory);

  if (dr == NULL) {
    switch (errno) {
    case ENOENT:
      printf("dir_count_files: opendir returned NULL, directory isn't exist: %s\n", directory);
      break;
    case EACCES:
      printf("dir_count_files: opendir returned NULL, permission is denied for directory: %s\n", directory);
      break;
    default:
      printf("dir_count_files: opendir returned NULL, error unknown, directory: %s\n", directory);
    }
    if (closedir(dr) == -1) {
      perror("dir_count_files: closedir error\n");
    }
    return 0;
  }
  
  while ((de = readdir(dr)) != NULL)
    counter++;
  
  if (closedir(dr) == -1) {
    printf("Failed to close directory\n");
  }
  
  return counter;
}

SDL_Surface* load_image(char* directory) {
  SDL_Surface* image = IMG_Load(directory);
  if (image == NULL) {
    printf("load_image: Skipped, SDL_image error or image not found: %s\n", directory);
    return NULL;
  }
  
  return image;
}

SDL_Texture** texture_sequence_copy(SDL_Renderer* renderer, char* directory) {
  SDL_Texture** texture_sequence;
  uint8_t sequence_count = dir_count_files(directory) - 2; /* minus two because '.' and '..' (most likely lol) */
  texture_sequence = (SDL_Texture**)malloc(sequence_count * sizeof(SDL_Texture*));

  dr = opendir(directory);
  
  for (int i = 0; (de = readdir(dr)) != NULL; i++)
    if (strcmp(de->d_name + (strlen(de->d_name) - 4), ".png") == 0 ||
	strcmp(de->d_name + (strlen(de->d_name) - 4), ".jpg") == 0) {
      printf("TEXTURE: INDEX IS %d; PATH IS %s\n", i, appendtodir(directory, de->d_name));
      texture_sequence[i] = SDL_CreateTextureFromSurface(renderer, load_image(appendtodir(directory, de->d_name)));
    }
  closedir(dr);
  
  return texture_sequence;
}


controls_t createControls(uint8_t walk_forward, uint8_t walk_backward,
                          uint8_t walk_right, uint8_t walk_left, uint8_t attack,
                          uint8_t drag_drop, uint8_t special) {
  /* TODO: Make controls reading from file */
  controls_t controls = {
    .walk_forward = walk_forward,
    .walk_backward = walk_backward,
    .walk_right = walk_right,
    .walk_left = walk_left,
    .attack = attack,
    .drag_drop = drag_drop,
    .special = special
  };

  return controls;
}

/*
 * SDL_Renderer* renderer - renderer of SDL library that is need to generate
 * textures from surfaces
 * char* directory - target character directory in player_sprites */
character_sprites_t createSprites(SDL_Renderer* renderer, char* directory) {

  SDL_Texture** idle = texture_sequence_copy(renderer, appendtodir(directory, "/sequences/idle"));
  SDL_Texture** walking = texture_sequence_copy(renderer, appendtodir(directory, "/sequences/walking"));
  SDL_Texture** execution_lay = texture_sequence_copy(renderer, appendtodir(directory, "/sequences/execution_lay"));
  SDL_Texture** drop_item = texture_sequence_copy(renderer, appendtodir(directory, "/sequences/drop_item"));
  SDL_Texture** death_vars = texture_sequence_copy(renderer, appendtodir(directory, "/sequences/death_vars"));
  
  SDL_Texture* lay_front = SDL_CreateTextureFromSurface(renderer, load_image(appendtodir(directory, "/lay_front.png")));
  SDL_Texture* lay_back = SDL_CreateTextureFromSurface(renderer, load_image(appendtodir(directory, "/lay_back.png")));
  SDL_Texture* lay_wall = SDL_CreateTextureFromSurface(renderer, load_image(appendtodir(directory, "/lay_wall.png")));
  
  character_sprites_t sprites = {
    .idle = idle,
    .walking = walking,
    .execution_lay = execution_lay,
    .drop_item = drop_item,
    .death_vars = death_vars,
    .lay_front = lay_front,
    .lay_back= lay_back,
    .lay_wall = lay_wall
  };

  return sprites;
}

player_t createPlayer(uint8_t id, vector2_t position, int16_t rotation, character_state_t state, controls_t controls, character_sprites_t sprites) {
  /* TODO: Support of online room logic, adding id and player to player list */

  controls_t downed_controls = createControls(0, 0, 0, 0, 0, 0, 0);
  
  player_t player = {
    .id = id,
    .position = position,
    .rotation = rotation,
    .state = state,
    .controls = controls,
    .sprites = sprites,
    .current_frame = 0,
    .next_anim_frame_time = 0,
    .combo_counter = 0,
    .knockedDown_timeout = 0,
    .downed_controls = downed_controls,
    .isAttacking = false,
    .isAlive = true
  };

  return player;
}

void updatePlayer(player_t* player, vector2_t position, int16_t rotation,
		  character_state_t state, uint8_t combo_counter,
		  uint8_t knockedDown_timeout, bool isAttacking, bool isAlive) {
  *player = (player_t) {
    .position = position,
    .rotation = rotation,
    .state = state,
    .combo_counter = combo_counter,
    .knockedDown_timeout = knockedDown_timeout,
    .isAttacking = isAttacking,
    .isAlive = isAlive
  };
}

void updatePlayerState(player_t* player, character_state_t state) {
  player->state = state;
}

void redrawPlayer(SDL_Renderer* renderer, player_t* player) {
  SDL_Rect rect = {
    .x = player->position.x,
    .y = player->position.y,
    .w = 32 * 4,
    .h = 32 * 4
  };
  switch (player->state) {
  case CHAR_STATE_IDLE:
    player->current_frame = 0;
    SDL_RenderCopy(renderer, player->sprites.idle[0], NULL, &rect);
    break;
  case CHAR_STATE_WALKING:
    SDL_RenderCopy(renderer, player->sprites.walking[player->current_frame], NULL, &rect);
    printf("CURR FRAME IS %d; TICKS: %d\n", player->current_frame, SDL_GetTicks());
    if (SDL_GetTicks() >= player->next_anim_frame_time) {
      player->next_anim_frame_time = SDL_GetTicks() + 500;
      player->current_frame++;
    }
    if (player->current_frame >= 6) { /* TODO: change 6 to something universal idk */
      player->current_frame = 0;
      log_info("reached >=7 frames, resetting");
    }
    break;
  default:
    log_error("redrawPlayer: state isn't found!");
    break;
  }
}

void changeDownedControls(controls_t* controls, controls_button_t button, uint8_t value) {
  switch (button) {
  case CONTROLS_WALK_FORWARD:
    controls->walk_forward = value;
    break;
  case CONTROLS_WALK_BACKWARD:
    controls->walk_backward = value;
    break;
  case CONTROLS_WALK_RIGHT:
    controls->walk_right = value;
    break;
  case CONTROLS_WALK_LEFT:
    controls->walk_left = value;
    break;
  case CONTROLS_ATTACK:
    controls->attack = value;
    break;
  case CONTROLS_DRAG_DROP:
    controls->drag_drop = value;
    break;
  case CONTROLS_SPECIAL:
    controls->special = value;
    break;
  }
}
