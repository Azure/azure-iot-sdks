#ifndef IOTHUB_MAP_HPP
#define IOTHUB_MAP_HPP

#include <string>
#include <map>
#include <memory>
#include "iothub_exception.hpp"

namespace azure { namespace iot {

    class map_exception : public azure::iot::exception {
    public:
        map_exception(std::string _func, MAP_RESULT _result);

        std::string decode_error(MAP_RESULT result) const;
    };

    class map {
        std::shared_ptr<MAP_HANDLE_DATA_TAG> mMapHandle;
        MAP_HANDLE mapHandle() const;
    public:
        map(const azure::iot::map& map);
        map();
        map(MAP_HANDLE _mapHandle);
        
        void add(const std::string key, const std::string value);
        void addOrUpdate(const std::string key, const std::string value);
        void erase(std::string key);
        bool containsKey(std::string key) const;
        bool containsValue(std::string value) const;
        std::string getValueFromKey(std::string key) const;
        std::map<std::string, std::string> getInternals() const;
    };

}}

#endif
