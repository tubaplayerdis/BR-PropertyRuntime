#include "IBrickPropertyInterface.hpp"
#include <algorithm>
#include <iostream>

template<typename T>
T RecoverVirtualTable(std::string signature)
{
#ifdef BRMK_SDK
	unsigned long long VirtualTableString = 0;//Signature::InternalResolveSignature(signature, Signature::SearchContext::RDATA, "BrickRigsModKitSteam-BrickRigs.dll");
#else
	unsigned long long VirtualTableString = Signature::InternalResolveSignature(signature, Signature::SearchContext::RDATA);
#endif // BRMK_SDK

	if (!VirtualTableString)
		std::cout << "Failed to recover virtual table!" << std::endl;

	std::cout << "Vtable: " << VirtualTableString << std::endl;

	// The vtable's last entry ends exactly where the string begins.
	// Since T's layout matches the real vtable slot-for-slot, sizeof(T)
	// is exactly the byte span of the vtable.
	uintptr_t VTableStart = static_cast<uintptr_t>(VirtualTableString) - sizeof(T);

	return *reinterpret_cast<T*>(VTableStart);
}

FNumericBrickPropertyValue SanitizeValue(FNumericBrickPropertyBase* Property, const FNumericBrickPropertyValue* NewValue)
{
	const auto Range = Property->ValueRange.Value.byte;
	auto OutValue = *NewValue;
	const auto ActualAxisLock = Property->AxisLock.Value.byte;
	for (auto i = 0; i < OutValue.NumUsed; ++i)
	{
		// Remap the index according to the axis lock
		const auto ValueIndex = SDK::UFluMathStatics::RemapAxisLockIndex(ActualAxisLock, i);
		OutValue.Set(i, std::clamp(OutValue.Get(ValueIndex), Range.Min.GetOrFirst(ValueIndex), Range.Max.GetOrFirst(ValueIndex)));
	}

	return OutValue;
}

template<typename FNumericType>
struct FNumericBrickProperty_vtbl : FNumericBrickPropertyBase_vtbl
{
	static bool _ComparePropertyValues(FNumericBrickPropertyBase* This, const void* A, const void* B)
	{
		return This->CompareInternal<FNumericType>(A, B);
	}

	static bool _SetValue(FNumericBrickPropertyBase* This, const FBrickPropertyContainer& Container, const FNumericBrickPropertyValue* NewValue)
	{
		const auto ActualValue = SanitizeValue(This, NewValue);
		return This->SetValueInternal<FNumericType>(Container, ActualValue);
	}

	static bool _GetValue(FNumericBrickPropertyBase* This, const FBrickPropertyContainer& Container, FNumericBrickPropertyValue& OutValue)
	{
		FNumericType ActualValue;
		if (This->GetValueInternal<FNumericType>(Container, ActualValue))
		{
			OutValue = ActualValue;
			return true;
		}
		return false;
	}

	FNumericBrickProperty_vtbl()
		: FNumericBrickPropertyBase_vtbl(RecoverVirtualTable<FNumericBrickPropertyBase_vtbl>("46 4E 75 6D 65 72 69 63 42 72 69 63 6B 50 72 6F"))
	{
		SetValue = _SetValue;
		GetValue = _GetValue;
		//ComparePropertyValues = reinterpret_cast<bool(__fastcall * ComparePropertyValues)(FBrickProperty * This, void*, void*)>(_ComparePropertyValues);
	}
};

#define DECLARE_VTABLE_NUMERIC(type) static auto type##__FNumericBrickPropertyVTable = FNumericBrickProperty_vtbl<type>()

using namespace SDK;
//DECLARE_VTABLE_NUMERIC(float);
//DECLARE_VTABLE_NUMERIC(int32_t);
//DECLARE_VTABLE_NUMERIC(uint32_t);
//DECLARE_VTABLE_NUMERIC(uint16_t);
//DECLARE_VTABLE_NUMERIC(uint8_t);
//DECLARE_VTABLE_NUMERIC(FVector2D);
//DECLARE_VTABLE_NUMERIC(FVector);
//DECLARE_VTABLE_NUMERIC(FRotator);



/*
* types:
* float
* int32
* uint32
* uint16
* uint8
* FVector2D
* FVector
* FRotator
*/

void FixVirutalTable(NumericPropertyTypes ValueType, FNumericBrickPropertyBase* Base)
{
	return;
	/*
	switch (ValueType)
	{
	case FLOAT_:
		Base->VTable = &float__FNumericBrickPropertyVTable;
		break;
	case INT32_:
		Base->VTable = &int32_t__FNumericBrickPropertyVTable;
		break;
	case UINT32_:
		Base->VTable = &uint32_t__FNumericBrickPropertyVTable;
		break;
	case UINT16_:
		Base->VTable = &uint16_t__FNumericBrickPropertyVTable;
		break;
	case UINT8_:
		Base->VTable = &uint8_t__FNumericBrickPropertyVTable;
		break;
	case FVECTOR2D:
		Base->VTable = &FVector2D__FNumericBrickPropertyVTable;
		break;
	case FVECTOR:
		Base->VTable = &FVector__FNumericBrickPropertyVTable;
		break;
	case FROTATOR:
		Base->VTable = &FRotator__FNumericBrickPropertyVTable;
		break;
	default:
		Base->VTable = &float__FNumericBrickPropertyVTable;
		break;
	}
	*/
}