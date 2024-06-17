#include "items.h"

weapon_t createItem(int8_t index, vector2_t position, weapon_data_t data) {
  weapon_t weapon = {
    .index = index,
    .position = position,
    .data = data
  };
  return weapon;
}
