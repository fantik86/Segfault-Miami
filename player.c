#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <SDL2/SDL_image.h>
#include "player.h"
#include "log.h"

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
  if (dir[strlen(dir)] == '/' && str2[0] == '/') {
    result = (char*)malloc(strlen(dir) - 1);
    memcpy(result, dir, strlen(dir) - 1);
    result = nrwcat(result, str2);
    return result;
  } else if (dir[strlen(dir)] != '/' && str2[0] != '/') {
    result = (char*)malloc(strlen(dir) + 1);
    memcpy(result, dir, strlen(dir));
    result[strlen(dir)] = '/';
    result = nrwcat(result, str2);
    printf("%s\n", result);
    return result;
  } else {
    return nrwcat(dir, str2);
  }
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
  
  int cldir = closedir(dr);
  if (cldir == -1) {
    printf("Failed to close directory\n");
  }
  
  return counter;
}

SDL_Surface* load_image(char* directory) {
  SDL_Surface* image = IMG_Load(directory);
  if (image == NULL) {
    printf("load_image: Skipped, image not found: %s\n", directory);
    return NULL;
  }
  
  return image;
}

SDL_Texture** texture_sequence_copy(SDL_Renderer* renderer, char* directory) {
  SDL_Texture** texture_sequence;
  uint8_t sequence_count = dir_count_files(directory);
  
  texture_sequence = (SDL_Texture**)malloc(sequence_count * sizeof(SDL_Texture*));

  dr = opendir(directory);
  
  for (int i = 0; (de = readdir(dr)) != NULL; i++)
    if (strncmp(de->d_name + (strlen(de->d_name) - 4), ".png", 4) == 0 ||
	strncmp(de->d_name + (strlen(de->d_name) - 4), ".jpg", 4) == 0)
      texture_sequence[i] = SDL_CreateTextureFromSurface(renderer, load_image(appendtodir(directory, de->d_name)));

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


character_sprites_t createSprites(SDL_Renderer* renderer, char* directory) {

  SDL_Texture** idle = texture_sequence_copy(renderer, appendtodir(directory, "sequences/idle"));
  SDL_Texture** walking = texture_sequence_copy(renderer, appendtodir(directory, "sequences/walking"));
  SDL_Texture** execution_lay = texture_sequence_copy(renderer, appendtodir(directory, "sequences/execution_lay"));
  SDL_Texture** drop_item = texture_sequence_copy(renderer, appendtodir(directory, "sequences/drop_item"));
  SDL_Texture** death_vars = texture_sequence_copy(renderer, appendtodir(directory, "sequences/death_vars"));
  
  SDL_Surface* lay_front = load_image(appendtodir(directory, "/lay_front.png"));
  SDL_Surface* lay_back = load_image(appendtodir(directory, "/lay_back.png"));
  SDL_Surface* lay_wall = load_image(appendtodir(directory, "/lay_wall.png"));
    
  character_sprites_t sprites = {
    .idle = idle,
    .walking = walking,
    .execution_lay = execution_lay,
    .drop_item = drop_item,
    .death_vars = death_vars,
    .lay_front = SDL_CreateTextureFromSurface(renderer, lay_front),
    .lay_back= SDL_CreateTextureFromSurface(renderer, lay_back),
    .lay_wall = SDL_CreateTextureFromSurface(renderer, lay_wall)
  };

  return sprites;
}

player_t createPlayer(uint8_t id, controls_t controls, character_sprites_t sprites) {
  /* TODO: Support of online room logic, adding id and player to player list */
  player_t player = {
    .id = id,
    .controls = controls,
    .sprites = sprites,
    .combo_counter = 0,
    .knockedDown_timeout = 0,
    .isAttacking = false,
    .isAlive = true
  };

  return player;
}
