#include "clientprogm.h"


int main() {
  ClientProgm client;

  if (client.setup_client()) {
    client.run_client();
  }

  return 0;
}
