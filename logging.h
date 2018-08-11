// Copied from https://stackoverflow.com/a/51802606/353407

#include <iostream>

struct Log
{
    ~Log(void) { ::std::cout << ::std::endl; }
};

template<typename T> Log &&
operator <<(Log && wrap, T const & whatever)
{
    ::std::cout << whatever;
    return ::std::move(wrap);
}

constexpr uint8_t DEBUG = 0;
constexpr uint8_t INFO = 1;
constexpr uint8_t ERROR = 2;

constexpr uint8_t CURRENT_LOG_LEVEL = DEBUG;
#define LOG(level) if(CURRENT_LOG_LEVEL <= level) Log()
