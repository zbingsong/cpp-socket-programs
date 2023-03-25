#include "servermainprog.h"

int main()
{
  ServerMainProgm server_main("32378", "30378", "31378");
  if (server_main.setup_server()) {
    server_main.run_server();
  }
  return 0;
}