//
// Created by ahmed on 2026-04-16.
//

#include <client/SimClient.h>
#include <iostream>

using std::cout;
using std::endl;

int main() {
  SimClient client;

  client.connect();
  cout << "Done" << endl;
  return 0;
}