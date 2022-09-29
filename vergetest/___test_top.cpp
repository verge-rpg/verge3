#define DOCTEST_CONFIG_IMPLEMENT

#include "fixtures/custom_reporters.h"

#include "./doctest.h"

int main(int argc, char** argv)
{
  doctest::Context ctx;
  ctx.setOption("abort-after", 100); // default - stop after 5 failed asserts DO NOT COMMIT
  ctx.applyCommandLine(argc, argv);  // apply command line - argc / argv
  ctx.setOption("no-breaks", true);  // override - don't break in the debugger
  ctx.setOption("fc", true);         // force colors!  (probably does nothing.)
  ctx.setOption("s", true);          // include successful assertions in output
  ctx.setOption("d", true);          // show duration of each test

  const int res = ctx.run(); // run test cases unless with --no-run

  if (ctx.shouldExit()) // query flags (and --exit) rely on this
    return res;       // propagate the result of the tests

  // your actual program execution goes here - only if we haven't exited

  return res; // + your_program_res
}

TEST_CASE("testing math")
{
  CHECK(0xFF != 0xFF000000);
  CHECK(0xFF == 0x000000FF);
}