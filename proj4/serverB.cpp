#include "backendprog.h"

int main()
{
  BackendProg serverB("B", "dataB.csv", "31378");
  if (serverB.setup_server()) {
    serverB.run_server();
  }
  return 0;
}
