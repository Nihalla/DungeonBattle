#include "ASGEGame.hpp"
#include <ASGEGameLib/GCNetClient.hpp>
#include <Engine/FileIO.hpp>

/// Initialises the game.
/// Setup your game and initialise the core components.
/// @param settings
ASGENetGame::ASGENetGame(const ASGE::GameSettings& settings) : OGLGame(settings), running(true)
{
  key_callback_id   = inputs->addCallbackFnc(ASGE::E_KEY, &ASGENetGame::keyHandler, this);
  click_callback_id = inputs->addCallbackFnc(ASGE::E_MOUSE_CLICK, &ASGENetGame::clickHandler, this);
  inputs->use_threads = false;
  // toggleFPS();
  auto client = std::make_unique<GCNetClient>();

  game_components.emplace_back(std::move(client));
  deployed_units.reserve(10);
  enemy_units.reserve(10);

  setUpTextures();
  initMap();
  initAudio();
  sprites.reserve(30);

  std::thread running_thread([&] { moreRun(); });
  running_thread.detach();

  background = renderer->createUniqueSprite();
  background->loadTexture("/data/img/Light_map.png");
  background->loadTexture("/data/img/Dark_map.png");

  user_interface = renderer->createUniqueSprite();
  user_interface->loadTexture("/data/img/UI.png");
  user_interface->loadTexture("/data/img/Empty_UI.png");

  sprite = renderer->createUniqueSprite();
  sprite->loadTexture("/data/img/Paladin_img.png");
  sprite->loadTexture("/data/img/Archer_img.png");
  sprite->loadTexture("/data/img/Knight_img.png");
  sprite->setGlobalZOrder(64);
  sprite->scale(1);
  sprite->xPos(7 * TILE_SCALE);
  sprite->yPos(8 * TILE_SCALE);

  if (auto idx = renderer->loadFont("/data/fonts/KGHAPPY.ttf", 48); idx > 0)
  {
    initTexts(idx);
  }
}

/// Destroys the game.
ASGENetGame::~ASGENetGame()
{
  running = false;
  this->inputs->unregisterCallback(key_callback_id);
}

/// Quick access function to receive the client pointer from game components
GCNetClient* ASGENetGame::getClientPtr()
{
  for (auto& component : game_components)
  {
    if (component->typeID() == GameComponent::ID::NETWORK_CLIENT)
    {
      return dynamic_cast<GCNetClient*>(component.get());
    }
  }
  return nullptr;
}

void ASGENetGame::initUnits(Unit::Class unit_class, int x_position, int y_position)
{
  Unit* unit = nullptr;
  switch (unit_class)
  {
    case Unit::Class::BASIC:
      unit = new Unit;
      break;

    case Unit::Class::ARCHER:
      unit = new Archer;
      break;

    case Unit::Class::KNIGHT:
      unit = new Knight;
      break;

    case Unit::Class::PALADIN:
      unit = new Paladin;
      break;

    case Unit::Class::INVALID:
      break;
  }
  if (unit != nullptr)
  {
    unit->init(x_position, y_position);
    unit->setPlayerID(player_id);
    unit->setUnitID(static_cast<int>(deployed_units.size()) + 1);
    deployed_units.emplace_back(unit);
  }
}

void ASGENetGame::initTexts(int idx)
{
  font_idx = idx;
  menu_text.setFont(renderer->getFont(font_idx))
    .setString("          LOBBY \n"
               "PRESS 'C' to connect")
    .setPosition(
      { 8 * TILE_SCALE - menu_text.getWidth() / 2, 4.0F * TILE_SCALE - menu_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  ready_text.setFont(renderer->getFont(font_idx))
    .setString("PRESS 'R' to Ready up!")
    .setPosition({ 8 * TILE_SCALE - ready_text.getWidth() / 2,
                   4.5F * TILE_SCALE - ready_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  turn_text.setFont(renderer->getFont(font_idx))
    .setString("Waiting for other player to finish!")
    .setPosition(
      { 8 * TILE_SCALE - turn_text.getWidth() / 2, 0.5F * TILE_SCALE + turn_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  end_turn_text.setFont(renderer->getFont(font_idx))
    .setString("End Turn")
    .setScale(0.5F)
    .setPosition({ 14 * TILE_SCALE, TILE_SCALE - 3.5 * 8 })
    .setColour(ASGE::COLOURS::WHITE);

  hp_const_text.setFont(renderer->getFont(font_idx))
    .setString("HP")
    .setScale(0.5F)
    .setPosition({ 8.5F * TILE_SCALE - hp_const_text.getWidth() / 2,
                   8.4F * TILE_SCALE - hp_const_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  hp_variant_text.setFont(renderer->getFont(font_idx))
    .setString("00")
    .setScale(0.9F)
    .setPosition({ 8.5F * TILE_SCALE - hp_variant_text.getWidth() / 2, 8.9F * TILE_SCALE })
    .setColour(ASGE::COLOURS::WHITE);

  mov_const_text.setFont(renderer->getFont(font_idx))
    .setString("Movement: ")
    .setScale(0.3F)
    .setPosition({ 9.1F * TILE_SCALE, 8.25F * TILE_SCALE + mov_const_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::PALEGREEN);

  mov_variant_text.setFont(renderer->getFont(font_idx))
    .setString("0")
    .setScale(0.3F)
    .setPosition({ 10.5F * TILE_SCALE - mov_variant_text.getWidth() / 2,
                   8.25F * TILE_SCALE + mov_variant_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  atk_const_text.setFont(renderer->getFont(font_idx))
    .setString("Atk Range: ")
    .setScale(0.3F)
    .setPosition({ 9.1F * TILE_SCALE, 8.75F * TILE_SCALE + atk_const_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::PALEGREEN);

  atk_variant_text.setFont(renderer->getFont(font_idx))
    .setString("0")
    .setScale(0.3F)
    .setPosition({ 10.5F * TILE_SCALE - atk_variant_text.getWidth() / 2,
                   8.75F * TILE_SCALE + atk_variant_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);

  ending_text.setFont(renderer->getFont(font_idx))
    .setString("YOU LOSE \n"
               "Press ESCAPE to exit.")
    .setPosition(
      { 8 * TILE_SCALE - menu_text.getWidth() / 2, 4.0F * TILE_SCALE - menu_text.getHeight() / 2 })
    .setColour(ASGE::COLOURS::WHITE);
}

void ASGENetGame::initAudio()
{
  audio_engine.init();
  ASGE::FILEIO::File bg_audio_file;
  if (bg_audio_file.open("/data/audio/background_music.mp3"))
  {
    static auto buffer = bg_audio_file.read();
    background_audio.loadMem(
      buffer.as_unsigned_char(), static_cast<unsigned int>(buffer.length), false, false);
  }
  background_audio.setVolume(0.25F);
  audio_engine.play(background_audio);
}

void ASGENetGame::setUpTextures()
{
  textures[Unit::Class::ARCHER]  = "/data/img/Archer_img.png";
  textures[Unit::Class::PALADIN] = "/data/img/Paladin_img.png";
  textures[Unit::Class::KNIGHT]  = "/data/img/Knight_img.png";
}

/// Processes key inputs.
/// This function is added as a callback to handle the game's
/// keyboard input. For this game, calls to this function
/// are not thread safe, so you may alter the game's state
/// but your code needs to designed to prevent data-races.
/// @param data
/// @see KeyEvent
void ASGENetGame::keyHandler(ASGE::SharedEventData data)
{
  if (input_valid)
  {
    const auto* key = dynamic_cast<const ASGE::KeyEvent*>(data.get());

    if (key->key == ASGE::KEYS::KEY_ESCAPE)
    {
      signalExit();
    }
    if (key->key == ASGE::KEYS::KEY_C)
    {
      if (!connected)
      {
        auto* client = dynamic_cast<GCNetClient*>(game_components[0].get());
        client->connect("127.0.0.1", 31276);
        Logging::DEBUG("It Connect");
        initUnits(Unit::PALADIN, 0, 2 * TILE_SCALE);
        initUnits(Unit::ARCHER, 0, 4 * TILE_SCALE);
        initUnits(Unit::KNIGHT, 0, 6 * TILE_SCALE);
        background->loadTexture("/data/img/Light_map.png");
        user_interface->loadTexture("/data/img/UI.png");
        connected = true;
      }
    }
    if ((key->action == ASGE::KEYS::KEY_RELEASED) && key->key == ASGE::KEYS::KEY_R)
    {
      auto* client_ptr = getClientPtr();
      if (client_ptr != nullptr && !is_ready)
      {
        playerReadyInput(client_ptr->socket);
        ready_text
          .setString("Waiting for other player.\n"
                     "Please try again pressing 'R'")
          .setPosition({ 240, 320 });
      }
    }
    if ((key->action == ASGE::KEYS::KEY_RELEASED) && key->key == ASGE::KEYS::KEY_M)
    {
      if (!muted)
      {
        audio_engine.stopAll();
        muted = true;
      }
      else
      {
        audio_engine.play(background_audio);
        muted = false;
      }
    }
    input_valid = false;
  }
}

void ASGENetGame::clickHandler(ASGE::SharedEventData data)
{
  input_mutex.lock();
  if (input_valid)
  {
    const auto* click = dynamic_cast<const ASGE::ClickEvent*>(data.get());
    if (click->action == ASGE::MOUSE::BUTTON_RELEASED && click->button == ASGE::MOUSE::MOUSE_BTN1)
    {
      double x_pos = click->xpos;
      double y_pos = click->ypos;

      int tile_x = static_cast<int>(x_pos / TILE_SCALE);
      int tile_y = static_cast<int>(y_pos / TILE_SCALE);

      if (current_player_turn && is_ready && enemy_units_alive > 0)
      {
        if (y_pos > TILE_SCALE && y_pos < TILE_SCALE * 8)
        {
          manageMovementClick(tile_x, tile_y);
        }
        else if (x_pos > TILE_SCALE * 14 && y_pos < TILE_SCALE && !moving_piece)
        {
          auto* client_ptr    = getClientPtr();
          current_player_turn = false;
          customInput(client_ptr->socket, true);
        }
      }
      input_valid = false;
    }
  }
  input_mutex.unlock();
}

/// Updates your game and all it's components.
/// @param us
void ASGENetGame::update(const ASGE::GameTime& us)
{
  for (auto& component : game_components)
  {
    component->update(us.deltaInSecs());
  }
  player_units_alive = 0;
  for (auto& unit : deployed_units)
  {
    if (unit != nullptr && unit->getPlayerUnitID() != 000)
    {
      if (unit->getStatus() == Unit::Status::ALIVE)
      {
        player_units_alive++;
      }
    }
  }

  std::thread thread_test(
    [&]
    {
      if (player_id != 0)
      {
        for (auto& enemy : enemy_units)
        {
          syncSprites(enemy.get());
        }
        for (auto& unit : deployed_units)
        {
          syncSprites(unit.get());
        }
      }
    });
  thread_test.detach();
}

/// Render your game and its scenes here.
void ASGENetGame::render(const ASGE::GameTime& /*us*/)
{
  renderer->render(*background);
  renderer->render(*user_interface);
  if (!connected)
  {
    renderer->render(menu_text);
  }
  else
  {
    if (!is_ready)
    {
      renderer->render(ready_text);
    }
  }
  if (moving_piece)
  {
    renderer->render(*sprite);
    renderer->render(hp_const_text);
    renderer->render(hp_variant_text);
    renderer->render(mov_const_text);
    renderer->render(mov_variant_text);
    renderer->render(atk_const_text);
    renderer->render(atk_variant_text);
  }
  if (player_id != 0)
  {
    if (enemy_units_alive <= 0)
    {
      ending_text.setString("YOU WIN \n"
                            "Press ESCAPE to exit.");
      renderer->render(ending_text);
    }
    else
    {
      if (!current_player_turn && is_ready)
      {
        renderer->render(turn_text);
      }
      if (current_player_turn)
      {
        renderer->render(end_turn_text);
      }
      if (player_units_alive > 0)
      {
        for (auto& unit : deployed_units)
        {
          if (unit != nullptr && unit->getPlayerUnitID() != 000)
          {
            if (unit->getStatus() != Unit::DEAD)
            {
              renderer->render(
                *sprites[static_cast<unsigned long long>(unit_to_sprite[unit->getPlayerUnitID()])]);
            }
          }
        }
        for (auto& enemy : enemy_units)
        {
          if (enemy != nullptr && enemy->getPlayerUnitID() != 000)
          {
            if (enemy->getStatus() != Unit::DEAD)
            {
              renderer->render(
                *sprites[static_cast<unsigned long long>(unit_to_sprite[enemy->getPlayerUnitID()])]);
            }
          }
        }
      }
      if (player_units_alive <= 0)
      {
        renderer->render(ending_text);
      }
    }
  }
}

/// Calls to fixedUpdate use the same fixed timestep
/// irrespective of how much time is passed. If you want
/// deterministic behaviour on clients, this is the place
/// to go.
///
/// https://gamedev.stackexchange.com/questions/1589/when-should-i-use-a-fixed-or-variable-time-step
/// "Use variable timesteps for your game and fixed steps for physics"
/// @param us
void ASGENetGame::fixedUpdate(const ASGE::GameTime& us)
{
  Game::fixedUpdate(us);
  if (!input_valid)
  {
    input_valid = true;
  }
}

/// Custom packet to send to the server
///@param bool data
void ASGENetGame::customInput(kissnet::tcp_socket& socket, bool data)
{
  MyPacket data_packet;
  data_packet << PacketID::BOOL;
  data_packet << data;

  socket.send(&data_packet.data()[0], data_packet.length());
}

/// Custom packet to send to the server
///@param Unit& unit
bool ASGENetGame::customInput(kissnet::tcp_socket& socket, Unit& unit)
{
  MyPacket data_packet;
  data_packet << PacketID::UNIT;
  data_packet << unit;

  socket.send(&data_packet.data()[0], data_packet.length());
  return true;
}

void ASGENetGame::playerReadyInput(kissnet::tcp_socket& socket) const
{
  MyPacket data_packet;
  data_packet << PacketID::READY;
  data_packet << player_id;

  socket.send(&data_packet.data()[0], data_packet.length());
}

/// The listening function of the client
/// Updates the client based on the data received from the server
///@param socket
void ASGENetGame::moreRun()
{
  while (running)
  {
    kissnet::buffer<4096> static_buffer;
    while (connected)
    {
      auto* client_ptr = dynamic_cast<GCNetClient*>(game_components[0].get());
      if (client_ptr != nullptr)
      {
        if (auto [size, valid] = client_ptr->socket.recv(static_buffer); valid)
        {
          if (valid.value == kissnet::socket_status::cleanly_disconnected)
          {
            connected = false;
            std::cout << "clean disconnection" << std::endl;
            client_ptr->socket.close();
            break;
          }

          if (size < static_buffer.size())
          {
            static_buffer[size] = std::byte{ 0 };
          }

          MyPacket packet;
          packet.data().resize(size);
          std::copy(
            static_buffer.begin(),
            static_buffer.begin() + static_cast<ptrdiff_t>(size),
            &packet.data()[0]);

          PacketID ide;
          packet >> ide;

          switch (ide)
          {
            case PacketID::ENEMY_UPDATE:
            {
              int unit_id = 0;
              int unit_hp = 0;
              packet >> unit_id;
              packet >> unit_hp;

              for (auto& deployed_unit : deployed_units)
              {
                if (deployed_unit != nullptr)
                {
                  if (unit_id == deployed_unit->getPlayerUnitID())
                  {
                    deployed_unit->setHP(unit_hp);
                    break;
                  }
                }
              }
              break;
            }
            case PacketID::BOOL:
            {
              bool data = false;
              packet >> data;
              current_player_turn = data;
              startTurn();
              break;
            }
            case PacketID::READY:
            {
              int id_ready = 0;
              packet >> id_ready;
              if (player_id == id_ready)
              {
                packet >> current_player_turn;
                updated = false;
              }
              break;
            }
            case PacketID::UNIT:
            {
              more_run_mutex.lock();
              if (enemy_units.size() != deployed_units.size())
              {
                Unit* data_unit = new Unit;
                packet >> *data_unit;

                if (data_unit != nullptr)
                {
                  if (unit_to_sprite[data_unit->getPlayerUnitID()] == 0)
                  {
                    auto new_sprite = renderer->createUniqueSprite();

                    switch (data_unit->getUnitClass())
                    {
                      case Unit::BASIC:
                        break;
                      case Unit::ARCHER:
                        new_sprite->loadTexture("/data/img/Archer_img.png");
                        break;
                      case Unit::KNIGHT:
                        new_sprite->loadTexture("/data/img/Knight_img.png");
                        break;
                      case Unit::PALADIN:
                        new_sprite->loadTexture("/data/img/Paladin_img.png");
                        break;
                      case Unit::INVALID:
                        break;
                    }

                    new_sprite->xPos(static_cast<float>(data_unit->getPosition()[0]));
                    new_sprite->yPos(static_cast<float>(data_unit->getPosition()[1]));
                    new_sprite->setGlobalZOrder(static_cast<int16_t>(70));
                    new_sprite->scale(1);
                    unit_to_sprite[data_unit->getPlayerUnitID()] = static_cast<int>(sprites.size());
                    unit_to_sprite[static_cast<int>(sprites.size())] = data_unit->getPlayerUnitID();
                    sprites.emplace_back(std::move(new_sprite));
                    enemy_units.emplace_back(data_unit);
                  }
                }
              }
              else
              {
                std::unique_ptr<Unit> discard_unit = std::make_unique<Unit>();
                packet >> *discard_unit;
                if (discard_unit->getPlayerID() != player_id)
                {
                  for (auto& enemy : enemy_units)
                  {
                    if (enemy->getPlayerUnitID() == discard_unit->getPlayerUnitID())
                    {
                      enemy->copy(*discard_unit);
                      break;
                    }
                  }
                }
                else
                {
                  for (auto& unit : deployed_units)
                  {
                    if (unit->getPlayerUnitID() == discard_unit->getPlayerUnitID())
                    {
                      unit->copy(*discard_unit);
                      break;
                    }
                  }
                }
              }
              more_run_mutex.unlock();
              break;
            }
            case PacketID::PLAYER_ID:
            {
              packet >> player_id;
              assignUnits();
              break;
            }
            case PacketID::UNIT_REQUEST:
            {
              MyPacket data_packet;
              data_packet << PacketID::UNIT_BUNDLE;
              int ammount = 3;
              data_packet << ammount;
              for (auto& unit : deployed_units)
              {
                if (unit != nullptr && unit->getUnitClass() != Unit::BASIC)
                {
                  data_packet << *unit;
                }
              }
              client_ptr->socket.send(&data_packet.data()[0], data_packet.length());

              break;
            }
            case PacketID::UNIT_BUNDLE:
            {
              int amount = 0;
              packet >> amount;
              for (int i = 0; i < amount; ++i)
              {
                Unit* data_unit = new Unit;
                packet >> *data_unit;

                if (data_unit != nullptr)
                {
                  if (unit_to_sprite[data_unit->getPlayerUnitID()] == 0)
                  {
                    auto new_sprite = renderer->createUniqueSprite();
                    switch (data_unit->getUnitClass())
                    {
                      case Unit::BASIC:
                        break;
                      case Unit::ARCHER:
                        new_sprite->loadTexture("/data/img/Archer_img.png");
                        break;
                      case Unit::KNIGHT:
                        new_sprite->loadTexture("/data/img/Knight_img.png");
                        break;
                      case Unit::PALADIN:
                        new_sprite->loadTexture("/data/img/Paladin_img.png");
                        break;
                      case Unit::INVALID:
                        break;
                    }

                    new_sprite->xPos(static_cast<float>(data_unit->getPosition()[0]));
                    new_sprite->yPos(static_cast<float>(data_unit->getPosition()[1]));
                    new_sprite->setGlobalZOrder(static_cast<int16_t>(70));
                    new_sprite->scale(1);
                    unit_to_sprite[data_unit->getPlayerUnitID()] = static_cast<int>(sprites.size());
                    unit_to_sprite[static_cast<int>(sprites.size())] = data_unit->getPlayerUnitID();
                    sprites.emplace_back(std::move(new_sprite));
                    enemy_units.emplace_back(data_unit);
                  }
                }
              }
              is_ready = true;
              break;
            }
            case PacketID::INVALID:
            {
              break;
            }
          }
        }
        else
        {
          if (connected)
          {
            connected = false;
            std::cout << "disconnected" << std::endl;
            client_ptr->socket.close();
          }
        }
      }
    }
  }
}

void ASGENetGame::initMap()
{
  tilemap.reserve(144);

  for (int y = 0; y < 9; y++)
  {
    for (int x = 0; x < 16; x++)
    {
      auto tile = Tile();
      tile.setTilePos(x, y);
      tilemap.emplace_back(tile);
    }
  }
}

void ASGENetGame::assignUnits()
{
  float position = TILE_SCALE;
  for (auto& unit : deployed_units)
  {
    if (unit != nullptr)
    {
      unit->setPlayerID(player_id);

      auto new_sprite = renderer->createUniqueSprite();
      switch (unit->getUnitClass())
      {
        case Unit::BASIC:
          break;
        case Unit::ARCHER:
          new_sprite->loadTexture("/data/img/Archer_img.png");
          break;
        case Unit::KNIGHT:
          new_sprite->loadTexture("/data/img/Knight_img.png");
          break;
        case Unit::PALADIN:
          new_sprite->loadTexture("/data/img/Paladin_img.png");
          break;
        case Unit::INVALID:
          break;
      }
      new_sprite->xPos(position);
      new_sprite->setGlobalZOrder(static_cast<int16_t>(position));
      position += TILE_SCALE;
      new_sprite->yPos(0);
      new_sprite->scale(1);
      unit_to_sprite[unit->getPlayerUnitID()] = static_cast<int>(sprites.size());
      sprites.emplace_back(std::move(new_sprite));
      if (player_id != 1)
      {
        unit->setPosition(unit->getPosition()[0] + 1200, unit->getPosition()[1]);
      }
    }
  }
}

/// Resolves the logic when moving your units
/// Determines whether the unit moves or attacks
///@param tile_x, tile_y
void ASGENetGame::manageMovementClick(int tile_x, int tile_y)
{
  auto& tile = tilemap[static_cast<unsigned long long>(tile_y * 16 + tile_x)];
  if (!moving_piece)
  {
    if (tile.getOccupied())
    {
      for (auto& unit : deployed_units)
      {
        if (
          unit->getPlayerID() == player_id && unit->getStatus() != Unit::Status::DEAD &&
          unit->getPosition()[0] / TILE_SCALE == static_cast<int>(tile_x) &&
          unit->getPosition()[1] / TILE_SCALE == static_cast<int>(tile_y))
        {
          active_unit = unit.get();
          tile.setOccupied(false);
          sprite->loadTexture(textures[active_unit->getUnitClass()]);
          moving_piece = true;
          updateText();
          break;
        }
      }
    }
  }
  else
  {
    if (tile.getOccupied())
    {
      if (!active_unit->getHasAttacked())
      {
        auto unit_tile_x = active_unit->getPosition()[0] / TILE_SCALE;
        auto unit_tile_y = active_unit->getPosition()[1] / TILE_SCALE;

        if (abs(unit_tile_x - tile_x) + abs(unit_tile_y - tile_y) <= active_unit->getAttackRange())
        {
          for (auto& enemy : enemy_units)
          {
            if (
              enemy->getPosition()[0] == tile_x * TILE_SCALE &&
              enemy->getPosition()[1] == tile_y * TILE_SCALE)
            {
              active_unit->setHasAttacked(true);
              enemy->takeDamage(active_unit->getDamage());
              if (enemy->getStatus() == Unit::Status::DEAD)
              {
                enemy_units_alive--;
              }

              MyPacket packet;
              packet << PacketID::ENEMY_UPDATE;
              packet << enemy->getPlayerUnitID();
              packet << enemy->getHealth();

              auto* client_ptr = getClientPtr();
              if (client_ptr != nullptr)
              {
                client_ptr->socket.send(&packet.data()[0], packet.length());
              }
              moving_piece = false;
              active_unit  = nullptr;
              break;
            }
          }
        }
      }
    }
    else if (
      active_unit->getPosition()[0] / TILE_SCALE == tile_x &&
      active_unit->getPosition()[1] / TILE_SCALE == tile_y)
    {
      moving_piece = false;
      active_unit  = nullptr;
    }
    else
    {
      if (!active_unit->getHasMoved())
      {
        auto unit_tile_x = active_unit->getPosition()[0] / TILE_SCALE;
        auto unit_tile_y = active_unit->getPosition()[1] / TILE_SCALE;

        if (abs(unit_tile_x - tile_x) + abs(unit_tile_y - tile_y) <= active_unit->getMovementRange())
        {
          active_unit->setPosition(tile_x * TILE_SCALE, tile_y * TILE_SCALE);
          active_unit->setHasMoved(true);
          auto* client_ptr = getClientPtr();
          if (client_ptr != nullptr)
          {
            customInput(client_ptr->socket, *active_unit);
          }
          active_unit  = nullptr;
          moving_piece = false;
        }
      }
    }
  }
}

/// Connects and updates the sprites to the units being displayed
///@param Unit* unit
void ASGENetGame::syncSprites(Unit* unit)
{
  if (unit != nullptr && unit->getPlayerUnitID() != 000)
  {
    sprites[static_cast<unsigned long long>(unit_to_sprite[unit->getPlayerUnitID()])]->xPos(
      static_cast<float>(unit->getPosition()[0]));
    sprites[static_cast<unsigned long long>(unit_to_sprite[unit->getPlayerUnitID()])]->yPos(
      static_cast<float>(unit->getPosition()[1]));

    int tile_x = unit->getPosition()[0] / TILE_SCALE;
    int tile_y = unit->getPosition()[1] / TILE_SCALE;

    if (unit->getStatus() != Unit::DEAD)
    {
      if (!moving_piece)
      {
        tilemap[static_cast<unsigned long long>(tile_y * 16 + tile_x)].setOccupied(true);
      }
    }
    else
    {
      tilemap[static_cast<unsigned long long>(tile_y * 16 + tile_x)].setOccupied(false);
    }
  }
}

/// Resets all units movement and attack stats
void ASGENetGame::startTurn()
{
  updated = false;
  for (auto& unit : deployed_units)
  {
    unit->setHasMoved(false);
    unit->setHasAttacked(false);
  }
}

void ASGENetGame::updateText()
{
  if (active_unit != nullptr)
  {
    hp_variant_text.setString(std::to_string(active_unit->getHealth()))
      .setPosition({ 8.5F * TILE_SCALE - hp_variant_text.getWidth() / 2, 8.9F * TILE_SCALE });
    mov_variant_text.setString(std::to_string(active_unit->getAttackRange()))
      .setPosition({ 10.5F * TILE_SCALE - mov_variant_text.getWidth() / 2,
                     8.25F * TILE_SCALE + mov_variant_text.getHeight() / 2 });
    atk_variant_text.setString(std::to_string(active_unit->getMovementRange()))
      .setPosition({ 10.5F * TILE_SCALE - atk_variant_text.getWidth() / 2,
                     8.75F * TILE_SCALE + atk_variant_text.getHeight() / 2 });
    if (active_unit->getHasMoved())
    {
      mov_const_text.setColour(ASGE::COLOURS::INDIANRED);
    }
    else
    {
      mov_const_text.setColour(ASGE::COLOURS::PALEGREEN);
    }
    if (active_unit->getHasAttacked())
    {
      atk_const_text.setColour(ASGE::COLOURS::INDIANRED);
    }
    else
    {
      atk_const_text.setColour(ASGE::COLOURS::PALEGREEN);
    }
  }
}
