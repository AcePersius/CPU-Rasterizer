#ifndef TEXT_SYSTEM_HPP
#define TEXT_SYSTEM_HPP

#include <string>
#include <softwareRender/structsANDoperators.hpp>
#include <softwareRender/textsystem.hpp>


void drawCharacter(XYcoord xy, framebuffer &buffer, const std::array<uint8_t, 7> &letter);
void drawString(XYcoord xy, framebuffer &buffer, std::string &string);


#endif