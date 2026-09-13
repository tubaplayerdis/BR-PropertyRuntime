#pragma once
#include <BR-SDK.hpp>

template<typename T>
struct TSharedRef
{
    T* Object;
    UC::int8 SharedReferenceCount[0x8];
};

template<typename T>
struct TSharedPtr
{
    T* Object;
    UC::int8 SharedReferenceCount[0x8];
};

template<typename T>
struct TWeakPtr
{
    T* Object;
    UC::int8 WeakReferenceCount[0x8];
};

template<typename T>
struct TSharedFromThis
{
    TWeakPtr<T> WeakThis;
};

const struct FBrickPropertyContainer
{
    SDK::UObject* RootObject;
    SDK::TArray<void*> ContainerChain;
};

const struct FBrickEditorReferenceResolver
{
    SDK::TArray<SDK::UObject*> Objects;
};

struct FBrickProperty;
struct FBrickProperty_vtbl
{
    SDK::FName* (__fastcall* GetTypeName)(FBrickProperty* This, SDK::FName* result);
    SDK::FName* (__fastcall* GetValueTypeName)(FBrickProperty* This, SDK::FName* result);
    bool(__fastcall* IsOfTypeInternal)(FBrickProperty* This, const SDK::FName*);
    void(__fastcall* GetTypeHierarchyInternal)(FBrickProperty* This, SDK::TArray<SDK::FName>*);
    void(__fastcall* Destructor_FBrickProperty)(FBrickProperty* This);
    void(__fastcall* GetTypeHierarchy)(FBrickProperty* This, SDK::TArray<SDK::FName>*);
    bool(__fastcall* ComparePropertyValues)(FBrickProperty* This, const void*, const void*);
    bool(__fastcall* IsPropertyNull)(FBrickProperty* This);//Name guessed from the behavior of the function
    bool(__fastcall* FluppuSpecialSauce1)(FBrickProperty* This, void*, void*);
    bool(__fastcall* SerializeProperty)(FBrickProperty* This, void* FArchive_Ptr, const FBrickPropertyContainer* Container, UC::int8 Version, const FBrickEditorReferenceResolver*);
    bool(__fastcall* DoesObjectContainPropertyInternal)(FBrickProperty* This, const SDK::UObject*);
    bool(__fastcall* GetValueAsText)(FBrickProperty* This, const FBrickPropertyContainer*, SDK::FText*);
    bool(__fastcall* SetValueAsText)(FBrickProperty* This, const FBrickPropertyContainer*, const SDK::FText*);
    bool(__fastcall* IsUserText)(FBrickProperty* This);
    SDK::FString* (__fastcall* ExportProperty)(FBrickProperty* This, SDK::FString* result, const FBrickPropertyContainer*);
    bool(__fastcall* CanExportProperty)(FBrickProperty* This, const FBrickPropertyContainer*);
    bool(__fastcall* ImportProperty)(FBrickProperty* This, const FBrickPropertyContainer*, const wchar_t*);
    bool(__fastcall* CanImportProperty)(FBrickProperty* This, const FBrickPropertyContainer*, const wchar_t*);
};

struct FBrickProperty
{
    FBrickProperty_vtbl* VTable;
    SDK::FProperty* Property;
    SDK::FName PropertyName;
};

struct __declspec(align(2)) FTextBrickProperty : FBrickProperty
{
    const int MaxTextLength;
    const bool bIsPassword;
    const bool bAllowMultiLine;
    const bool bIsUserText;
};
//static_assert(sizeof(FTextBrickProperty) == 0x20);

typedef FBrickProperty FBoolBrickProperty;//Struct of same size in dissasembly

struct FBrickPropertyCategory
{
    SDK::FText DisplayName;
};

struct FBrickPropertyInstance
{
    TSharedRef<FBrickProperty> BrickProperty;
    SDK::FString FullPropertyName;
    SDK::TArray<SDK::FStructProperty> ParentPropertyChain;
};
static_assert(sizeof(FBrickPropertyInstance) == 0x30);

const struct __declspec(align(8)) FBrickPropertyChangedEvent
{
    SDK::TWeakObjectPtr<SDK::ABasePlayerController> Player;
    SDK::FString FullPropertyName;
    SDK::TArray<SDK::FName> PropertyChain;
    int PropertyChainDepth;
    SDK::TArray<SDK::TWeakObjectPtr<SDK::UObject>> Objects;
    SDK::TWeakObjectPtr<SDK::UObject> ActiveObject;
    SDK::EValueChangedEventType EventType[1];
    bool bExternalChange;
    bool bUpdateAllProperties;
};

const struct __declspec(align(8)) FBrickPropertyEditInfo : FBrickPropertyInstance, TSharedFromThis<FBrickPropertyEditInfo>
{
    SDK::FText DisplayName;
    SDK::FText DescriptionText;
    SDK::TArray<SDK::TWeakObjectPtr<SDK::UObject>> ContainerObjects;
    bool bIsEnabled;
    bool bIsReadOnly;
    SDK::EBrickUIColorStyle ColorStyle;
    SDK::uint8 pad_0[1];
    int MaxComboBoxListItems;
    int MaxComboBoxItemsPerRow;
    SDK::uint8 pad_1[4];
    TSharedPtr<FBrickPropertyChangedEvent> PendingChangedEvent;
    SDK::TOptional<std::byte> OrientationOverride;
    SDK::uint8 pad_2[6];
};
static_assert(sizeof(FBrickPropertyEditInfo) == 0xA8);

struct FBrickPropertyReflection
{
    bool bIsSerializing;
    SDK::uint8 pad_0[7];
    SDK::TArray<SDK::TWeakObjectPtr<SDK::UObject>> ContainerObjects;
    SDK::FBrickPropertyReflectionFilter Filter;
    void* SomethingFluppiAdded;
    SDK::TArray<FBrickPropertyInstance> BrickProperties;
    SDK::TArray<SDK::TPair<TSharedRef<FBrickPropertyEditInfo>, int>> BrickPropertyEditInfos;
    SDK::TArray<FBrickPropertyCategory> Categories;
    int CurrentCategoryIndex;
    SDK::uint8 pad_1[4];
    SDK::TArray<SDK::FStructProperty*> ParentPropertyChain;
};
static_assert(sizeof(FBrickPropertyReflection) == 0x88);
static_assert(offsetof(FBrickPropertyReflection, BrickPropertyEditInfos) == 0x50);

enum class EBrickValidityState : UC::uint8
{
    None,
    Neutral,
    Invalid,
    Valid
};

const struct FBrickPropertyFocusEvent : FBrickPropertyInstance
{
    const SDK::FName SubPropertyName;
    const bool bIsFocused;
    SDK::TMap<SDK::UBrickEditorObject*, enum EBrickValidityState> ObjectsToHighlight;
};

/* 200980 */
struct /*VFT*/ IBrickPropertyInterface
{
    void(__fastcall * Deconstructor_IBrickPropertyInterface)(SDK::UBrickEditorObject* This);
    SDK::UObject* (__fastcall* _getUObject)(SDK::UBrickEditorObject* This);
    void(__fastcall* ReflectBrickProperties)(SDK::UBrickEditorObject* This, FBrickPropertyReflection*);
    bool(__fastcall* CanModifyBrickProperty)(SDK::UBrickEditorObject* This);
    void(__fastcall* PostModifyBrickProperty)(SDK::UBrickEditorObject* This, const FBrickPropertyChangedEvent*);
    void(__fastcall* UpdateFocusedBrickProperty)(SDK::UBrickEditorObject* This, const FBrickPropertyFocusEvent*);
};

