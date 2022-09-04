#pragma once

extern "C"
{

/**
 * @brief convert assembly code to machine code
 * 
 * @param buf is a pointer to assembly code buffer
 * @return machine code
 */
const char *assemble(const char *buf);

}
