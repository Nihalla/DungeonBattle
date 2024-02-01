
#include "ASGEServer.hpp"
#include <ASGEGameLib/Unit.hpp>
#include <Engine/Logger.hpp>
#include <iostream>

namespace
{
  std::list<kissnet::tcp_socket>& connections()
  {
    auto static connections_ = new std::list<kissnet::tcp_socket>;
    return *connections_;
  }

  std::vector<std::thread>& workers()
  {
    auto static workers_ = new std::vector<std::thread>;
    return *workers_;
  }

  // constexpr kissnet::port_t port = 12321;
  std::atomic running = true;
  using socket_cref   = std::reference_wrapper<const kissnet::tcp_socket>;
  using socket_list   = std::list<socket_cref>;
} // namespace

ASGEServer::~ASGEServer()
{
  delete &connections();
  delete &workers();
}

bool ASGEServer::init()
{
  net_server.start();
  return true;
}
int ASGEServer::run()
{
  while (running)
  {
    auto& socket = connections().emplace_back(net_server.server.accept());
    workers().emplace_back(
      [&]
      {
        listen(socket);

        std::cout << "detected disconnect\n";
        if (const auto SOCKET_ITER =
              std::find(connections().begin(), connections().end(), std::ref(socket));
            SOCKET_ITER != connections().end())
        {
          std::cout << "closing socket...\n";
          connections().erase(SOCKET_ITER);
        }
      });
    if (connection_idx != connections().size())
    {
      connection_idx = connections().size();
      tag();
    }
  }
  return EXIT_SUCCESS;
}
void ASGEServer::listen(kissnet::tcp_socket& socket)
{
  bool continue_receiving = true;
  kissnet::buffer<4096> static_buffer;
  while (continue_receiving)
  {
    // is there data ready for us?
    if (auto [size, valid] = socket.recv(static_buffer); valid)
    {
      if (valid.value == kissnet::socket_status::cleanly_disconnected)
      {
        continue_receiving = false; // client disconnected
      }
      MyPacket packet;
      packet.data().resize(size);
      std::copy(
        static_buffer.begin(),
        static_buffer.begin() + static_cast<ptrdiff_t>(size),
        &packet.data()[0]);
      PacketID ide = PacketID::INVALID;
      packet >> ide;
      MyPacket send_packet;

      switch (ide)
      {
        case PacketID::UNIT:
        {
          send(static_buffer, size, { socket });
          break;
        }
        case PacketID::READY:
        {
          if (players_ready < 2 && connections().size() > 1)
          {
            players_ready++;
            int player_id = 0;
            packet >> player_id;
            ids_ready[players_ready - 1] = player_id;
            initDisplayUnits(socket);
            if (players_ready == 2)
            {
              initInitiative();
            }
          }
          break;
        }
        case PacketID::BOOL:
        {
          send(static_buffer, size, { socket });
          break;
        }
        case PacketID::PLAYER_ID:
        {
          break;
        }
        case PacketID::ENEMY_UPDATE:
        {
          send(static_buffer, size, { socket });
          break;
        }
        case PacketID::UNIT_REQUEST:
        {
          break;
        }
        case PacketID::UNIT_BUNDLE:
        {
          send(static_buffer, size, { socket });
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
      continue_receiving = false;
      socket.close();
    }
  }
}
void ASGEServer::tag()
{
  unsigned long long temp_idx = 0;
  for (auto& connection : connections())
  {
    if (temp_idx == (connection_idx - 1) && connection_idx < 3)
    {
      MyPacket packet;

      PacketID id;
      id = PacketID::PLAYER_ID;
      packet << id;
      packet << static_cast<int>(temp_idx + 1);
      connection.send(&packet.data()[0], packet.length());
    }
    else
    {
      temp_idx++;
    }
  }
}
void ASGEServer::send(const kissnet::buffer<4096>& buffer, size_t length, const socket_list& exclude)
{
  for (auto& socket : connections())
  {
    if (auto it = std::find(exclude.cbegin(), exclude.cend(), socket); it == exclude.cend())
    {
      socket.send(buffer, length);
    }
  }
}
void ASGEServer::initDisplayUnits(kissnet::tcp_socket& socket)
{
  MyPacket packet;
  packet << PacketID::UNIT_REQUEST;
  for (auto& connection : connections())
  {
    if (connection != socket)
    {
      connection.send(&packet.data()[0], packet.length());
    }
  }
}
void ASGEServer::initInitiative()
{
  MyPacket firstp_packet;
  firstp_packet << PacketID::READY;
  firstp_packet << ids_ready[0];
  firstp_packet << true;
  for (auto& connection : connections())
  {
    connection.send(&firstp_packet.data()[0], firstp_packet.length());
  }
}
