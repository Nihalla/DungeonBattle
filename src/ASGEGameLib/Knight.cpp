

#include "Knight.hpp"
void Knight::init(int x_pos, int y_pos)
{
  unit_class     = KNIGHT;
  hp             = 12;
  damage         = 5;
  movement_range = 2;
  attack_range   = 2;
  initiative     = 7;
  position[0]    = x_pos;
  position[1]    = y_pos;
}
