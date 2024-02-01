
#include "Tile.hpp"

int Tile::getTileX() const
{
  return x;
}
int Tile::getTileY() const
{
  return y;
}
bool Tile::getOccupied() const
{
  return occupied;
}
bool Tile::getValidMove() const
{
  return valid_move;
}
void Tile::setTileX(int new_x)
{
  x = new_x;
}
void Tile::setTileY(int new_y)
{
  y = new_y;
}
void Tile::setTilePos(int new_x, int new_y)
{
  x = new_x;
  y = new_y;
}
void Tile::setOccupied(bool occupied_status)
{
  occupied = occupied_status;
}
void Tile::setValidMove(bool status)
{
  valid_move = status;
}
bool Tile::isPointInTile(float point_x, float point_y) const
{
  auto ver1 = static_cast<float>(x);
  auto ver2 = static_cast<float>(x + WIDTH);
  auto ver3 = static_cast<float>(y);
  auto ver4 = static_cast<float>(y + HEIGHT);

  return (ver1 <= point_x && point_x <= ver2 && ver3 <= point_y && point_y <= ver4);
}
