#include <exception>
#include <string>
#include <sstream>

#include "iothub_exception.hpp"

namespace azure { namespace iot {

    exception::exception(const std::string& _exc, const std::string& _cls, const std::string& _func, const std::string &_aux):
        std::runtime_error(_cls + "." + _func + ", " + _aux + "\n")
    {}

}}
