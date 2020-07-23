# Heap-Allocation
Heap allocation stimulation using linked list.

Structure
2 Linked lists :- 1>Allocnode l.list to keep list of allocated nodes with their names,sizes,location
		  2>Freelist l.list to keep list of freelist nodes with their location and sizes

Method for allocation
	1.On allocation for specific size of bytes, a new_node is formed containing all the details of the allocation(name,size,location)
	2.We search along freelist for a node which has smallest possible difference between its size and the size of our desired node.
	3.On finding a particular freelist node :-a.free the node if exact size
						  b. decrease its size if not exact	
	4.In the allocnode list, insert_at_end this new_node at the end of the list

Method for deallocation
	1.User is asked the name of the pointer to be deallocated
	2.Search through the allocnode list for the name of this pointer and if found have it point to a tempnode
	3.Delete this tempnode from the alloclist
	4.Create a new freelist f_node with details of this tempnode such as location and size
	5.Insert into freelist this f_node at the appropriate position according to its location. 
		Ex:- 
			alloclist = a(1),b(2),c(3),d(4),e(5),f(6)	  a(1) is ptr name a with location position 1
			freelist = MAX-(s(a)+s(b)+s(c)+s(d)+s(e)+s(f))    s(a) is size of a
		**on freeing c**
                alloclist = a(1),b(2),d(4),e(5),f(6)
                freelist = s(c) , MAX-(s(a)+s(b)+s(d)+s(e)+s(f))
		**on freeing d**
              alloclist = a(1),b(2),e(5),f(6)
              freelist = s(c) , s(d), MAX-(s(a)+s(b)+s(d)+s(e)+s(f))	
		**on freeing a**
             alloclist = b(2),e(5),f(6) 
             freelist = s(a),s(c),s(d), MAX-(s(a)+s(b)+s(d)+s(e)+s(f))
