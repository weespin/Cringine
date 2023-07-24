#pragma once
#include <any>
#include <cassert>
#include <iostream>
#include <map>
#include <regex>
#include <unordered_map>
#include <string>
#include <typeindex>

#include "Utils/ColorHelper.h"

enum class EPropertyType
{
    BOOL,
    //Characters
    CHAR, 
    WCHAR,
    STRING,
    //NUMBERS
    UCHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    LONG,
    ULONG,
    LONGLONG,
    ULONGLONG,
    //Floating Point
    FLOAT,
    DOUBLE,
    //Math
    FLOAT4,
    FLOAT4X4,
    MATRIX,
    //Engine Specific Stuff
    TRANSFORM,
    COLOR_RGBA
};
class IPropertyManager
{
public:
    virtual ~IPropertyManager() = default; // Virtual destructor makes the class polymorphic
private:
    std::map<std::string, std::any> properties;

public:
    template<typename T>
    void RegisterProperty(const std::string& name, T* property)
    {
        properties[name] = std::any(property);
    }

    template<typename T>
    T* GetProperty(const std::string& name)
    {
	    for (auto element : properties)
	    {
		    if(element.first == name)
		    {
                try
                {
                    return std::any_cast<T*>(element.second);
                }
                catch (const std::bad_any_cast&)
                {
                    // Handle bad cast if the stored type doesn't match T
                }
		    }
	    }
        return nullptr;
    }

    std::map<std::string, std::any> GetAllProperties()
    {
        return properties;
    }
};

template<typename T>
struct PropertyBinder
{
    T* variable;

    PropertyBinder(T* var, const char* pszName, IPropertyManager* manager) : variable(var)
    {
        assert(pszName);
        manager->RegisterProperty(pszName, var);
    }

    operator T& () const { return *variable; }

    PropertyBinder& operator=(const T& value)
    {
        *variable = value;
        return *this;
    }
};


#define CREATE_PROPERTY(type, name, defaultValue) \
    type name = defaultValue; \
    type* name ## _ptr = &name; \
    PropertyBinder<type> name ## _bind = PropertyBinder<type>(name ## _ptr, #name, this); \

#define CREATE_PROPERTY_DEFAULT(type, name) \
    CREATE_PROPERTY(type,name, {})

#define INSERT_TO_UC_VECTOR(dataptr, size) reinterpret_cast<unsigned char*>(dataptr), reinterpret_cast<unsigned char*>(dataptr) + size
#define INSERT_CONST_TO_UC_VECTOR(dataptr, size) reinterpret_cast<const unsigned char*>(dataptr), reinterpret_cast<const unsigned char*>(dataptr) + size 
#define SERIALIZE_POD_TYPE(Type, EnumType) \
    else if (value.type() == typeid(Type*)) \
    { \
        EPropertyType PropertyType = EnumType; \
        dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&PropertyType, sizeof(PropertyType))); \
        auto Converted = std::any_cast<Type*>(value); \
        dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(Converted, sizeof(Type))); \
    }

#define SERIALIZE_SUBPROPERTY(Type) \
    do \
    { \
        auto Converted = std::any_cast<Type*>(value); \
        auto ConvertedProperties = Converted->GetAllProperties(); \
        unsigned long ConvertedPropertiesSize = ConvertedProperties.size(); \
        dataOut.insert(dataOut.end(), reinterpret_cast<unsigned char*>(&ConvertedPropertiesSize), reinterpret_cast<unsigned char*>(&ConvertedPropertiesSize) + sizeof(ConvertedPropertiesSize)); \
        for (auto [name, value] : ConvertedProperties) \
        { \
            auto type_name = name; \
            unsigned long type_name_size = type_name.size(); \
            dataOut.insert(dataOut.end(), reinterpret_cast<unsigned char*>(&type_name_size), reinterpret_cast<unsigned char*>(&type_name_size) + sizeof(type_name_size)); \
            dataOut.insert(dataOut.end(), reinterpret_cast<const unsigned char*>(type_name.c_str()), reinterpret_cast<const unsigned char*>(type_name.c_str()) + type_name_size); \
            SerializeValue(value, dataOut); \
        } \
    } while (false)


void SerializeValue(const std::any& value, std::vector<unsigned char>& dataOut);
void DeserializeValue(unsigned char** dataIn, IPropertyManager* PropertyManager);