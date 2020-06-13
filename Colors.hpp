#ifndef SFML_COLOR_PALETTE_HPP
#define SFML_COLOR_PALETTE_HPP

#include <SFML/Graphics.hpp>

struct HSL
{
    float hue;
    float saturation;
    float lightness;
};
//0 - 1
sf::Color fromHSL(HSL hsl);
sf::Color fromHSL(float h, float s, float l);

HSL fromRGB(sf::Color rgb);
HSL fromRGB(float r, float g, float b);

//color functions
sf::Color setLuminance(sf::Color color, float luminance);

sf::Color brighten(sf::Color color, float amount = 1.75f);

sf::Color desaturateByAvg(sf::Color &color);

sf::Color desaturateByLuma(sf::Color &color);

sf::Color alphaBlend(sf::Color &color, sf::Color &baseColor);

sf::Color variate(sf::Color color, int amount);

sf::Color setValue(sf::Color color, int v);

sf::Color randColor();

#endif