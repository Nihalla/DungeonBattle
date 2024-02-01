
#include "Archer.hpp"

void Archer::init(int x_pos, int y_pos)
{
  unit_class     = ARCHER;
  hp             = 7;
  damage         = 3;
  movement_range = 3;
  attack_range   = 3;
  initiative     = 8;
  position[0]    = x_pos;
  position[1]    = y_pos;
}
