// This is the entrypoint for the normal program build. This file isn't in
// the src folder because the .cpp files in src are shared with the unit test
// build. The files unique to the unit test build are in the test folder.

// This is the only file unique to the normal build. I could put this file in a
// separate folder but it seems silly to make a folder for one file.

#ifdef ENABLE_DOCTEST
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#endif

#include <SDL2/SDL.h>

void realMain();

// SDL-Compatible main function
extern "C" int main(int argument_count, char* arguments[])
{
  realMain();
  return 0;
}
