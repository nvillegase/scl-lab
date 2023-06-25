#ifndef UTILS_H
#define UTILS_H

#include <iostream>

template< typename T >
std::string int_to_hex( T i ) {
  std::stringstream stream;
  // stream << "0x" 
  stream << std::setfill('0') << std::setw(sizeof(T)*2) 
         << std::hex << i;
  return stream.str();
}

#endif