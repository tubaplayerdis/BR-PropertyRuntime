#include "PropertyInterfaceOverrides.hpp"
#include "Extension.hpp"

bool CanModifyBrickPropertyOverride(IBrickPropertyInterface* This)
{
	return true;
}

void PostModifyBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyChangedEvent* Event)
{
	auto BPInterface = GetAsInterface(This);
	SDK::FBP_ShouldCallSuperSettings ShouldCallSuperSettings;
	BPInterface->ShouldCallSuperPostModifyBrickProperty(&ShouldCallSuperSettings);
	const bool CallSuper = ShouldCallSuperSettings.ShouldCallSuper_1_3CC922DD4B3C9B04AF8CC79AF56C037B;
	const bool CallAtStart = ShouldCallSuperSettings.CallAtStart_3_2B65F40F4734A4579FD5929B66219043;

	auto SuperPostModify = [](IBrickPropertyInterface* This, const FBrickPropertyChangedEvent* Event) -> void
		{
			auto Brick = GetEditorObject(This);
			auto Interface = InterfaceRegistry.find(Brick->Class->Name.ToString());
			if (Interface != InterfaceRegistry.end())
			{
				Interface->second->PostModifyBrickProperty(This, Event);
			}
		};

	if (CallSuper && CallAtStart)
	{
		SuperPostModify(This, Event);
	}

	//TODO: Call custom interface implementation

	if (CallSuper && !CallAtStart)
	{
		SuperPostModify(This, Event);
	}
}

void UpdateFocusedBrickPropertyOverride(IBrickPropertyInterface* This, const FBrickPropertyFocusEvent* Event)
{
	auto BPInterface = GetAsInterface(This);
	SDK::FBP_ShouldCallSuperSettings ShouldCallSuperSettings;
	BPInterface->ShouldCallSuperUpdateFocusedBrickProperty(&ShouldCallSuperSettings);
	const bool CallSuper = ShouldCallSuperSettings.ShouldCallSuper_1_3CC922DD4B3C9B04AF8CC79AF56C037B;
	const bool CallAtStart = ShouldCallSuperSettings.CallAtStart_3_2B65F40F4734A4579FD5929B66219043;

	auto SuperUpdateFoucused = [](IBrickPropertyInterface* This, const FBrickPropertyFocusEvent* Event) -> void
		{
			auto Brick = GetEditorObject(This);
			auto Interface = InterfaceRegistry.find(Brick->Class->Name.ToString());
			if (Interface != InterfaceRegistry.end())
			{
				Interface->second->UpdateFocusedBrickProperty(This, Event);
			}
		};

	if (CallSuper && CallAtStart)
	{
		SuperUpdateFoucused(This, Event);
	}

	//TODO: Call custom interface implementation

	if (CallSuper && !CallAtStart)
	{
		SuperUpdateFoucused(This, Event);
	}
}