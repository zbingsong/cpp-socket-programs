#include "backendprog.h"

int main()
{
  BackendProg serverA("A", "dataA.csv", "30378");
  if (serverA.setup_server()) {
    serverA.run_server();
  }
  return 0;
}
