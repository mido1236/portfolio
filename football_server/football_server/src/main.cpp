#include "../../football_server_lib/include/football_server/GameServer.h"

struct App {
  InBoundQueue inbound;
  UwsHub hub{inbound};
  GameServer server{inbound, hub};
};

int main() {
  App().server.start();
  return 0;
}
