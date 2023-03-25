#include "backendprog.h"

int main()
{
  BackendProg serverB("B", "dataB.txt", "31378", "32378");
  if (serverB.setup_server()) {
    serverB.run_server();
  }
  return 0;
}
