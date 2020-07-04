#include "Colors.hpp"

namespace COLOR
{

//https://stackoverflow.com/questions/2353211/hsl-to-rgb-color-conversion

sf::Color fromHSL(HSL hsl)
{
    return fromHSL(hsl.hue, hsl.saturation, hsl.lightness);
}

//[0, 1] 
sf::Color fromHSL(float hue, float saturation, float lightness)
{
    if(saturation > 1.f) saturation = 1.f;
    auto hueToRGB = [](float p, float q, float t) -> float
    {
        if(t < 0.f) t += 1.f;
        if(t > 1.f) t -= 1.f;
        if(t < 1.f/6.f) return p + (q - p) * 6.f * t;
        if(t < 1.f/2.f) return q;
        if(t < 2.f/3.f) return p + (q - p) * (2.f/3.f  - t) * 6.f;
        return p;
    };

    float q = lightness < 0.5f ? lightness * (1.f + saturation) : lightness + saturation - (lightness * saturation);
    float p = 2.f * lightness - q;
    float r = hueToRGB(p, q, hue + 1.f/3.f);
    float g = hueToRGB(p, q, hue);
    float b = hueToRGB(p, q, hue - 1.f/3.f);

    return sf::Color(255.f * r, 255.f * g, 255.f * b);
}

//r, g, b in range [0,1]
HSL fromRGB(float r, float g, float b)
{
    float h, s, l;

    float cmax = std::max(r,std::max(g,b));
    float cmin = std::min(r,std::min(g,b));
    
    h = s = l = (cmax + cmin) / 2.f;

    if(cmax == cmin)
    {
        h = s = 0;
    }
    else
    {
        float d = cmax - cmin;
        s = l > 0.5f ? d / (2.f - cmax - cmin) : d / (cmax + cmin);
        if(cmax == r)
            h = (g - b) / d + (g < b ? 6.f : 0.f);
        else if(cmax == g)
            h = (b - r) / d + 2.f;
        else
            h = (r - g) / d + 4.f;
        h /= 6.f;
    }
    return {h,s,l};
}

HSL fromRGB(sf::Color rgb)
{
    float r = (float)rgb.r / 255.f;
    float g = (float)rgb.g / 255.f;
    float b = (float)rgb.b / 255.f;

    return fromRGB(r,g,b);
}

//color functions
sf::Color brighten(sf::Color color, float amount)
{
    //if(amount > 1.f){
    color.r = std::min((float)color.r * amount, 255.f);
    color.g = std::min((float)color.g * amount, 255.f);
    color.b = std::min((float)color.b * amount, 255.f);
    /*}
    else
    {
    color.r = std::max((float)color.r * amount, 0.f);
    color.g = std::max((float)color.g * amount, 0.f);
    color.b = std::max((float)color.b * amount, 0.f);
    }*/
    
    return color;
}

sf::Color setLuminance(sf::Color color, float luminance)//luminance 0-1
{
    luminance *= 255;
    float curLuminance = 0.299f * float(color.r) + 0.587f * float(color.g) + 0.114f * float(color.b);
    float factor = luminance / curLuminance;
    return sf::Color(float(color.r) * factor, float(color.g) * factor, float(color.b) * factor);
}

sf::Color desaturateByAvg(sf::Color &color)
{
    int luminance = (float)(color.r + color.g + color.b)/3.f;
    return sf::Color(luminance,luminance,luminance,color.a);
}

sf::Color desaturateByLuma(sf::Color &color)
{
    int luminance = 0.299f * float(color.r) + 0.587f * float(color.g) + 0.114f * float(color.b);
    return sf::Color(luminance,luminance,luminance,color.a);
}

sf::Color alphaBlend(sf::Color &color, sf::Color &baseColor)
{//Alpha blending color function
    float a = (float)color.a / 255.f;
    sf::Color destColor;
    destColor.r = ((float)color.r * a) + ((float)baseColor.r * (1 - a));
    destColor.g = ((float)color.g * a) + ((float)baseColor.g * (1 - a));
    destColor.b = ((float)color.b * a) + ((float)baseColor.b * (1 - a));
    return destColor;
}

//dependant on the average luminosity?
sf::Color variate(sf::Color color, int amount)
{
    if(color.r < 128) color.r += amount;
    else color.r -= amount;
    if(color.g < 128) color.g += amount;
    else color.g -= amount;
    if(color.b < 128) color.b += amount;
    else color.b -= amount;
    return color;
}

//hmmm
//set the luminance = avg color / 3 ???
sf::Color setValue(sf::Color color, int v)
{
    float curV = float(color.r + color.g + color.b) / 3.f;
    float ratio = float(v)/float(curV);
    //find the max value...
//    int maxV = std::max(std::max(color.r, color.g), color.b);
//    float ratio = float(v)/float(maxV);
    color.r = float(color.r) * ratio;
    color.g = float(color.g) * ratio;
    color.b = float(color.b) * ratio;
    return color;
}

sf::Color randColor()
{
    return sf::Color(rand()%256,rand()%256,rand()%256);
}

}//namespace COLOR