#include "PropertyInterfaceOverrides.hpp"
#include "Extension.hpp"

bool CanModifyBrickPropertyOverride(IBrickPropertyInterface* This)
{
	return true;
}

void PostModifyBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyChangedEvent* Event)
{

}

void UpdateFocusedBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyFocusEvent* Event)
{

}