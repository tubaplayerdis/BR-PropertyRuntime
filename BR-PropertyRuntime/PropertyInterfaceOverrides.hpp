#pragma once
#include <string>
#include <map>
#include "IBrickPropertyInterface.hpp"
#include "BP_IBrickPropertyInterface_classes.hpp"

//Some virtual tables have strings post the virtual table declarations in rdata.
template<typename T>
T* ReoverVirtualTable(std::string signature);

extern std::map<std::string, IBrickPropertyInterface*> InterfaceRegistry;

inline SDK::UBrickEditorObject* GetEditorObject(IBrickPropertyInterface* This)
{
	//Since the IBrickPropertyInterface is at base + 0x28 or base + 0x30 (Uobject size) for the 
	/*
	* UBrickEditorObject
	* UObject members
	* IBrickPropertyInterface*
	* UBrickEditorObject Members
	*/
	return reinterpret_cast<SDK::UBrickEditorObject*>(
		reinterpret_cast<std::uint8_t*>(This) - sizeof(SDK::UObject));

}

inline SDK::IBP_IBrickPropertyInterface_C* GetAsInterface(IBrickPropertyInterface* This)
{
	return reinterpret_cast<SDK::IBP_IBrickPropertyInterface_C*>(GetEditorObject(This));
}

void Deconstructor_IBrickPropertyInterfaceOverride(IBrickPropertyInterface*);
void ReflectBrickPropertiesOverride(IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection);
bool CanModifyBrickPropertyOverride(IBrickPropertyInterface* This);
void PostModifyBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyChangedEvent* Event);
void UpdateFocusedBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyFocusEvent* Event);
