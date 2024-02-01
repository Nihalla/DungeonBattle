

#ifndef GAMELIB_GCNETSERVER_HPP
#define GAMELIB_GCNETSERVER_HPP

#include "GComponent.hpp"
#include <atomic>
#include <kissnet.hpp>
#include <list>

class GCNetServer final : public GameComponent
{
 public:
  GCNetServer();
  ~GCNetServer() override;
  void update(double dt) override;
  void start();

  GCNetServer(const GCNetServer&) = delete;
  GCNetServer& operator=(const GCNetServer&) = delete;

  std::list<kissnet::tcp_socket> connections;
  kissnet::tcp_socket server;

 private:
  std::atomic<bool> accept_connections = false;
};

#endif // GAMELIB_GCNETSERVER_HPP
