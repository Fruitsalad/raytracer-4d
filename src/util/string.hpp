#ifndef INSTANT_DICTIONARY_STRING_HPP
#define INSTANT_DICTIONARY_STRING_HPP

#include "typedefs.hpp"

template<class ListType>
auto& getLast(ListType& list, uint offset = 0) {
  return list[list.size() - offset];
}


inline bool endsWith(String$& suffix, String$& string) {
  if (string.size() < suffix.size())
    return false;
  
  for (uint i = 0; i < suffix.size(); i++)
    if (getLast(string, i) != getLast(suffix, i))
      return false;
  
  return true;
}

#endif //INSTANT_DICTIONARY_STRING_HPP
