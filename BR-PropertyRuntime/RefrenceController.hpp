#pragma once

class FReferenceControllerBase
{
public:
    int SharedReferenceCount;
    int WeakReferenceCount;

    FReferenceControllerBase() : SharedReferenceCount(1), WeakReferenceCount(1) {}

    virtual void DestroyObject() = 0;
    virtual ~FReferenceControllerBase() = default;
};

// Concrete controller: owns the actual T instance inline (matches the engine's
// "intrusive"/combined-allocation style you've seen in the disassembly all
// through this conversation — one allocation holding both the controller and
// the object).
template<typename T>
class TIntrusiveReferenceController final : public FReferenceControllerBase
{
public:
    T Object;

    template<typename... Args>
    explicit TIntrusiveReferenceController(Args&&... args)
        : Object(std::forward<Args>(args)...)
    {}

    void DestroyObject() override
    {
        // Object's own destructor runs; the controller itself is freed
        // separately once WeakReferenceCount also hits zero (matches the
        // strong/weak two-phase teardown you saw in the disassembly:
        // DestroyObject() runs when SharedReferenceCount hits 0, but the
        // controller memory itself isn't freed until WeakReferenceCount
        // also hits 0).
        Object.~T();
    }
};

template<typename T>
struct TSharedRef
{
    T* Object = nullptr;
    FReferenceControllerBase* ReferenceController = nullptr;

    // --- Construction: allocate a real object + controller, no binary calls ---
    template<typename... Args>
    static TSharedRef Make(Args&&... args)
    {
        auto* Controller = new TIntrusiveReferenceController<T>(std::forward<Args>(args)...);
        TSharedRef Ref;
        Ref.Object = &Controller->Object;
        Ref.ReferenceController = Controller;
        return Ref; // starts at SharedReferenceCount == 1, matches controller's own init
    }

    // --- Converting copy: U* must be convertible to T* (e.g. Derived* -> Base*) ---
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedRef(const TSharedRef<U>& Other)
        : Object(static_cast<T*>(Other.Object))          // real pointer adjustment happens here
        , ReferenceController(Other.ReferenceController)  // controller is SHARED, not duplicated
    {
        if (ReferenceController)
            ++ReferenceController->SharedReferenceCount;
    }

    // --- Converting move ---
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedRef(TSharedRef<U>&& Other) noexcept
        : Object(static_cast<T*>(Other.Object))
        , ReferenceController(Other.ReferenceController)
    {
        Other.Object = nullptr;
        Other.ReferenceController = nullptr;
    }

    // --- Converting assignment (copy) ---
    template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    TSharedRef& operator=(const TSharedRef<U>& Other)
    {
        Release();
        Object = static_cast<T*>(Other.Object);
        ReferenceController = Other.ReferenceController;
        if (ReferenceController)
            ++ReferenceController->SharedReferenceCount;
        return *this;
    }

    TSharedRef()
        : Object(nullptr), ReferenceController(nullptr)
    {}

    // --- Copy: increment ---
    TSharedRef(const TSharedRef& Other)
        : Object(Other.Object), ReferenceController(Other.ReferenceController)
    {
        if (ReferenceController)
            ++ReferenceController->SharedReferenceCount;
    }

    TSharedRef& operator=(const TSharedRef& Other)
    {
        if (this != &Other)
        {
            Release();
            Object = Other.Object;
            ReferenceController = Other.ReferenceController;
            if (ReferenceController)
                ++ReferenceController->SharedReferenceCount;
        }
        return *this;
    }

    // --- Move: steal, no refcount change needed ---
    TSharedRef(TSharedRef&& Other) noexcept
        : Object(Other.Object), ReferenceController(Other.ReferenceController)
    {
        Other.Object = nullptr;
        Other.ReferenceController = nullptr;
    }

    TSharedRef& operator=(TSharedRef&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();
            Object = Other.Object;
            ReferenceController = Other.ReferenceController;
            Other.Object = nullptr;
            Other.ReferenceController = nullptr;
        }
        return *this;
    }

    // --- Destruction: decrement, and tear down at zero ---
    ~TSharedRef()
    {
        Release();
    }

    T* operator->() const { return Object; }
    T& operator*()  const { return *Object; }

private:
    void Release()
    {
        if (!ReferenceController)
            return;

        if (--ReferenceController->SharedReferenceCount == 0)
        {
            ReferenceController->DestroyObject();

            if (--ReferenceController->WeakReferenceCount == 0)
                delete ReferenceController; // virtual dtor handles the concrete type correctly
        }

        Object = nullptr;
        ReferenceController = nullptr;
    }

    template<typename U> friend struct TSharedRef;
};