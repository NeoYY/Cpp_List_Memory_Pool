#include <memory>

#ifndef STACK_ALLOC_H
#define STACK_ALLOC_H

template <typename T>
struct StackNode_
{
    T data;
    StackNode_* prev;
};

// T is the type of the object, Alloc is the allocator, by default is std::alloc
template <typename T, typename Alloc = std::allocator<T> >
class StackAlloc
{
public:
    //Use typedef simplify type name
    typedef StackNode_<T> Node;
	typedef typename Alloc::template rebind<Node>::other allocator;

	// Default Constructor
	StackAlloc() { head_ = 0; }
	// Default Desctructor
	~StackAlloc() { clear(); }

	//Return true when empty
	bool empty() { return (head_ == 0); }
	
	//Release everything in Stack
	void clear()
    {
        Node* curr = head_;
        //pop from stack one by one
        while (curr)
        {
            Node* tmp = curr->prev;
            //Destruct first, and reallocate the memory
            allocator_.destroy(curr);
            allocator_.deallocate(curr, 1);
            curr = tmp;
        }
        head_ = 0;
    }

	//push
	void push(T element)
    {
        //Assign memory for one node
        Node *new_Node = allocator_.allocate(1);
        //Constuctor for the node
        allocator_.construct(new_Node, Node());

        //push
        new_Node->data = element;
        new_Node->prev = head_;
        head_ = new_Node;
    }

	//pop
	T pop()
    {
        T result = head_->data;
        Node *tmp = head_->prev;
        allocator_.destroy(head_);
        allocator_.deallocate(head_, 1);
        head_ = tmp;
        return result;        
    }

	//Return the last element
	T top() { return head_->data; }

private:
	//
	allocator allocator_;
	// Stack top
	Node* head_;
};

#endif // STACK_ALLOC_H


