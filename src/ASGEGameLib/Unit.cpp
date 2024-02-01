
//#pragma once
#include "Unit.hpp"
#include "MyPacket.hpp"
//#include <ASGEGameLib/MyPacket.hpp>
//#include <ASGEGameLib/Unit.hpp>

#include <utility>

void Unit::setPosition(int new_x, int new_y)
{
  position[0] = new_x;
  position[1] = new_y;
}
void Unit::setPlayerID(int ide)
{
  player_id = ide;
}
void Unit::setUnitID(int ide)
{
  unit_id = ide;
}
int Unit::getPlayerID() const
{
  return player_id;
}
int Unit::getUnitID() const
{
  return unit_id;
}
int Unit::getPlayerUnitID() const
{
  return player_id * 100 + unit_id;
}
Unit::Class Unit::getUnitClass()
{
  return unit_class;
}
Unit::Status Unit::getStatus()
{
  return status;
}
std::array<int, 2> Unit::getPosition()
{
  return position;
}
int Unit::getHealth() const
{
  return hp;
}
int Unit::getDamage() const
{
  return damage;
}
int Unit::getAttackRange() const
{
  return attack_range;
}
int Unit::getMovementRange() const
{
  return movement_range;
}
int Unit::getInitiative() const
{
  return initiative;
}
void Unit::takeDamage(int dmg)
{
  hp -= dmg;
  if (hp <= 0)
  {
    status = DEAD;
    hp     = 0;
  }
}
void Unit::init(int x_pos, int y_pos)
{
  player_id      = 0;
  unit_id        = 00;
  unit_class     = BASIC;
  hp             = 10;
  damage         = 1;
  movement_range = 1;
  attack_range   = 1;
  initiative     = 5;
  position[0]    = x_pos;
  position[1]    = y_pos;
}
bool Unit::getHasMoved() const
{
  return has_moved;
}
void Unit::setHasMoved(bool moved)
{
  has_moved = moved;
}
void Unit::copy(Unit& original_unit)
{
  hp       = original_unit.hp;
  status   = original_unit.status;
  position = original_unit.position;
}
void Unit::setHasAttacked(bool attacked)
{
  has_attacked = attacked;
}
bool Unit::getHasAttacked()
{
  return has_attacked;
}
void Unit::setHP(int new_hp)
{
  hp = new_hp;
  if (hp <= 0)
  {
    status = DEAD;
    hp     = 0;
  }
}

/// More errors to do with the packet/unit interaction
