#include "PropertyInterfaceOverrides.hpp"

//FNumericBrickProperty 46 4E 75 6D 65 72 69 63 42 72 69 63 6B 50 72 6F

template<typename T>
T RecoverVirtualTable(std::string signature)
{
#ifdef BRMK_SDK
	unsigned long long VirtualTableString = Signature::InternalResolveSignature(signature, Signature::SearchContext::RDATA, "BrickRigsModKitSteam-BrickRigs.dll")
#else
	unsigned long long VirtualTableString = Signature::InternalResolveSignature(signature, Signature::SearchContext::RDATA)
#endif // BRMK_SDK

    if (!VirtualTableString)
        std::cout << "Failed to recover virtual table!"
        return nullptr;

    // The vtable's last entry ends exactly where the string begins.
    // Since T's layout matches the real vtable slot-for-slot, sizeof(T)
    // is exactly the byte span of the vtable.
    uintptr_t VTableStart = static_cast<uintptr_t>(VirtualTableString) - sizeof(T);

    return *reinterpret_cast<T*>(VTableStart);
}