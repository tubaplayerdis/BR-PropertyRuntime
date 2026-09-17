#pragma once
#include <BR-SDK.hpp>
#include "RefrenceController.hpp"

template<typename T>
struct TFSharedRef
{
    T* Object;
    UC::int8 SharedReferenceCount[0x8];
};

template<typename T>
struct TOptional
{
    T byte;
    bool isSet;
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

    void* GetContainer() const
    {
        return ContainerChain.Num() ? ContainerChain[ContainerChain.Num()-1] : RootObject;
    }
};

const struct FBrickEditorReferenceResolver
{
    SDK::TArray<SDK::UObject*> Objects;
};

struct FBrickProperty;
struct FBrickProperty_vtbl //Search for string FNumericBrickProperty to get the FNumericBrickPropertyBase vtable. subclasses only should implement Get/Set value which can be done on our side.
{
    SDK::FName* (__fastcall* GetTypeName)(FBrickProperty* This, SDK::FName* result);
    SDK::FName* (__fastcall* GetValueTypeName)(FBrickProperty* This, SDK::FName* result);
    bool(__fastcall* IsOfTypeInternal)(FBrickProperty* This, const SDK::FName*);
    void(__fastcall* GetTypeHierarchyInternal)(FBrickProperty* This, SDK::TArray<SDK::FName>*);
    void(__fastcall* Destructor_FBrickProperty)(FBrickProperty* This);
    void(__fastcall* GetTypeHierarchy)(FBrickProperty* This, SDK::TArray<SDK::FName>*);
    bool(__fastcall* IsPropertyNull)(FBrickProperty* This);//Name guessed from the behavior of the function
    bool(__fastcall* ComparePropertyValues)(FBrickProperty* This, void*, void*);
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

    template <typename ValueType>
    ValueType* GetValuePtr(const FBrickPropertyContainer& Container)
    {
        //check(Property && Container.IsValid());
        return Property->ContainerPtrToValuePtr<ValueType>(Container.GetContainer());
    }

    template <typename ValueType>
    bool SetValueInternal(const FBrickPropertyContainer& Container, const ValueType& NewValue)
    {
        // Set the actual value
        ValueType* ValuePtr = GetValuePtr<ValueType>(Container);
        if (ValuePtr)
        {
            *ValuePtr = NewValue;
            return true;
        }

        return false;
    }

    template <typename ValueType>
    bool GetValueInternal(const FBrickPropertyContainer& Container, ValueType& OutValue)
    {
        const ValueType* ValuePtr = GetValuePtr<ValueType>(Container);
        if (ValuePtr)
        {
            OutValue = *ValuePtr;
            return true;
        }

        return false;
    }

    template <typename T>
    static bool CompareInternal(const void* A, const void* B)
    {
        return *static_cast<const T*>(A) == *static_cast<const T*>(B);
    }
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

enum NumericPropertyTypes
{
    FLOAT_,
    INT32_,
    UINT32_,
    UINT16_,
    UINT8_,
    FVECTOR2D,
    FVECTOR,
    FROTATOR
};

/* 199928 */
struct __declspec(align(4)) FNumericBrickPropertyValue
{
    SDK::FVector Data;
    unsigned __int8 NumUsed;

    FNumericBrickPropertyValue() = default;

    // ~Constructor
    FNumericBrickPropertyValue(float V)
        : Data(V, 0.f, 0.f), NumUsed(1)
    {}

    // ~Constructor
    FNumericBrickPropertyValue(SDK::FVector2D V)
        : Data(V.X, V.Y, 0.f), NumUsed(2)
    {}

    // ~Constructor
    FNumericBrickPropertyValue(SDK::FVector V)
        : Data(V), NumUsed(3)
    {}

    // ~Constructor
    FNumericBrickPropertyValue(SDK::FRotator R)
        : Data(R.Roll, R.Pitch, R.Yaw), NumUsed(3)
    {}

    operator float() const
    {
        return Data.X;
    }

    operator SDK::FVector2D() const
    {
        return { Data.X, Data.Y };
    }

    operator SDK::FVector() const
    {
        return Data;
    }

    operator SDK::FRotator() const
    {
        return SDK::FRotator(Data.Y, Data.Z, Data.X);
    }

    auto Get(const int Index) const
    {
        switch (Index)
        {
        case 0:
            return Data.X;
        case 1:
            return Data.Y;
        case 2:
            return Data.Z;
        default:
            return Data.X;
            break;
        }
    }

    auto GetOr(const int Index, const float Fallback) const
    {
        return Index < NumUsed ? Get(Index) : Fallback;
    }

    auto GetOrFirst(const int Index) const
    {
#undef min
        return Get(std::min(Index, NumUsed - 1));
    }

    void Set(const int Index, const float Value)
    {
#undef max
        NumUsed = std::max(static_cast<int>(NumUsed), Index + 1);
        switch (Index)
        {
        case 0:
            Data.X = Value;
            break;
        case 1:
            Data.Y = Value;
            break;
        case 2:
            Data.Z = Value;
            break;
        default:
            Data.X = Value;
            break;
        }
    }
};

/* 199929 */
struct FNumericBrickPropertyRange
{
    FNumericBrickPropertyValue Min;
    FNumericBrickPropertyValue Max;
};

template<typename T>
struct TBrickPropAttribute
{
    TOptional<T> Value;
    SDK::TDelegate<T(FBrickPropertyContainer)> Delegate;
};
//static_assert(sizeof(TBrickPropAttribute<SDK::EFluAxisLock>) == 0x18);

struct FNumericBrickPropertyBase;
void FixVirutalTable(NumericPropertyTypes ValueType, FNumericBrickPropertyBase* Base);

struct FNumericBrickPropertyBase : FBrickProperty
{
    TBrickPropAttribute<enum SDK::ENumericValueType> ValueType;
    TBrickPropAttribute<FNumericBrickPropertyRange> ValueRange;
    TBrickPropAttribute<enum SDK::EFluAxisLock> AxisLock;

    static unsigned char GetNumUsed(NumericPropertyTypes Types)
    {
        switch (Types)
        {
        case FLOAT_:
            return 1;
        case INT32_:
            return 1;
        case UINT32_:
            return 1;
        case UINT16_:
            return 1;
        case UINT8_:
            return 1;
        case FVECTOR2D:
            return 2;
        case FVECTOR:
            return 3;
        case FROTATOR:
            return 3;
        default:
            return 1;
        }
    }

    FNumericBrickPropertyBase(SDK::ENumericValueType TypeDisplay, NumericPropertyTypes valueType, SDK::FVector Min, SDK::FVector Max, SDK::EFluAxisLock axisLock) : FBrickProperty()
    {
        const auto NumUsed = GetNumUsed(valueType);
        AxisLock.Value.byte = axisLock;
        AxisLock.Value.isSet = true;
        ValueRange.Value.byte = FNumericBrickPropertyRange{ .Min = FNumericBrickPropertyValue(Min), .Max = FNumericBrickPropertyValue(Max) };
        ValueRange.Value.isSet = true;
        ValueType.Value.byte = TypeDisplay;
        ValueType.Value.isSet = true;
        FixVirutalTable(valueType, this);
    }
};

struct FNumericBrickPropertyBase_vtbl : FBrickProperty_vtbl
{
    bool(__fastcall* GetValue)(FNumericBrickPropertyBase* This, const FBrickPropertyContainer&, FNumericBrickPropertyValue&);
    bool(__fastcall* SetValue)(FNumericBrickPropertyBase* This, const FBrickPropertyContainer&, const FNumericBrickPropertyValue*);
    FNumericBrickPropertyRange* (__fastcall* GetValueRange)(FNumericBrickPropertyBase* This, FNumericBrickPropertyRange* result, const FBrickPropertyContainer*);
};

struct FBrickPropertyCategory
{
    SDK::FText DisplayName;
};

struct FBrickPropertyInstance
{
    TFSharedRef<FBrickProperty> BrickProperty;
    SDK::FString FullPropertyName;
    SDK::TArray<SDK::FStructProperty> ParentPropertyChain;
};

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
    SDK::FText DisplayName;              // 0x40, size 0x18
    SDK::uint64 Unknown1;                // 0x58
    void* Unknown2;                      // 0x60
    SDK::FText DescriptionText;          // 0x68, size 0x18
    SDK::TArray<SDK::TWeakObjectPtr<SDK::UObject>> ContainerObjects; // 0x80, size 0x10
    SDK::FWeakObjectPtr Unknown5;        // 0x90
    bool bIsEnabled;                     // 0x98
    bool bIsReadOnly;                    // 0x99  <- CONFIRMED
    SDK::EBrickUIColorStyle ColorStyle;
    SDK::uint8 pad_0[1];
    int MaxComboBoxListItems;
    int MaxComboBoxItemsPerRow;
    SDK::uint8 pad_1[4];
    TSharedPtr<FBrickPropertyChangedEvent> PendingChangedEvent;
    SDK::TOptional<std::byte> OrientationOverride;
    SDK::uint8 pad_2[6];
};
//static_assert(sizeof(FBrickPropertyEditInfo) == 0xA8);

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
    void(__fastcall * Deconstructor_IBrickPropertyInterface)(IBrickPropertyInterface* This);
    SDK::UObject* (__fastcall* _getUObject)(IBrickPropertyInterface* This);
    void(__fastcall* ReflectBrickProperties)(IBrickPropertyInterface* This, FBrickPropertyReflection*);
    bool(__fastcall* CanModifyBrickProperty)(IBrickPropertyInterface* This);
    void(__fastcall* PostModifyBrickProperty)(IBrickPropertyInterface* This, const FBrickPropertyChangedEvent*);
    void(__fastcall* UpdateFocusedBrickProperty)(IBrickPropertyInterface* This, const FBrickPropertyFocusEvent*);
};

