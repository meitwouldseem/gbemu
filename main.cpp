#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Bus.h"

using namespace std;

int main(int argc, char* argv[])
{
    Bus* gameboy = new Bus();

    gameboy->cpu.RunTillStop();

    gameboy->dumptiles();

    gameboy->printbg();

    gameboy->drawbg();
}
