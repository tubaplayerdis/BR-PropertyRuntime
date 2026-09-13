#include "PropertyInterfaceOverrides.hpp"
#include "Extension.hpp"

bool CanModifyBrickPropertyOverride(SDK::UBrickEditorObject* This)
{
	return true;
}

void PostModifyBrickPropertyOverride(SDK::UBrickEditorObject* This, const FBrickPropertyChangedEvent* Event)
{

}

void UpdateFocusedBrickPropertyOverride(SDK::UBrickEditorObject* This, const FBrickPropertyFocusEvent* Event)
{

}