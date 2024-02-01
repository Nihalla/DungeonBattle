//
// Created by james on 28/01/2021.
//

#ifndef ASGENETGAME_ASGESERVER_HPP
#define ASGENETGAME_ASGESERVER_HPP

#include "atomic"
#include "list"
#include "thread"
#include <ASGEGameLib/GCNetServer.hpp>
#include <ASGEGameLib/MyPacket.hpp>
#include <mutex>
#include <vector>

class ASGEServer
{
  using socket_cref = std::reference_wrapper<const kissnet::tcp_socket>;
  using socket_list = std::list<socket_cref>;

 public:
  ASGEServer() = default;
  ~ASGEServer();

  bool init();
  int run();
  void listen(kissnet::tcp_socket& socket);
  void send(const kissnet::buffer<4096>& buffer, size_t length, const socket_list& exclude = {});
  void tag();

  void initInitiative();
  void initDisplayUnits(kissnet::tcp_socket& socket);

 private:
  GCNetServer net_server;
  unsigned long long connection_idx = 0;
  unsigned long long players_ready  = 0;
  std::array<int, 2> ids_ready      = { 0, 0 };
};

#endif // ASGENETGAME_ASGESERVER_HPP
