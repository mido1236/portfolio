#include <football_server/GameServer.h>
#include <football_server/RedisWriter.h>

struct App {
  InBoundQueue inbound;
  OutBoundQueue outbound;
  UwsHub hub{inbound};
  EgressStats egressStats;
  RedisWriter writer{outbound,
                     egressStats,
                     {.host = "loved-lynx-83691.upstash.io",
                      .port = 6379,
                      .password = "gQAAAAAAAUbrAAIncDI5N2QwZjNhYzk0ZTQ0MzkxOTQ3"
                                  "YThhZGNmYzhhMjU5NnAyODM2OTE"}};
  GameServer server{inbound, hub, hub, outbound, &writer};
};

int main() {
  auto app = App();
  app.server.start();
  std::cout << "Done" << std::endl;
  app.writer.start();
  return 0;
}
