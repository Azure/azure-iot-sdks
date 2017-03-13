#ifndef IOTHUB_EXCEPTION_HPP
#define IOTHUB_EXCEPTION_HPP

#include <exception>
#include <string>

namespace azure { namespace iot {

    class exception : public std::runtime_error {
    public:
        exception(const std::string& _exc, const std::string& _cls, const std::string& _func, const std::string &_aux);
    };

}}

#endif
