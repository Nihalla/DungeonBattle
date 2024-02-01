
#ifndef ASGENETGAME_ARCHER_HPP
#define ASGENETGAME_ARCHER_HPP
#include "Unit.hpp"

class Archer : public Unit
{
 public:
  Archer()           = default;
  ~Archer() override = default;
  void init(int x_pos, int y_pos) override;
};

#endif // ASGENETGAME_ARCHER_HPP
