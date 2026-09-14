#pragma once
#include <string>
#include <map>
#include "IBrickPropertyInterface.hpp"

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

void Deconstructor_IBrickPropertyInterfaceOverride(IBrickPropertyInterface*);
void ReflectBrickPropertiesOverride(IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection);
bool CanModifyBrickPropertyOverride(IBrickPropertyInterface* This);
void PostModifyBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyChangedEvent* Event);
void UpdateFocusedBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyFocusEvent* Event);