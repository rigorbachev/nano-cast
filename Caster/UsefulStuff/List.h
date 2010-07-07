/*
 * List.h
 *
 *  Created on: Jun 22, 2010
 *      Author: John
 */

#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>  // for NULL


// Note: Element must have a "next" pointer

template <class Element>  class List
{
public:  // We should do an iterator instead
	Element* first;   // pointer to first element in list

public:
	List() {
		first = NULL;
	}

	virtual ~List() {
		Delete();
	}

	Element* Head(void) {
		return first;
	}


	void Push(Element* node) {
		node->next = first;
		first = node;
	}

	Element* Pop() {
		Element* node = first;
		if (first != NULL) {
			first = first->next;
			node->next = NULL;
		}
		return node;
	}


        void Add(Element* node) {
            Push(node);
        }

	void Delete() {
		while (Head() != NULL)
			delete Pop();
	}



        // Note: Use doubly linked list if this is a common operation
	void Remove(Element* node) {

		// If at head of list, delete it
		if (first == node)
			first = first->next;

		// Otherwise,
		else {
			// Scan the list, deleting the node if found
			for (Element *prev=first; prev->next != NULL; prev=prev->next) {
			    if (prev->next == node) {
			    	prev->next = node->next;
			    	node->next = NULL;
			    	break;
			    }
			}
		}
	}

};




// Just like List, but elements are pushed onto the end of the list.

template <class Element>  class FifoList : public List<Element> {
protected:
    Element* last;

public:
    using List<Element>::first;

    void Push(Element* e) 
    {
        e->next = NULL;

        if (first == NULL) first = e;
        else               last->next = e;

        last = e;
    }

    void Add(Element* e) {Push(e);}
};

#endif /* LIST_H_ */
