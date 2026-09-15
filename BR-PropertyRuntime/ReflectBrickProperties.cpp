#include "PropertyInterfaceOverrides.hpp"
#include "Extension.hpp"
#include "Hooks.hpp"

#ifdef BRMK_SDK
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName)>                                AddBrickProperty("48 89 5C 24 10 55 56 57 48 83 EC 60", "BrickRigsModKitSteam-BrickRigs.dll");
Function<FBrickPropertyEditInfo*(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName, SDK::FText* InDisplayName)>     AddBrickPropertyDisplayInfo("48 89 5C 24 10 55 56 57 41 56 41 57 48 81", "BrickRigsModKitSteam-BrickRigs.dll");
Function<TSharedRef<FBoolBrickProperty>* (TSharedRef<FBoolBrickProperty>*)>                                                                             ConstructBrickProperty_Bool("E8 ?? ?? ?? ?? 0F BA EE 08", "BrickRigsModKitSteam-BrickRigs.dll", true);
Function<SDK::FProperty*(SDK::UStruct*, SDK::FName)>																									FindPropertyByName(Hooks::GetSymbolAddress("BrickRigsModKitSteam-CoreUObject.dll", "?FindPropertyByName@UStruct@@QEBAPEAVFProperty@@VFName@@@Z"));
Function<SDK::FText*(SDK::FText* This, SDK::FText* That)>																								FTextCopyOperator(Hooks::GetSymbolAddress("BrickRigsModKitSteam-Core.dll", "??4FText@@QEAAAEAV0@$$QEAV0@@Z"));
#else
Function<void(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName)>                                AddBrickProperty("48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 48 8D 59");
Function<FBrickPropertyEditInfo*(FBrickPropertyReflection*, TSharedRef<FBrickProperty>* InBrickProperty, SDK::FString* InFullPropertyName, SDK::FText* InDisplayName)>     AddBrickPropertyDisplayInfo("48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC 50 49 8B D9");
Function<TSharedRef<FBoolBrickProperty>* (TSharedRef<FBoolBrickProperty>*)>                                                                             ConstructBrickProperty_Bool("E8 ?? ?? ?? ?? 48 8B 8D 38 07 00 00");
Function<SDK::FProperty* (SDK::UStruct*, SDK::FName)>																									FindPropertyByName("48 8B 41 70 48 85 C0 74 16");
Function<SDK::FText* (SDK::FText* This, SDK::FText* That)>																								FTextCopyOperator("48 89 5C 24 18 48 89 74 24 20 41 56 48 83 EC 40 48 8B 5A");
#endif


void DeclareBooleanProperty(SDK::UClass* ObjClass, SDK::FBP_FBrickPropertyDeclaration Declaration, FBrickPropertyReflection* Reflection)
{
	TSharedRef<FBoolBrickProperty> PropertyReference;
	std::cout << (uintptr_t)ConstructBrickProperty_Bool.GetPtr() - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") + 0x180000000 << std::endl;
	std::cout << (uintptr_t)AddBrickProperty.GetPtr() - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") + 0x180000000 << std::endl;
	std::cout << (uintptr_t)AddBrickPropertyDisplayInfo.GetPtr() - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") + 0x180000000 << std::endl;
	ConstructBrickProperty_Bool(&PropertyReference);

	SDK::FString FullPropertyName = SDK::UKismetTextLibrary::Conv_TextToString(Declaration.PropertyName_6_90CFF1AA403BE3727D57088D9A0E8480);
	std::cout << FullPropertyName.ToString() << std::endl;

	PropertyReference.Object->PropertyName = SDK::UKismetStringLibrary::Conv_StringToName(FullPropertyName);
	PropertyReference.Object->Property = FindPropertyByName(ObjClass, PropertyReference.Object->PropertyName);

	std::cout << "PROP: " << PropertyReference.Object->Property << std::endl;

	FBrickPropertyInstance Instance = FBrickPropertyInstance{
		.BrickProperty = PropertyReference,
		.FullPropertyName = FullPropertyName,
		.ParentPropertyChain = SDK::TArray<SDK::FStructProperty>()
	};
	std::cout << "SER: " << Reflection->bIsSerializing << std::endl;
	if (Reflection->bIsSerializing)
	{
		//std::cout << Reflection->BrickProperties.Max() - Reflection->BrickProperties.Num() << std::endl;
		//Reflection->BrickProperties.Add(Instance);
		AddBrickProperty(Reflection, &PropertyReference, &Instance.FullPropertyName);
	}
	else
	{
		auto EditInfo = AddBrickPropertyDisplayInfo(Reflection, &PropertyReference, &FullPropertyName, &Declaration.DisplayName_7_E5FF33B84665E94A6EF8C49F03E1C2C5);
	
		EditInfo->DisplayName = Declaration.DisplayName_7_E5FF33B84665E94A6EF8C49F03E1C2C5;
		//EditInfo->DescriptionText = Declaration.Description_10_FEF8680340F9F4222764FE876625C467;
		EditInfo->bIsEnabled = Declaration.bIsEnabled_12_C37DF3214F1B585DB23291A83D02D5FE;
		EditInfo->bIsReadOnly = Declaration.bIsReadOnly_14_36BD3D53474F4FE5A355B294996AD4E5;
		EditInfo->ColorStyle = Declaration.ColorStyle_17_E6E6524E41F70E650422F49B6491187E;
		//EditInfo->MaxComboBoxItemsPerRow = 0;
		//EditInfo->MaxComboBoxListItems = 0;
	}
}

void ReflectBrickPropertiesOverride(IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection)
{
	std::cout << "Custom function!. Calling original now: " << GetEditorObject(This) << std::endl;
	auto EditorObject = GetEditorObject(This);
	
	auto BPInterface = reinterpret_cast<SDK::IBP_IBrickPropertyInterface_C*>(EditorObject);
	SDK::FBP_ShouldCallSuperSettings ShouldCallSuperSettings;
	BPInterface->ShouldCallSuperReflectBrickProperties(&ShouldCallSuperSettings);
	const bool CallSuper = ShouldCallSuperSettings.ShouldCallSuper_1_3CC922DD4B3C9B04AF8CC79AF56C037B;
	const bool CallAtStart = ShouldCallSuperSettings.CallAtStart_3_2B65F40F4734A4579FD5929B66219043;
	
	auto SuperFunction = [](IBrickPropertyInterface* This, FBrickPropertyReflection* Reflection) -> void
	{
		auto Brick = GetEditorObject(This);
		auto Interface = InterfaceRegistry.find(Brick->Class->Name.ToString());
		if (Interface != InterfaceRegistry.end())
		{
			Interface->second->ReflectBrickProperties(This, Reflection);
		}
	};

	if (CallSuper && CallAtStart)
	{
		SuperFunction(This, Reflection);
	}

	//Actually Reflect Properties
	auto Declarations = SDK::TArray<SDK::FBP_FBrickPropertyDeclaration>();
	BPInterface->ReflectBrickProperties(&Declarations);

	for (const auto &Declare : Declarations)
	{
		switch (Declare.Type_33_5AFD4DE54866742EFCB3248308311FFE)
		{
		case SDK::EBP_BrickPropertyType::Bool:
			DeclareBooleanProperty(EditorObject->Class, Declare, Reflection);
			break;
		default:
			break;
		}
	}

	if (CallSuper && !CallAtStart)
	{
		SuperFunction(This, Reflection);
	}
}