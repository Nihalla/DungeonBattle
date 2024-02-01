
#pragma once

#include "Tile.hpp"
#include "kissnet.hpp"
#include "soloud.h"
#include "soloud_wav.h"
#include <ASGEGameLib/Archer.hpp>
#include <ASGEGameLib/GCNetClient.hpp>
#include <ASGEGameLib/GComponent.hpp>
#include <ASGEGameLib/Knight.hpp>
#include <ASGEGameLib/MyPacket.hpp>
#include <ASGEGameLib/Paladin.hpp>
#include <ASGEGameLib/Unit.hpp>
#include <Engine/Logger.hpp>
#include <Engine/OGLGame.hpp>
#include <Engine/Sprite.hpp>
#include <map>
#include <synchapi.h>
#include <vector>

constexpr const int TILE_SCALE = 80;

class ASGENetGame : public ASGE::OGLGame
{
 public:
  explicit ASGENetGame(const ASGE::GameSettings& settings);
  ~ASGENetGame() override;

  ASGENetGame(const ASGENetGame&) = delete;
  ASGENetGame& operator=(const ASGENetGame&) = delete;

  void initTexts(int idx);
  void initUnits(Unit::Class unit_class, int x_position, int y_position);
  void initMap();
  void initAudio();
  void setUpTextures();
  void assignUnits();

  void keyHandler(ASGE::SharedEventData data);
  void clickHandler(ASGE::SharedEventData data);
  void manageMovementClick(int tile_x, int tile_y);

  void update(const ASGE::GameTime& us) override;
  void render(const ASGE::GameTime& us) override;
  void fixedUpdate(const ASGE::GameTime& us) override;

  void customInput(kissnet::tcp_socket& socket, bool data);
  bool customInput(kissnet::tcp_socket& socket, Unit& unit);
  void playerReadyInput(kissnet::tcp_socket& socket) const;

  void moreRun();
  void syncSprites(Unit* unit);
  void startTurn();
  void updateText();

  GCNetClient* getClientPtr();

 private:
  std::vector<std::unique_ptr<GameComponent>> game_components;
  std::vector<std::unique_ptr<Unit>> deployed_units;
  std::vector<std::unique_ptr<Unit>> enemy_units;
  std::vector<std::unique_ptr<ASGE::Sprite>> sprites;
  std::vector<Tile> tilemap;

  std::unique_ptr<ASGE::Sprite> background;
  std::unique_ptr<ASGE::Sprite> user_interface;
  std::unique_ptr<ASGE::Sprite> sprite;

  Unit* active_unit = nullptr;

  std::map<int, int> unit_to_sprite;
  std::map<Unit::Class, std::string> textures;

  std::string key_callback_id{}; /**< Key Input Callback ID. */
  std::string click_callback_id{};

  std::mutex input_mutex;
  std::mutex more_run_mutex;

  SoLoud::Soloud audio_engine;
  SoLoud::Wav background_audio;

  bool current_player_turn = false;
  bool updated             = false;
  bool running             = false;
  bool connected           = false;
  int player_id            = 0;
  bool moving_piece        = false;
  bool input_valid         = true;
  bool is_ready            = false;
  bool muted               = false;
  int player_units_alive   = 0;
  int enemy_units_alive    = 3;

  [[maybe_unused]] int font_idx                = 0;
  [[maybe_unused]] ASGE::Text menu_text        = {};
  [[maybe_unused]] ASGE::Text ready_text       = {};
  [[maybe_unused]] ASGE::Text turn_text        = {};
  [[maybe_unused]] ASGE::Text end_turn_text    = {};
  [[maybe_unused]] ASGE::Text hp_variant_text  = {};
  [[maybe_unused]] ASGE::Text hp_const_text    = {};
  [[maybe_unused]] ASGE::Text mov_variant_text = {};
  [[maybe_unused]] ASGE::Text mov_const_text   = {};
  [[maybe_unused]] ASGE::Text atk_variant_text = {};
  [[maybe_unused]] ASGE::Text atk_const_text   = {};
  [[maybe_unused]] ASGE::Text ending_text      = {};
};
