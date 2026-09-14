#include "PropertyInterfaceOverrides.hpp"
#include "Extension.hpp"

#ifdef BRMK_SDK
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName)>                                AddBrickProperty("48 89 5C 24 10 55 56 57 48 83 EC 60", "BrickRigsModKitSteam-BrickRigs.dll");
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName, SDK::FText* InDisplayName)>     AddBrickPropertyDisplayInfo("48 89 5C 24 10 55 56 57 41 56 41 57 48 81", "BrickRigsModKitSteam-BrickRigs.dll");
Function<TSharedRef<FBoolBrickProperty>* (TSharedRef<FBoolBrickProperty>*)>                                                                              ConstructBrickProperty_Bool("E8 ?? ?? ?? ?? 0F BA EE 08", "BrickRigsModKitSteam-BrickRigs.dll");
#else
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName)>                                AddBrickProperty("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 48 8D 59");
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName, SDK::FText* InDisplayName)>     AddBrickPropertyDisplayInfo("48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC 50 49 8B D9");
Function<TSharedRef<FBoolBrickProperty>* (TSharedRef<FBoolBrickProperty>*)>                                                                             ConstructBrickProperty_Bool("E8 ?? ?? ?? ?? 48 8B 8D 38 07 00 00");
#endif


void DeclareBooleanProperty(SDK::FBP_FBrickPropertyDeclaration Declaration, FBrickPropertyReflection* Reflection)
{
	TSharedRef<FBoolBrickProperty> PropertyReference;
	ConstructBrickProperty_Bool(&PropertyReference);

	std::cout << Declaration.PropertyName_6_90CFF1AA403BE3727D57088D9A0E8480.GetStringRef().ToString() << std::endl;

	FBrickPropertyInstance Instance = FBrickPropertyInstance{
		.BrickProperty = PropertyReference,
		.FullPropertyName = Declaration.PropertyName_6_90CFF1AA403BE3727D57088D9A0E8480.GetStringRef(),
		.ParentPropertyChain = SDK::TArray<SDK::FStructProperty>()
	};
	if (Reflection->bIsSerializing)
	{
		AddBrickProperty(Reflection, &PropertyReference, &Instance.FullPropertyName);
	}
	else
	{
		AddBrickPropertyDisplayInfo(Reflection, &PropertyReference, &Instance.FullPropertyName, &Declaration.DisplayName_7_E5FF33B84665E94A6EF8C49F03E1C2C5);
		for (SDK::TPair<TSharedRef<FBrickPropertyEditInfo>, int> EditInfos : Reflection->BrickPropertyEditInfos)
		{
			FBrickPropertyEditInfo* EditInfo = EditInfos.First.Object;
			EditInfo->DescriptionText = Declaration.Description_10_FEF8680340F9F4222764FE876625C467;
			EditInfo->bIsEnabled = Declaration.bIsEnabled_12_C37DF3214F1B585DB23291A83D02D5FE;
			EditInfo->bIsReadOnly = Declaration.bIsReadOnly_14_36BD3D53474F4FE5A355B294996AD4E5;
			EditInfo->ColorStyle = Declaration.ColorStyle_17_E6E6524E41F70E650422F49B6491187E;
			EditInfo->MaxComboBoxItemsPerRow = 0;
			EditInfo->MaxComboBoxListItems = 0;
		}
	}
}

void ReflectBrickPropertiesOverride(IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection)
{
	std::cout << "Custom function!. Calling original now: " << GetEditorObject(This) << std::endl;

	/*
	auto BPInterface = reinterpret_cast<SDK::IBP_IBrickPropertyInterface_C*>(GetEditorObject(This));
	SDK::FBP_ShouldCallSuperSettings ShouldCallSuperSettings;
	BPInterface->ShouldCallSuperReflectBrickProperties(&ShouldCallSuperSettings);
	const bool CallSuper = ShouldCallSuperSettings.ShouldCallSuper_1_3CC922DD4B3C9B04AF8CC79AF56C037B;
	const bool CallAtStart = ShouldCallSuperSettings.CallAtStart_3_2B65F40F4734A4579FD5929B66219043;
	*/

	auto SuperFunction = [](IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection) -> void
	{
		auto Brick = GetEditorObject(This);
		auto Interface = InterfaceRegistry.find(Brick->Class->Name.ToString());
		for (const auto& [key, value] : InterfaceRegistry) {
			std::cout << key << " : " << value << "\n";
		}
		if (Interface != InterfaceRegistry.end())
		{
			std::cout << (uintptr_t)Interface->second->ReflectBrickProperties - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") << std::endl;
			Interface->second->ReflectBrickProperties(This, Reflection);
		}
	};

	SuperFunction(This, Reflection);
	return;

	if (true/*CallSuper && CallAtStart*/)
	{
		SuperFunction(This, Reflection);
	}

	return;
	/*

	//Actually Reflect Properties
	auto Declarations = SDK::TArray<SDK::FBP_FBrickPropertyDeclaration>();
	BPInterface->ReflectBrickProperties(&Declarations);

	for (const auto &Declare : Declarations)
	{
		switch (Declare.Type_33_5AFD4DE54866742EFCB3248308311FFE)
		{
		case SDK::EBP_BrickPropertyType::Bool:
			DeclareBooleanProperty(Declare, Reflection);
			break;
		default:
			break;
		}
	}

	if (CallSuper && !CallAtStart)
	{
		SuperFunction(This, Reflection);
	}
	*/
}