#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <climits>
#include <cstddef>

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
    //Using typedef to simplify the writing
    typedef T*      pointer;

    //Define rebind<U>::other interface
    template <typename U> struct rebind
    {
        typedef MemoryPool<U> other;
    };

    //Default constructor, initialize all pointers
    //C++11 using nonexcept to explictly declare this function will not throw exception
    MemoryPool() noexcept
    {
        currentBlock_ = nullptr;
        currentSlot_ = nullptr;
        lastSlot_ = nullptr;
        freeSlots_ = nullptr;
    }

    //Deconstruct
    ~MemoryPool() noexcept
    //looping to destory every slots in the memory block    
    {
        slot_pointer_ curr = currentBlock_;
        while (curr != nullptr)
        {
            slot_pointer_ prev = curr->next;
            operator delete(reinterpret_cast<void*>(curr));
            curr = prev;
        }
    }

    //Allocate one at one time, n and hint will be ignored
    pointer allocate(size_t n = 1, const T* hint = 0)
    {
        // If have free slot, allocate
        if (freeSlots_ != nullptr)
        {
            pointer result = reinterpret_cast<pointer>(freeSlots_);
            freeSlots_ = freeSlots_->next;
            return result;
        }
        else
        {
            // if does not have free slot, allocate new memory block
            if (currentSlot_ >= lastSlot_)
            {
                // Assign a new memory block, point to the previous block
                data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(BlockSize));
                reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
                currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
                //Fill the whole block to satisfy the alignment requirement 
                data_pointer_ body = newBlock + sizeof(slot_pointer_);
                uintptr_t result = reinterpret_cast<uintptr_t>(body);
                size_t bodyPadding = (alignof(slot_type_) - result) % alignof(slot_type_);
                currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
                lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + BlockSize - sizeof(slot_type_));
            }
            return reinterpret_cast<pointer>(currentSlot_++);
        }
    }

    //Destroy the memory block the pointer points to
    void deallocate(pointer p, size_t n = 1)
    {
        if (p != nullptr) 
        {
            // reinterpret_cast is force converting sign
            // To access next we have to forcely convert p to slot_pointer_
            reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
            freeSlots_ = reinterpret_cast<slot_pointer_>(p);
        }
    }

    //Call construct function
    template <typename U, typename... Args>
    void construct(U *p, Args&&... args)
    {
        new (p) U (std::forward<Args>(args)...);
    }

    //Destroy the objects in memory pool => call desctructor
    template <typename U>
    void destroy(U *p)
    {
        p->~U();
    }

private:
    //Use to store object slot in memory pool
    //Either be initialize as a slot to store object
    //Or be initialize as a pointer points to a slot
    union Slot_
    {
        T element;
        Slot_* next;
    };

    //Data pointer
    typedef char* data_pointer_;
    //Object Slot
    typedef Slot_ slot_type_;
    //Pointer to object slot
    typedef Slot_* slot_pointer_;

    //Point to current memory block
    slot_pointer_ currentBlock_;
    //Point to the slot in current memory block
    slot_pointer_ currentSlot_;
    //Point to the last slot in the current memory block
    slot_pointer_ lastSlot_;
    //Point to the free slot in the current memory block
    slot_pointer_ freeSlots_;

    //Check whether the selfdefined memory pool is too small
    static_assert(BlockSize >= 2 * sizeof(slot_type_), "Block Size too Small" );

};

#endif //MEMORY_POOL_HPP