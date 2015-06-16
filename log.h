#pragma once

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

enum class Severity
{
  Debug,
  Info,
  Warning,
  Error,
};

namespace black_magic 
{

  template<Severity severity> inline const char * getSeverityLabel();
  template<> inline const char * getSeverityLabel<Severity::Debug>() { return "[DEBUG] "; }
  template<> inline const char * getSeverityLabel<Severity::Info>() { return "[INFO ] "; }
  template<> inline const char * getSeverityLabel<Severity::Warning>() { return "[WARN ] "; }
  template<> inline const char * getSeverityLabel<Severity::Error>() { return "[ERROR] "; }

  template<typename ... Args>
  inline std::string stringFmt( const char* format, Args ... args )
  {
    const size_t size = std::snprintf( nullptr, 0, format, args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    std::snprintf( buf.get(), size, format, args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
  }

} // namespace black_magic

template<Severity severity, typename ... Args>
inline void log(const char * format, Args ... args) 
{
  std::cout << black_magic::getSeverityLabel<severity>() << black_magic::stringFmt(format, args...) << std::endl;
}

#define LOG_DEBUG(...) log<Severity::Debug>(__VA_ARGS__)
#define LOG_INFO(...) log<Severity::Info>(__VA_ARGS__)
#define LOG_WARN(...) log<Severity::Warning>(__VA_ARGS__)
#define LOG_ERROR(...) log<Severity::Error>(__VA_ARGS__)
