#include "Hooks.hpp"
#include "IBrickPropertyInterface.hpp"
#include "PropertyInterfaceOverrides.hpp"
#include <BR-SDK.hpp>
#include "Extension.hpp"
#include <string>
#include <map>

std::map<std::string, IBrickPropertyInterface*> InterfaceRegistry = std::map<std::string, IBrickPropertyInterface*>();

bool Hooks::IsEditorBinary()
{
	static const bool DetectedEditorBinary = GetModuleHandle(L"BrickRigsModKitSteam.exe") != nullptr;
	return DetectedEditorBinary;
}

uintptr_t Hooks::GetSymbolAddress(const char* Module, const char* Symbol)
{
	HMODULE hModuleKit = GetModuleHandleA(Module); // or whatever the module's actually called
	return hModuleKit != 0 ? (uintptr_t)GetProcAddress(hModuleKit, Symbol) : 0;
}

uintptr_t GetInitializeBrickEditorObjectPointer()
{
	constexpr char INITIALIZE_EDITOR_OBJECT_SIG[] = "4C 8B DC 55 53 57 41 54 49 8D 6B";
	constexpr char EDITOR_INITIALIZE_EDITOR_OBJECT_MODULE[] = "BrickRigsModKitSteam-BrickRigs.dll";
	constexpr char EDITOR_INITIALIZE_EDITOR_OBJECT_SYB[] = "?InitializeBrickEditorObject@UBrickEditorObject@@AEAAXPEAVUClass@@AEBUFBrickEditorObjectID@@W4EBrickEditorObjectContext@@AEBEAEBUFLegacyBrickEditorObjectClassID@@_N5@Z";
	if constexpr (EditorSDK)
	{
		return Hooks::GetSymbolAddress(EDITOR_INITIALIZE_EDITOR_OBJECT_MODULE, EDITOR_INITIALIZE_EDITOR_OBJECT_SYB);
	}
	else return Signature(INITIALIZE_EDITOR_OBJECT_SIG).GetPtr();
}

uintptr_t GetImplementsInterfacePointer()
{
	constexpr char IMPLEMNETS_INTERFACE_OBJECT_SIG[] = "48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9 48 85 D2 74 63";
	constexpr char EDITOR_IMPLEMNETS_INTERFACE_OBJECT_MODULE[] = "BrickRigsModKitSteam-CoreUObject.dll";
	constexpr char EDITOR_IMPLEMNETS_INTERFACE_OBJECT_SYB[] = "?ImplementsInterface@UClass@@QEBA_NPEBV1@@Z";
	if constexpr (EditorSDK)
	{
		return Hooks::GetSymbolAddress(EDITOR_IMPLEMNETS_INTERFACE_OBJECT_MODULE, EDITOR_IMPLEMNETS_INTERFACE_OBJECT_SYB);
	}
	else return Signature(IMPLEMNETS_INTERFACE_OBJECT_SIG).GetPtr();
}

enum class EBrickEditorObjectContext : UC::uint8
{
	None,
	// None means spawned, not in editor
	Editor,
	ThumbnailRender
};

struct FLegacyBrickEditorObjectClassID
{
	static constexpr UC::uint8 MAX_uint8 = 255;
	// ~Constructor
	FLegacyBrickEditorObjectClassID(const UC::uint8 CategoryIndex = MAX_uint8, const UC::uint8 ClassIndex = MAX_uint8)
		: CategoryIndex(CategoryIndex), ClassIndex(ClassIndex)
	{}

	bool IsValid() const
	{
		return CategoryIndex != MAX_uint8 && ClassIndex != MAX_uint8;
	}

private:
	UC::uint8 CategoryIndex;
	UC::uint8 ClassIndex;

	//friend class UBrickDataSingleton;
};

Function<bool(SDK::UClass*, SDK::UClass*)> ImplementsInterface(GetImplementsInterfacePointer());

#ifdef BRMK_SDK
IBrickPropertyInterface OverrideBrickPropertyInterface =
{
	.Deconstructor_IBrickPropertyInterface = [](IBrickPropertyInterface* This) -> void
	{
		std::cout << "I am supposed to call a destructor: " << This << std::endl;
	},
	._getUObject = (SDK::UObject * (__fastcall*)(IBrickPropertyInterface*))Hooks::GetSymbolAddress("BrickRigsModKitSteam-BrickRigs.dll", "?_getUObject@UBrickEditorObject@@UEBAPEAVUObject@@XZ"),
	.ReflectBrickProperties = ReflectBrickPropertiesOverride,
	.CanModifyBrickProperty = CanModifyBrickPropertyOverride,
	.PostModifyBrickProperty = PostModifyBrickPropertyOverride,
	.UpdateFocusedBrickProperty = UpdateFocusedBrickPropertyOverride
};
#else
IBrickPropertyInterface OverrideBrickPropertyInterface =
{

	.ReflectBrickProperties = ReflectBrickPropertiesOverride
};
#endif


Hook<void(SDK::UBrickEditorObject* This, SDK::UClass* InStaticInfoClass, SDK::FBrickEditorObjectID* InObjectID, EBrickEditorObjectContext InEditorContext, SDK::uint8 Version, FLegacyBrickEditorObjectClassID* LegacyClassID, bool bInHasBeenRecycled, bool bInCreatedAsMirrored)> InitializeBrickEditorObjectHook(GetInitializeBrickEditorObjectPointer(),
[](SDK::UBrickEditorObject* This, SDK::UClass* InStaticInfoClass, SDK::FBrickEditorObjectID* InObjectID, EBrickEditorObjectContext InEditorContext, SDK::uint8 Version, FLegacyBrickEditorObjectClassID* LegacyClassID, bool bInHasBeenRecycled, bool bInCreatedAsMirrored) -> void
{		
	if (This->Class && ImplementsInterface(This->Class, SDK::IBP_IBrickPropertyInterface_C::StaticClass()))
	{
		auto VTable = GetMember<IBrickPropertyInterface*>(This, sizeof(SDK::UObject));
		std::cout << "Implemented Class Found! " << This << std::endl;

		std::string ClassName = This->Class->Name.ToString();
		std::cout << ClassName << std::endl;
		if (!InterfaceRegistry.contains(ClassName))
		{
			std::cout << "Registering!" << std::endl;
			InterfaceRegistry.insert(std::make_pair(ClassName, VTable));
		}
		std::cout << "Patching!" << std::endl;

		std::cout << (uintptr_t)VTable->Deconstructor_IBrickPropertyInterface - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") << std::endl;
		std::cout << (uintptr_t)VTable->_getUObject - (uintptr_t)GetModuleHandleW(L"BrickRigsModKitSteam-BrickRigs.dll") << std::endl;
		SetMember<IBrickPropertyInterface*>(This, sizeof(SDK::UObject), &OverrideBrickPropertyInterface);
	}
	InitializeBrickEditorObjectHook.CallOriginal(This, InStaticInfoClass, InObjectID, InEditorContext, Version, LegacyClassID, bInHasBeenRecycled, bInCreatedAsMirrored);
	//Offset of the IBrickPropertyInterfacePointer is 0x28 in release and 0x30 in editor.
});

void Hooks::SetupCreateObjectHook()
{
	InitializeBrickEditorObjectHook.Create();
	InitializeBrickEditorObjectHook.Enable();
}