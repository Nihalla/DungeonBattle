
#ifndef ASGENETGAME_KNIGHT_HPP
#define ASGENETGAME_KNIGHT_HPP

#include "Unit.hpp"
class Knight : public Unit
{
 public:
  Knight()           = default;
  ~Knight() override = default;
  void init(int x_pos, int y_pos) override;
};

#endif // ASGENETGAME_KNIGHT_HPP
