#ifndef ITEMS_H
#define ITEMS_H

#include <stdint.h>
#include <stdbool.h>
#include "vectors.h"

typedef struct {
  int8_t ammo_count;        /* ammo_count = -1 means that the weapon is melee */
  bool isBroken;
} weapon_data_t;

typedef struct {
  int8_t index;
  vector2_t position;
  weapon_data_t data;
} weapon_t;

weapon_t createItem(int8_t index, vector2_t position, weapon_data_t data);

#endif
