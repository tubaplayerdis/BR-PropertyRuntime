#pragma once
#include <string>
#include <map>
#include "IBrickPropertyInterface.hpp"

static auto InterfaceRegistry = std::map<std::string, IBrickPropertyInterface>();

void Deconstructor_IBrickPropertyInterfaceOverride(IBrickPropertyInterface*);
void ReflectBrickPropertiesOverride(SDK::UBrickEditorObject* This, FBrickPropertyReflection* Reflection);
bool CanModifyBrickPropertyOverride(SDK::UBrickEditorObject* This);
void PostModifyBrickPropertyOverride(SDK::UBrickEditorObject* This, const FBrickPropertyChangedEvent* Event);
void UpdateFocusedBrickPropertyOverride(SDK::UBrickEditorObject* This, const FBrickPropertyFocusEvent* Event);