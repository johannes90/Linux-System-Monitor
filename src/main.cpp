#include "ncurses_display.h"
#include "system.h"
#include "format.h"
#include <iostream>


int main() {

  // Debug functions:
  long int longint = 51061;
  std::string test = Format::ElapsedTime(longint);
  std::cout << test;

  System system;
  NCursesDisplay::Display(system);
}