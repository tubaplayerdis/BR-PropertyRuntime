#include "IBrickPropertyInterface.hpp"
#include "BP_IBrickPropertyInterface_classes.hpp"
#include "PropertyInterfaceOverrides.hpp"
#include <algorithm>


FNumericBrickPropertyValue SanitizeValue(const FNumericBrickPropertyBase& Property, const FNumericBrickPropertyValue& NewValue)
{
	const auto Range = Property.ValueRange.Value.byte;
	auto OutValue = NewValue;
	const auto ActualAxisLock = Property.AxisLock.Value.byte;
	for (auto i = 0; i < OutValue.NumUsed; ++i)
	{
		// Remap the index according to the axis lock
		const auto ValueIndex = SDK::UFluMathStatics::RemapAxisLockIndex(ActualAxisLock, i);
		OutValue.Set(i, std::clamp(OutValue.Get(ValueIndex), Range.Min.GetOrFirst(ValueIndex), Range.Max.GetOrFirst(ValueIndex)));
	}

	return OutValue;
}

template <typename FNumericType>
bool ComparePropertyValues(FBrickProperty* This, const void* A, const void* B)
{
	return This->CompareInternal<FNumericType>(A, B);
}
template <typename FNumericType>
bool SetValue(FBrickProperty* This, const FBrickPropertyContainer& Container, const FNumericBrickPropertyValue& NewValue)
{
	const auto ActualValue = This->SanitizeValue(Container, NewValue);
	return This->SetValueInternal<FNumericType>(Container, ActualValue);
}
template <typename FNumericType>
bool GetValue(FBrickProperty* This, , const FBrickPropertyContainer& Container, FNumericBrickPropertyValue& OutValue)
{
	FNumericType ActualValue;
	if (This->GetValueInternal<FNumericType>(Container, ActualValue))
	{
		OutValue = ActualValue;
		return true;
	}

	return false;
}


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

static unsigned char GetNumUsed(NumericPropertyTypes Types)
{
	switch (Types)
	{
	case FLOAT:
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
 

static FNumericBrickPropertyBase_vtbl OriginalVTable = RecoverVirtualTable<FNumericBrickPropertyBase_vtbl>("46 4E 75 6D 65 72 69 63 42 72 69 63 6B 50 72 6F");
static FNumericBrickPropertyBase_vtbl FloatVTable = OriginalVTable;


FNumericBrickPropertyBase ConstructNumericProperty(SDK::ENumericValueType TypeDisplay, NumericPropertyTypes ValueType, SDK::FVector Min, SDK::FVector Max, SDK::EFluAxisLock AxisLock)
{
	const auto NumUsed = GetNumUsed(ValueType);

	FNumericBrickPropertyBase Base{};
	FNumericBrickPropertyBase_vtbl Replacement = OrginalVTable;

	Replacement.Destructor_FBrickProperty = 


	Base.AxisLock.Value.byte = AxisLock;
	Base.AxisLock.Value.isSet = true;
	Base.ValueRange.Value.byte = FNumericBrickPropertyRange{ .Min = {.Data = Min, .NumUsed = NumUsed }, .Max = {.Data = Max, .NumUsed = NumUsed} };
	Base.ValueRange.Value.isSet = true;
	Base.ValueType.Value.byte = TypeDisplay;
	Base.ValueType.Value.isSet = true;

	switch (ValueType)
	{
	case FLOAT:

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