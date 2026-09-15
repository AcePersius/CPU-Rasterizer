#ifndef TEXT_SYSTEM_HPP
#define TEXT_SYSTEM_HPP


void drawCharacter(XYcoord xy, framebuffer &buffer, const std::array<uint8_t, 7> &letter);
void drawString(XYcoord xy, framebuffer &buffer, std::string &string);

std::uint32_t WHITE_PIXEL = 0xFFFFFFFF;
std::uint32_t BLACK_PIXEL = 0x000000FF;

void drawString(XYcoord xy, framebuffer &buffer, std::string &string)
{
    for (int i = 0; i < string.length(); i++)
    {
        switch (string[i])
        {
        case 'A':
            drawCharacter(xy, buffer, A);
            xy.x += 7;
            break;
        case 'E':
            drawCharacter(xy, buffer, E);
            xy.x += 7;
            break;
        case 'F':
            drawCharacter(xy, buffer, F);
            xy.x += 7;
            break;
        case 'I':
            drawCharacter(xy, buffer, I);
            xy.x += 7;
            break;
        case 'M':
            drawCharacter(xy, buffer, M);
            xy.x += 7;
            break;
        case 'P':
            drawCharacter(xy, buffer, P);
            xy.x += 7;
            break;
        case 'R':
            drawCharacter(xy, buffer, R);
            xy.x += 7;
            break;
        case 'S':
            drawCharacter(xy, buffer, S);
            xy.x += 7;
            break;
        case 'T':
            drawCharacter(xy, buffer, T);
            xy.x += 7;
            break;
        case ' ':
            drawCharacter(xy, buffer, SPACE);
            xy.x += 7;
            break;
        case '0':
            drawCharacter(xy, buffer, ZERO);
            xy.x += 7;
            break;
        case '1':
            drawCharacter(xy, buffer, ONE);
            xy.x += 7;
            break;
        case '2':
            drawCharacter(xy, buffer, TWO);
            xy.x += 7;
            break;
        case '3':
            drawCharacter(xy, buffer, THREE);
            xy.x += 7;
            break;
        case '4':
            drawCharacter(xy, buffer, FOUR);
            xy.x += 7;
            break;
        case '5':
            drawCharacter(xy, buffer, FIVE);
            xy.x += 7;
            break;
        case '6':
            drawCharacter(xy, buffer, SIX);
            xy.x += 7;
            break;
        case '7':
            drawCharacter(xy, buffer, SEVEN);
            xy.x += 7;
            break;
        case '8':
            drawCharacter(xy, buffer, EIGHT);
            xy.x += 7;
            break;
        case '9':
            drawCharacter(xy, buffer, NINE);
            xy.x += 7;
            break;
        case ':':
            drawCharacter(xy, buffer, COLON);
            xy.x += 7;
            break;
        case '.':
            drawCharacter(xy, buffer, PERIOD);
            xy.x += 7;
            break;
        default:
            drawCharacter(xy, buffer, SPACE);
            xy.x += 7;
            break;
        }
    }
}

void drawCharacter(XYcoord xy, framebuffer &buffer, const std::array<uint8_t, 7> &letter)
{
    for (int j = 0; j < 7; ++j)
    {
        for (int i = 0; i < 7; ++i)
        {
            if (letter[j] & (1 << (6-i)))
            {
                buffer.colorPixels.pixels[(xy.x + i + (xy.y * frameWidth))] = WHITE_PIXEL;
                continue;
            }
            buffer.colorPixels.pixels[(xy.x + i + (xy.y * frameWidth))] = BLACK_PIXEL;
        }
        ++xy.y;
    }
}


#endif