#ifndef FRUIT_EXCEPTION_HPP
#define FRUIT_EXCEPTION_HPP

#include <exception>
#include <iostream>
#include "typedefs.hpp"

#ifdef __EMSCRIPTEN__
// Emscripten generally does not support exceptions (at least not efficiently)
// so we just log the error instead.
#define THROW(...) std::cerr << String(__VA_ARGS__) << std::endl \
  << "Previous error occurred in function \"" << String(__PRETTY_FUNCTION__) \
  << "\", in source file \"" << String(__FILE__) << "\"." << std::endl
#define THROW_TYPE(type, ...) THROW(__VA_ARGS__)
#elifdef __GNUC__
#define THROW(...) throw Exception(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define THROW_TYPE(type, ...) throw type(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#else
#define THROW(...) throw Exception(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define THROW_TYPE(type, ...) throw type(__FILE__, __LINE__, __func__, __VA_ARGS__)
#endif

class Exception : public std::exception
{
public:
  Exception() : message("Something went wrong but I don't know what.") {}
  
  explicit Exception(String message)
      : message(std::move(message)) {}
  
  Exception(String file, int line, String function, String message)
      : message(std::move(message)),
        function(std::move(function)),
        file(std::move(file)),
        line(line) {}
  
  [[nodiscard]] const char* what() const noexcept override {
    return message.c_str();
  }
  
  // Addendums are useful for adding notes like "~ While processing vertex.glsl"
  void addNote(String$& note) {
    message += "\n" + note;
  }
  
  String getPrettyPrint() {
    return "### Exception thrown in function \""+function+"\" "
           "(line "+$(line)+" of "+file+") ###\n"
      + message + "\n";
  }
  
  String message;
  String function;
  String file;
  int line = 0;
};


template<class CustomFunction>
void runAndPrintExceptions(CustomFunction doSomething) {
  try {
    doSomething();
  } catch (Exception& exception) {
    std::cerr << exception.getPrettyPrint() << std::endl;
  } catch (std::exception& exception) {
    std::cerr << exception.what() << std::endl;
  } catch (...) {
    std::cerr << "Oops! Mysterious error. Have fun debugging." << std::endl;
  }
}


#endif //FRUIT_EXCEPTION_HPP
