// This tells Catch to provide a main() - only do this in one cpp file:
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "falaise/falaise.h"


int main(int argc, char* argv[])
{
  falaise::initialize(argc, argv);

  int result = Catch::Session().run( argc, argv );

  falaise::terminate();

  return result;
}

