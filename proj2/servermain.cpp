#include "serverprogm.h"


int main() {
  ServerProgm server;

  if (server.setup_server()) {
    server.run_server();
  }

  return 0;
}
