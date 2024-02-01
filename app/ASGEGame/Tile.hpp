
#ifndef ASGENETGAME_TILE_HPP
#define ASGENETGAME_TILE_HPP

class Tile
{
 public:
  [[nodiscard]] int getTileX() const;
  [[nodiscard]] int getTileY() const;
  [[nodiscard]] bool getOccupied() const;
  [[nodiscard]] bool getValidMove() const;

  void setTileX(int new_x);
  void setTileY(int new_y);
  void setTilePos(int new_x, int new_y);
  void setOccupied(bool occupied_status);
  void setValidMove(bool status);
  [[nodiscard]] bool isPointInTile(float point_x, float point_y) const;

 private:
  int x            = 0;
  int y            = 0;
  const int HEIGHT = 80;
  const int WIDTH  = 80;
  bool occupied    = false;
  bool valid_move  = false;
};

#endif // ASGENETGAME_TILE_HPP
