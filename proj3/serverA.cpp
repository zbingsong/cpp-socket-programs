#include "backendprog.h"

int main()
{
  BackendProg serverA("A", "dataA.txt", "30378", "32378");
  if (serverA.setup_server()) {
    serverA.run_server();
  }
  return 0;
}
