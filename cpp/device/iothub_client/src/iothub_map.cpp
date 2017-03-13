#include <string>
#include <sstream>
#include <map>

#include "iothub_client.h"

#include "iothub_exception.hpp"
#include "iothub_map.hpp"

namespace azure { namespace iot {

    map_exception::map_exception(std::string _func, MAP_RESULT _result) :
        exception("IoTHubMapError", "IoTHubMap", _func, decode_error(_result))
    {}

    std::string map_exception::decode_error(MAP_RESULT result) const
    {
        std::stringstream s;
        s << "IoTHubMapResult.";
        switch (result)
        {
        case MAP_OK: s << "OK"; break;
        case MAP_ERROR: s << "ERROR"; break;
        case MAP_INVALIDARG: s << "INVALIDARG"; break;
        case MAP_KEYEXISTS: s << "KEYEXISTS"; break;
        case MAP_KEYNOTFOUND: s << "KEYNOTFOUND"; break;
        case MAP_FILTER_REJECT: s << "FILTER_REJECT"; break;
        }
        return s.str();
    }

    map::map(const map& map)
    {
        auto handle = Map_Clone(map.mMapHandle.get());
        if (handle == NULL)
        {
            throw map_exception(__func__, MAP_ERROR);
        }
        mMapHandle = std::shared_ptr<MAP_HANDLE_DATA_TAG>(handle, Map_Destroy);
    }

    map::map()
    {
        auto handle = Map_Create(NULL);
        if (handle == NULL)
        {
            throw map_exception(__func__, MAP_ERROR);
        }
        mMapHandle = std::shared_ptr<MAP_HANDLE_DATA_TAG>(handle, Map_Destroy);
    }

    map::map(MAP_HANDLE _mapHandle)
    {
        if (_mapHandle == NULL)
        {
            throw map_exception(__func__, MAP_ERROR);
        }
        auto handle = Map_Clone(_mapHandle);
        mMapHandle = std::shared_ptr<MAP_HANDLE_DATA_TAG>(handle, Map_Destroy);
    }

    MAP_HANDLE map::mapHandle() const
    {
        return mMapHandle.get();
    }

    void map::add(const std::string key, const std::string value)
    {
        auto result = Map_Add(mapHandle(), key.c_str(), value.c_str());
        if (result != MAP_OK)
        {
            throw map_exception(__func__, result);
        }
    }

    void map::addOrUpdate(const std::string key, const std::string value)
    {
        auto result = Map_AddOrUpdate(mapHandle(), key.c_str(), value.c_str());
        if (result != MAP_OK)
        {
            throw result;
        }
    }

    void map::erase(std::string key)
    {
        auto result = Map_Delete(mapHandle(), key.c_str());
        if (result != MAP_OK)
        {
            throw result;
        }
    }

    bool map::containsKey(std::string key) const
    {
        MAP_RESULT result;
        bool keyExists;
        result = Map_ContainsKey(mapHandle(), key.c_str(), &keyExists);
        if (result != MAP_OK)
        {
            throw map_exception(__func__, result);
        }
        return keyExists;
    }

    bool map::containsValue(std::string value) const
    {
        bool valueExists;
        auto result = Map_ContainsValue(mapHandle(), value.c_str(), &valueExists);
        if (result != MAP_OK)
        {
            throw map_exception(__func__, result);
        }
        return valueExists;
    }

    std::string map::getValueFromKey(std::string key) const
    {
        auto result = Map_GetValueFromKey(mapHandle(), key.c_str());
        if (result == NULL)
        {
            throw map_exception(__func__, MAP_KEYNOTFOUND);
        }
        return std::string(result);
    }

    std::map<std::string, std::string> map::getInternals() const
    {
        const char*const* keys;
        const char*const* values;
        std::map<std::string, std::string> keyValuePair;
        size_t count;
        auto result = Map_GetInternals(mapHandle(), &keys, &values, &count);
        if (result == MAP_OK)
        {
            for (unsigned int i = 0; i < count; i++)
            {
                auto key = std::string(keys[i]);
                auto value = std::string(values[i]);
                keyValuePair[key] = value;
            }
        }
        else
        {
            throw map_exception(__func__, result);
        }
        return keyValuePair;
    }
}}
