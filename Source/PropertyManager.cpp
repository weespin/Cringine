#include "PropertyManager.h"

#include "EngineHeaders.h"
#define DESERIALIZE_POD_PROPERTY(Type) \
	do { \
	Type Value; \
	memcpy(&Value, *dataIn, sizeof(Value)); \
	* dataIn += sizeof(Value); \
	auto VP = PropertyManager->GetProperty<Type>(PropertyName); \
	assert(VP); \
	* VP = Value; \
	} while (false); \
	break;

void DeserializeValue(unsigned char** dataIn, IPropertyManager* PropertyManager)
{
	unsigned long nNameSize;
	std::memcpy(&nNameSize, *dataIn, sizeof(nNameSize));
	*dataIn += sizeof(nNameSize);

	std::string PropertyName;
	PropertyName.resize(nNameSize, '\0');
	memcpy(PropertyName.data(), *dataIn, nNameSize);
	*dataIn += nNameSize;

	EPropertyType propertyType;
	std::memcpy(&propertyType, *dataIn, sizeof(propertyType));
	*dataIn += sizeof(propertyType);

	switch (propertyType)
	{
		case EPropertyType::BOOL:
		{
			DESERIALIZE_POD_PROPERTY(bool);
		}
		case EPropertyType::CHAR:
		{
			DESERIALIZE_POD_PROPERTY(char);
		}
		case EPropertyType::WCHAR: 
		{
			DESERIALIZE_POD_PROPERTY(wchar_t);
		}
		case EPropertyType::STRING: 
		{
			unsigned long nStringLen;
			memcpy(&nStringLen, *dataIn, sizeof(nStringLen));
			*dataIn += sizeof(nStringLen);
			std::string Value;
			Value.resize(nStringLen, '\0');
			memcpy(Value.data(), *dataIn, nStringLen);
			*dataIn += nStringLen;
			auto VP = PropertyManager->GetProperty<std::string>(PropertyName);
			assert(VP);
			*VP = Value;
			break;
		}
		case EPropertyType::UCHAR:
		{
			DESERIALIZE_POD_PROPERTY(unsigned char);
		}
		case EPropertyType::SHORT:
		{
			DESERIALIZE_POD_PROPERTY(short);
		}
		case EPropertyType::USHORT:
		{
			DESERIALIZE_POD_PROPERTY(unsigned short);
		}
		case EPropertyType::INT:
		{
			DESERIALIZE_POD_PROPERTY(int);
		}

		case EPropertyType::UINT:
		{
			DESERIALIZE_POD_PROPERTY(unsigned int);
		}
		case EPropertyType::LONG:
		{
			DESERIALIZE_POD_PROPERTY(long);
		}
		case EPropertyType::ULONG:
		{
			DESERIALIZE_POD_PROPERTY(unsigned long);
		}
		case EPropertyType::LONGLONG: 
		{
			DESERIALIZE_POD_PROPERTY(long long);
		}
		case EPropertyType::ULONGLONG: 
		{
			DESERIALIZE_POD_PROPERTY(unsigned long long);
		}
		case EPropertyType::FLOAT: 
		{
			DESERIALIZE_POD_PROPERTY(float);
		}
		case EPropertyType::DOUBLE: 
		{
			DESERIALIZE_POD_PROPERTY(double);
		}
		case EPropertyType::FLOAT4: 
		{
			DESERIALIZE_POD_PROPERTY(XMFLOAT4);
		}
		case EPropertyType::FLOAT4X4: 
		{
			DESERIALIZE_POD_PROPERTY(XMFLOAT4X4);
		}
		case EPropertyType::MATRIX: 
		{
			DESERIALIZE_POD_PROPERTY(XMMATRIX);
		}
		case EPropertyType::TRANSFORM:
		{
			auto VP = PropertyManager->GetProperty<Transform>(PropertyName);
			assert(VP);
			unsigned long nSubProperties;
			std::memcpy(&nSubProperties, *dataIn, sizeof(nSubProperties));
			*dataIn += sizeof(nSubProperties);
			for(unsigned long nSubProperty = 0; nSubProperty < nSubProperties; ++nSubProperty)
			{
				//Let recursion finish this bs

				DeserializeValue(dataIn, VP);
			}

			
			break;
		}
		case EPropertyType::COLOR_RGBA: 
		{
			DESERIALIZE_POD_PROPERTY(ColorHelper::RGBA);
		}
	default: 
		assert(false);;
	}
}

void SerializeValue(const std::any& value, std::vector<unsigned char>& dataOut)
{

	if (value.type() == typeid(std::string*))
	{
		EPropertyType PropertyType = EPropertyType::STRING;
		dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&PropertyType, sizeof(PropertyType)));

		auto Converted = std::any_cast<std::string*>(value);
		unsigned long stringLen = Converted->size();
		dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&stringLen, sizeof(stringLen)));
		dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(Converted->c_str(), stringLen));
	}
	SERIALIZE_POD_TYPE(bool, EPropertyType::BOOL)
	SERIALIZE_POD_TYPE(wchar_t, EPropertyType::WCHAR)
	SERIALIZE_POD_TYPE(char, EPropertyType::CHAR)
	SERIALIZE_POD_TYPE(unsigned char, EPropertyType::UCHAR)
	SERIALIZE_POD_TYPE(short, EPropertyType::SHORT)
	SERIALIZE_POD_TYPE(unsigned short, EPropertyType::USHORT)
	SERIALIZE_POD_TYPE(int, EPropertyType::INT)
	SERIALIZE_POD_TYPE(unsigned int, EPropertyType::UINT)
	SERIALIZE_POD_TYPE(long, EPropertyType::LONG)
	SERIALIZE_POD_TYPE(unsigned long, EPropertyType::ULONG)
	SERIALIZE_POD_TYPE(long long, EPropertyType::LONGLONG)
	SERIALIZE_POD_TYPE(unsigned long long, EPropertyType::ULONGLONG)
	SERIALIZE_POD_TYPE(float, EPropertyType::FLOAT)
	SERIALIZE_POD_TYPE(double, EPropertyType::DOUBLE)
	SERIALIZE_POD_TYPE(XMFLOAT4, EPropertyType::FLOAT4)
	SERIALIZE_POD_TYPE(XMMATRIX, EPropertyType::MATRIX)
	SERIALIZE_POD_TYPE(XMFLOAT4X4, EPropertyType::FLOAT4X4)
	SERIALIZE_POD_TYPE(ColorHelper::RGBA, EPropertyType::COLOR_RGBA)
	else if (value.type() == typeid(Transform*))
	{
		EPropertyType PropertyType = EPropertyType::TRANSFORM;
		dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&PropertyType, sizeof(PropertyType)));
		//Serialize included Properties
		SERIALIZE_SUBPROPERTY(Transform);
	}
	else
	{
		assert(false);
	}
}
