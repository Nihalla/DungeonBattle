#include "Paladin.hpp"

void Paladin::init(int x_pos, int y_pos)
{
  unit_class     = PALADIN;
  hp             = 15;
  damage         = 7;
  movement_range = 1;
  attack_range   = 1;
  initiative     = 6;
  position[0]    = x_pos;
  position[1]    = y_pos;
}
