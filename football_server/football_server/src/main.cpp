#include <football_server/GameServer.h>

struct App {
  InBoundQueue inbound;
  UwsHub hub{inbound};
  GameServer server{inbound, hub, hub};
};

int main() {
  App().server.start();
  return 0;
}
