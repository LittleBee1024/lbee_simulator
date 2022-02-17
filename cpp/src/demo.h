#pragma once

#include <cstdint>
#include <cfloat>

namespace DEMO
{
/**
 * @brief sub two numbers
 * 
 * @return int 
 */
int sub(int a, int b);

/**
 * @brief multiply two numbers
 * 
 * @return int 
 */
int multiply(int a, int b);

/**
 * @brief Get the string pointer
 * 
 * @return const char* 
 */
const char* get_string();

/**
 * @brief Set the string object
 * 
 * @param s 
 * @return int 
 */
int set_string(const char* s);

}
