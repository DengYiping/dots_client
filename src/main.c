#include "log.h"
#include "argparse.h"

int main(int argc, char** argv) {
  log_info("Hello, world!");
  struct argparse_option options = {
  };
  return 0;
}
