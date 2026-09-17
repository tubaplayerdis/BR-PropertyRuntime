#include "PropertyInterfaceOverrides.hpp"

template<typename T>
T* ReoverVirtualTable(std::string signature)
{
	return nullptr;
	//Use the signature class (on pc) to get the address. cast the result into a pointer to the real virtual table.
	void* Address = 0;
}