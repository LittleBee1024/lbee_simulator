#pragma once

#include <string>

namespace LBEE_SIMULATOR
{

/**
 * @brief convert assembly code to machine code
 * 
 * @param buf is a pointer to assembly code buffer
 * @return std::string is the machine code string
 */
std::string assemble(const char *buf);

}
