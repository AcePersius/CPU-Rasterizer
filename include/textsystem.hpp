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
        case 'B':
            drawCharacter(xy, buffer, B);
            xy.x += 7;
            break;
        case 'C':
            drawCharacter(xy, buffer, C);
            xy.x += 7;
            break;
        case 'D':
            drawCharacter(xy, buffer, D);
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
        case 'G':
            drawCharacter(xy, buffer, G);
            xy.x += 7;
            break;
        case 'H':
            drawCharacter(xy, buffer, H);
            xy.x += 7;
            break;
        case 'I':
            drawCharacter(xy, buffer, I);
            xy.x += 7;
            break;
        case 'J':
            drawCharacter(xy, buffer, J);
            xy.x += 7;
            break;
        case 'K':
            drawCharacter(xy, buffer, K);
            xy.x += 7;
            break;
        case 'L':
            drawCharacter(xy, buffer, L);
            xy.x += 7;
            break;
        case 'M':
            drawCharacter(xy, buffer, M);
            xy.x += 7;
            break;
        case 'N':
            drawCharacter(xy, buffer, N);
            xy.x += 7;
            break;
        case 'O':
            drawCharacter(xy, buffer, O);
            xy.x += 7;
            break;
        case 'P':
            drawCharacter(xy, buffer, P);
            xy.x += 7;
            break;
        case 'Q':
            drawCharacter(xy, buffer, Q);
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
        case 'U':
            drawCharacter(xy, buffer, U);
            xy.x += 7;
            break;
        case 'V':
            drawCharacter(xy, buffer, V);
            xy.x += 7;
            break;
        case 'W':
            drawCharacter(xy, buffer, W);
            xy.x += 7;
            break;
        case 'X':
            drawCharacter(xy, buffer, X);
            xy.x += 7;
            break;
        case 'Y':
            drawCharacter(xy, buffer, Y);
            xy.x += 7;
            break;
        case 'Z':
            drawCharacter(xy, buffer, Z);
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
        case '!':
            drawCharacter(xy, buffer, BANG);
            xy.x += 7;
            break;
        case '?':
            drawCharacter(xy, buffer, QUESTION);
            xy.x += 7;
            break;
        case ',':
            drawCharacter(xy, buffer, COMMA);
            xy.x += 7;
            break;
        case '/':
            drawCharacter(xy, buffer, FSLASH);
            xy.x += 7;
            break;
        case '\\':
            drawCharacter(xy, buffer, BSLASH);
            xy.x += 7;
            break;
        case '(':
            drawCharacter(xy, buffer, LPARENTH);
            xy.x += 7;
            break;
        case ')':
            drawCharacter(xy, buffer, RPARENTH);
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
                buffer.colorPixels.pixels[(xy.x + i + (xy.y * settings.frameWidth))] = WHITE_PIXEL;
                continue;
            }
            buffer.colorPixels.pixels[(xy.x + i + (xy.y * settings.frameWidth))] = BLACK_PIXEL;
        }
        ++xy.y;
    }
}


#endif