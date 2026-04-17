//
// Created by ahmed on 2026-04-16.
//

#include <client/SimClient.h>
#include <football/Match.h>
#include <IXWebSocket.h>

void SimClient::connect() {
  ix::WebSocket app;
  //
  // auto connection =
  //     app.connect("", {.open = [](auto *ws) {},
  //                      .message = [](auto *ws, uWS::OpCode opCode,
  //                                    const char *message, size_t size) {},
  //                      .close = [](auto *ws) {}});
}
void SimClient::sendInput() {
  InputCmd input{.matchId = 0, .clientTick = 0, .ax = 0, .ay = 0, .buttons = 0};
}