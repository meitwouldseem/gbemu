#include "Bus.h"
#include <iostream>
#include <bitset>
#include <fstream>
#include <SFML/Graphics.hpp>


Bus::Bus()
{
    cpu.ConnectBus(this);
}

Bus::~Bus()
{
    //dtor
}

void Bus::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0xffff)
        ram[addr] = data;
    if (addr == 0x8010 || addr == 0x8012)
    {
        std::cout << "WRITE TO VRAM LOCATION:" << std::endl;
        cpu.PrintStatus();
    }
}

//the following two function will need cleaning up later.
uint8_t Bus::read(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x00ff)
    {
        return bootrom[addr];
    }
    else if (addr >= 0x0104 && addr <= 0x0133)
    {
        return logo[addr-0x0104];
    }
    else if (addr >= 0x0134 && addr <= 0x014D)
    {
        return checksum[addr-0x0134];
    }
    else if (addr == 0xff44)
    {
        return 144;//simulates the LCD driver.
    }
    else if (addr >= 0x0000 && addr <= 0xffff)
    {
        return ram[addr];
    }

    return 0x00;
}

uint8_t& Bus::getref(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x00ff) {
        return bootrom[addr];
    } else if (addr >= 0x0000 && addr <= 0xffff) {
        return ram[addr];
    }

    throw "BAD REFERENCE";
}

void Bus::dumptiles()
{
    using namespace std;

    uint8_t b1, b2;

    cout << "tileset 1:" << endl;

    for (uint16_t i=0x8000; i < 0x9000; i+=2)
    {
        b1 = read(i);
        b2 = read(i+1);
        if (b1 != 0x0000)
            cout << dec << +(i-0x8000) << "(" << hex << +i << "):" << bitset<8>(b1) << endl;
        if (b2 != 0x0000)
            cout << dec << +(i-0x8000) << "(" << hex << +i << "):" << bitset<8>(b2) << endl;
    }

    cout << "tileset 2:" << endl;

    for (uint16_t i=0x8800; i < 0x9800; i+=2)
    {
        b1 = read(i);
        b2 = read(i+1);
        if (b1 != 0x0000)
            cout << dec << +(i-0x8800) << "(" << hex << +i << "):" << bitset<8>(b1) << endl;
        if (b2 != 0x0000)
            cout << dec << +(i-0x8800) << "(" << hex << +i << "):" << bitset<8>(b2) << endl;
    }
}

void Bus::printbg()
{
    using namespace std;

    //std::array<std::array<short, 256>, 256> bg = {};

    for (uint16_t addr=0x0; addr < 0x0400; addr++)
    {
        cout << +read(addr+0x9800);
        if ((addr) % 32 == 0)
            cout << endl;
    }
}

void Bus::drawbg()
{
    sf::Uint8* pixels = new sf::Uint8[256 * 256 * 4];

    uint8_t B1;
    uint8_t B2;
    bool b1;
    bool b2;
    int pixel;

    for (uint16_t addr=0x0; addr < 0x0400; addr++)
    {
        uint16_t tile = read(addr+0x9800);

        for (uint16_t row=0; row < 8; row++)
        {
            B1 = read((row*2)+tile);
            B2 = read((row*2)+tile+1);

            for (int p=0; p < 8; p++)
            {
                pixel = ((addr*8)+p+(row*256))*4;
                b1 = (B1 & (0x80 >> p));
                b2 = (B2 & (0x80 >> p));

                if (b1 && b2)
                {
                    //std::cout << "black" << std::endl;
                    pixels[pixel] = 255;
                    pixels[pixel+1] = 255;
                    pixels[pixel+2] = 255;
                    pixels[pixel+3] = 255;
                }
                else if (b1 && !b2)
                {
                    //std::cout << "Dgrey" << std::endl;
                    pixels[pixel] = 168;
                    pixels[pixel+1] = 168;
                    pixels[pixel+2] = 168;
                    pixels[pixel+3] = 255;
                }
                else if (!b1 && b2)
                {
                    //std::cout << "Lgrey" << std::endl;
                    pixels[pixel] = 84;
                    pixels[pixel+1] = 84;
                    pixels[pixel+2] = 84;
                    pixels[pixel+3] = 255;
                }
                else
                {
                    pixels[pixel] = 0;
                    pixels[pixel+1] = 0;
                    pixels[pixel+2] = 0;
                    pixels[pixel+3] = 255;
                }
            }
        }
    }


    std::cout << "BG draw done" << std::endl;

    sf::Texture bg;
    if (!bg.create(256, 256))
    {
        std::cout << "Failed to create texture" << std::endl;
        return;
    }

    bg.update(pixels);

    sf::Sprite background;
    background.setTexture(bg);

//    sf::Texture test;
//    if (!test.loadFromFile("test.jpg"))
//    {
//        std::cout << "Texture load failed" << std::endl;
//        return;
//    }
//    background.setTexture(test);

    sf::RenderWindow window(sf::VideoMode(256, 256), "Haze EMU BG test");

    std::cout << "Entering draw loop" << std::endl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Blue);
        window.draw(background);
        window.display();
    }

    std::cout << "Draw loop done" << std::endl;

    return;
}

