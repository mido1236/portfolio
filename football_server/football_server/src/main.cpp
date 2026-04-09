#include <football_server/GameServer.h>
#include <football_server/RedisWriter.h>

struct App {
  InBoundQueue inbound;
  OutBoundQueue outbound;
  UwsHub hub{inbound};
  EgressStats egressStats;
  RedisWriter writer{outbound,
                     egressStats,
                     {.host = "right-hawk-95248.upstash.io",
                      .port = 6379,
                      .password = "gQAAAAAAAXQQAAIncDFkNjY5M2E3NDlhMWY0MGYwYjBk"
                                  "NTM1MTU0MzdjMmUwMHAxOTUyNDg"}};
  GameServer server{inbound, hub, hub, outbound, &writer};
};

int main() {
  auto app = App();
  app.server.start();
  std::cout << "Done" << std::endl;
  app.writer.start();
  return 0;
}
