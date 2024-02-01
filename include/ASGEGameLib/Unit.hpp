#pragma once
#ifndef ASGENETGAME_UNIT_HPP
#  define ASGENETGAME_UNIT_HPP
#  include "MyPacket.hpp"
#  include <array>
#  include <vector>

class Unit
{
 public:
  Unit()          = default;
  virtual ~Unit() = default;

  enum Class
  {
    BASIC   = 0,
    ARCHER  = 1,
    KNIGHT  = 2,
    PALADIN = 3,
    INVALID = -1
  };
  enum Status
  {
    DEAD    = 0,
    ALIVE   = 1,
    UNKNOWN = -1
  };

  /*void setPosition(std::vector<int> new_position);*/

  void setPosition(int new_x, int new_y);
  void setPlayerID(int ide);
  void setUnitID(int ide);
  void setHasMoved(bool moved);
  void setHasAttacked(bool attacked);
  void setHP(int new_health);
  void copy(Unit& original_unit);
  virtual void init(int x_pos, int y_pos);

  [[nodiscard]] int getPlayerID() const;
  [[nodiscard]] int getUnitID() const;
  [[nodiscard]] int getPlayerUnitID() const;
  Class getUnitClass();
  Status getStatus();
  std::array<int, 2> getPosition();
  [[nodiscard]] int getHealth() const;
  [[nodiscard]] int getDamage() const;
  [[nodiscard]] int getAttackRange() const;
  [[nodiscard]] int getMovementRange() const;
  [[nodiscard]] int getInitiative() const;
  [[nodiscard]] bool getHasMoved() const;
  bool getHasAttacked();

  void takeDamage(int dmg);

  /// Custom Package Unit

  friend MyPacket& operator<<(MyPacket& packet, const Unit& unit)
  {
    return packet << unit.player_id << unit.unit_id << unit.unit_class << unit.status
                  << unit.position[0] << unit.position[1] << unit.hp;
  }
  friend MyPacket& operator>>(MyPacket& packet, Unit& unit)
  {
    return packet >> unit.player_id >> unit.unit_id >> unit.unit_class >> unit.status >>
           unit.position[0] >> unit.position[1] >> unit.hp;
  }

 protected:
  int player_id    = 0;
  int unit_id      = 00;
  Class unit_class = BASIC;
  Status status    = ALIVE;
  std::array<int, 2> position{ 0, 0 };
  int hp             = 10;
  int damage         = 1;
  int attack_range   = 1;
  int movement_range = 1;
  int initiative     = 5;
  bool has_moved     = false;
  bool has_attacked  = false;
};

#endif // ASGENETGAME_UNIT_HPP
