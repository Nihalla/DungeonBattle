
#ifndef ASGENETGAME_PALADIN_HPP
#define ASGENETGAME_PALADIN_HPP

#include "Unit.hpp"

class Paladin : public Unit
{
 public:
  Paladin()           = default;
  ~Paladin() override = default;
  void init(int x_pos, int y_pos) override;
};

#endif // ASGENETGAME_PALADIN_HPP
