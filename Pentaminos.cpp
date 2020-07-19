#include "Pentaminos.hpp"

uint32_t Pentamino::shapes[12] =
{
    0x2492,//2942,//0100100100100100
    0x0C92,//29C0,
    0x02D2,//2D20,
    0x04D2,//2D40,
    0x05D0,//0D50,
    0x02D8,//8D20,
    0x04B8,//8B40,
    0x0698,//8960,
    0x0E48,//84E0,
    0x0CC8,//8CC0,
    0x0598,//8950,
    0x0C98
    /*
    0b0010000100001000010000100,//0x2492,//2942,//0100100100100100
    0b0010000100001000011000000,//0x0C92,//29C0,
    0b0010000100011000100000000,//0x02D2,//2D20,
    0b0010000100011000010000000,//0x04D2,//2D40,
    0b0000000100011100010000000,//0x05D0,//0D50,
    0b0000000110001100010000000,//0x02D8,//8D20,
    0b0000001110001000010000000,//0x04B8,//8B40,
    0b0000001100001000110000000,//0x0698,//8960,
    0b0010000100001110000000000,//0x0E48,//84E0,
    0b0000001000011000011000000,//0x0CC8,//8CC0,
    0b0000001100001100010000000,//0x0598,//8950,
    0b0000001100001000011000000//0x0C98//89C0*/
};//3x5 grid...
/*
INSIDE_PIECE = 0,
SINGLE_WALL,
CHANNEL_WALLS,
WINZONE,
ANGLE_WALLS,
END_WALLS,
BACKGROUND,
EMPTY

I = 0,
    L,
    N,
    Y,
    X,
    P,
    T,
    U,
    V,
    W,
    F,
    Z*/
//some way to store the id and the rotation together... 3 bits + 2 bits = 8 bits
//just add and bitshift
uint8_t Pentamino::shapeTextures[60] =
{
    //TexShape          TexOrientation
    //I_SHAPE(
    END_WALLS       +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    END_WALLS       +   (DEFAULT <<4),

    //L_SHAPE()
    END_WALLS       +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    ANGLE_WALLS     +   (DEFAULT <<4),
    END_WALLS       +   (R90CCW  <<4),

    //N_SHAPE()
    END_WALLS       +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    ANGLE_WALLS     +   (R90CW   <<4),
    ANGLE_WALLS     +   (R90CCW  <<4),
    END_WALLS       +   (DEFAULT <<4),

    //Y_SHAPE()
    END_WALLS       +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    END_WALLS       +   (R90CW   <<4),
    SINGLE_WALL     +   (R180    <<4),
    END_WALLS       +   (DEFAULT <<4),

    //X_SHAPE()
    END_WALLS       +   (R180    <<4),
    END_WALLS       +   (R90CW   <<4),
    INSIDE_PIECE    +   (DEFAULT <<4),
    END_WALLS       +   (R90CCW  <<4),
    END_WALLS       +   (DEFAULT <<4),

    //P_SHAPE()
    ANGLE_WALLS     +   (R90CW   <<4),
    ANGLE_WALLS     +   (R180    <<4),
    SINGLE_WALL     +   (DEFAULT <<4),
    ANGLE_WALLS     +   (R90CCW  <<4),
    END_WALLS       +   (DEFAULT <<4),

    //T_SHAPE()
    END_WALLS       +   (R90CW   <<4),
    SINGLE_WALL     +   (R90CW   <<4),
    END_WALLS       +   (R90CCW  <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    END_WALLS       +   (DEFAULT <<4),

    //U_SHAPE()
    END_WALLS       +   (R90CW   <<4),
    ANGLE_WALLS     +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    END_WALLS       +   (R90CW   <<4),
    ANGLE_WALLS     +   (R90CCW  <<4),

    //V_SHAPE()
    END_WALLS       +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    ANGLE_WALLS     +   (DEFAULT <<4),
    CHANNEL_WALLS   +   (R90CCW  <<4),
    END_WALLS       +   (R90CCW  <<4),

    //W_SHAPE()
    END_WALLS       +   (R180    <<4),
    ANGLE_WALLS     +   (DEFAULT <<4),
    ANGLE_WALLS     +   (R180    <<4),
    ANGLE_WALLS     +   (DEFAULT <<4),
    END_WALLS       +   (R90CCW  <<4),

    //F_SHAPE()
    END_WALLS       +   (R90CW   <<4),
    ANGLE_WALLS     +   (R180    <<4),
    SINGLE_WALL     +   (DEFAULT <<4),
    END_WALLS       +   (R90CCW  <<4),
    END_WALLS       +   (DEFAULT <<4),

    //Z_SHAPE()
    END_WALLS       +   (R90CW   <<4),
    ANGLE_WALLS     +   (R180    <<4),
    CHANNEL_WALLS   +   (DEFAULT <<4),
    ANGLE_WALLS     +   (DEFAULT <<4),
    END_WALLS       +   (R90CCW  <<4)
};

const Orientation rots[4] = {DEFAULT, R90CCW, R180, R90CW};

/***
sf::Color WeldonBlue(0x9C986BD0);//FF);//119,156,171);//0x779CABFF);
sf::Color MountainMeadow(0x32DE8AD0);// 50,222,138);
sf::Color Tuscan(0xFCD499D0);
sf::Color OceanBlue(0x4637AED0);
sf::Color SmashedPumpkin(0xFF6B35D0);//is full! value is why it breaks!
sf::Color UsafaBlue(0x004E89D0);
sf::Color OperaMauve(0xBA7BA1D0);
sf::Color Grullo(0xA0927ED0);
sf::Color RoseTaupe(0x8C6264D0);
sf::Color FrenchWine(0xAF1B3FD0);
sf::Color KombuGreen(0x2F4236D0);
sf::Color PurpleMountainMajesty(0x9772A8D0);

sf::Color Beige(0xF4F1DE);//245,245,220);//0xF5F5DC);//F4F1DE);//white bg
sf::Color GunMetal(0x2A3036);//dark bg


sf::Color blockColors[14] = 
{
    /*
    sf::Color(174, 95, 69, 192),
    sf::Color(0, 158, 98, 192),
    sf::Color(45, 89, 174, 192),
    sf::Color(90, 90, 110, 192),
    sf::Color(153, 51, 153, 192),
    sf::Color(203, 8, 23, 192),
    sf::Color(173, 132, 53, 192),
    sf::Color(153, 139, 76, 192),
    sf::Color(101, 193, 0, 192),
    sf::Color(199, 78, 0, 192),// i like
    sf::Color(179,179, 0, 192),
    sf::Color(17, 193, 22, 192),
    *  /
    WeldonBlue,
    MountainMeadow,
    Tuscan,
    OceanBlue,
    SmashedPumpkin,// i like
    UsafaBlue,
    OperaMauve,
    Grullo,
    RoseTaupe,
    FrenchWine,//i like 
    KombuGreen,
    PurpleMountainMajesty,
    sf::Color::White,//Beige,
    sf::Color::Black//(128,128,128)
};
****/
/*
sf::Vector2f Pentamino::textureOffsets[12] = 
{
    //
}*/



//5 x 5 grid address space...
void transformAddress(int n, unsigned int &ci, unsigned int &cj, Orientation rot, bool isFlipped)
{
    if(isFlipped) n = n + ((n % 3) - 1) * -2;//horizontally flipped across index 2 [012345] -> [54321]
    
    switch (rot)
    {
    case DEFAULT:
        ci = 1 + n % 3;
        cj = n / 3;
//        ci = n%5;//1 + n % 3;
//        cj = n/5;//n / 3;
        break;
    case R90CCW:
        ci = 4 - n / 3;
        cj = 1 + n % 3;
//        ci = 4 - n/5;//4 - n / 3;
//        cj = n%5;//1 + n % 3;
        break;
    case R180:
        ci = 3 - n % 3;
        cj = 4 - n / 3;
//        ci = 3-n%5;//3 - n % 3;
//        cj = 4-n/5;//4 - n / 3;
        break;
    case R90CW:
        ci = n / 3;
        cj = 3 - n % 3;
//        ci = n/5;//n / 3;
//        cj = 3-n%5;//3 - n % 3;
        break;
    default:
        break;
    }
}

void Pentamino::rotateRight()
{
    //5x5 grid
    for(auto &C : coords)
    {
        int temp = C.i;
        C.i = C.j;
        C.j = -temp;
    }
    m_orientation = isFlipped ? (m_orientation + 1) % 4 : (m_orientation + (4 - 1)) % 4;
    for(auto &o : orientations)
    {//isFLipped????
        if(!isFlipped)
        {
            if(o == 3) o = 0;
            else ++o;
        }
        else
        {
            if(o == 0) o = 3;
            else --o;
        }
        
        //o = (o + 1)%4;
    }
}

void Pentamino::rotateLeft()
{
    //5x5 grid
    for(auto &C : coords)
    {
        int temp = C.i;
        C.i = -C.j;
        C.j = temp;
    }
    m_orientation = isFlipped ? (m_orientation + (4 - 1)) % 4 : (m_orientation + 1) % 4;
    for(auto &o : orientations)
    {
        if(isFlipped)
        {
            if(o == 3) o = 0;
            else ++o;
        }
        else
        {
            if(o == 0) o = 3;
            else --o;
        }
        //if(o == 0) o = 3;
        //else --o;
    }
}

void Pentamino::rotate180()
{
    //5x5 grid
    for(auto &C : coords)
    {
        C.i *= -1;
        C.j *= -1;
    }
    m_orientation = (m_orientation + 2) % 4;
    for(auto &o : orientations)
    {
        //add 2 circularly
        o = (o + 2) % 4;
    }
}

void Pentamino::flip()
{
    //swap x
    for(auto &C : coords)
    {
        C.i *= -1;
    }
    isFlipped = !isFlipped;
}

//load the pieces and the bits... then transform it to the rotation afterwards (rotate)
Pentamino::Pentamino(unsigned int i, Coord c, Orientation r, bool flipped) : isFlipped(flipped), id(i), defaultPos(c)//, defaultRot(r)
{
    //load block...
    //get the local coordinate points to draw...
    int ctr = 0;
    for(int n = 0; n < 15; ++n)//n = bit Position in shape, is it valid?
    {
        if(checkBit(shapes[id], n))
        {
            Coord C;
            transformAddress(n, C.i, C.j, r, flipped);
            C.i -= 2;
            C.j -= 2;

            texIDs[ctr] = shapeTextures[id*5 + ctr] & 0b00001111;//constant
            orientations[ctr] = (shapeTextures[id*5 + ctr] & 0b11110000) >> 4;
            //orientation transformation...
            orientations[ctr] = isFlipped ? (orientations[ctr] + r) % 4 : (orientations[ctr] + (4 - r)) % 4;
            coords[ctr++] = C;
        }
    }
    m_orientation = r;
}

void Pentamino::setOrientation(Orientation newOrientation, bool flipped)
{
    if(flipped != isFlipped) flip();
    if(newOrientation == m_orientation) return;
    if(newOrientation == ((m_orientation + 1)%4))//circular!
    {
        rotateLeft();
        return;
    }
    if(((newOrientation + 1)%4) == m_orientation)
    {
        rotateRight();
        return;
    }
    rotate180();
}