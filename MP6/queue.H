
#ifndef QUEUE_H
#define QUEUE_H


#include"thread.H"
#include "console.H"
#include "utils.H"

// A linked list (LL) node to store a queue entry 
struct QNode 
{ 
	Thread *key; 
	struct QNode *next; 
}; 

// The queue, front stores the front node of LL and rear stores ths 
// last node of LL 
class Queue 
{ 
public:

	struct QNode *front, *rear; 
	
	    // A utility function to create a new linked list node. 
    struct QNode* newNode(Thread *k) 
    { 
	    struct QNode *temp = new QNode(); 
	    temp->key = k; 
	    temp->next = NULL; 
	    return temp; 
    } 

    // A utility function to create an empty queue 
    Queue() 
    { 
	    
	    front = rear = NULL; 
    } 

    // The function to add a key k to q 
    void enQueue(Thread* k) 
    { 
	    // Create a new LL node 
	    struct QNode *temp = newNode(k); 

	    // If queue is empty, then new node is front and rear both 
	    if (rear == NULL) 
	    { 
	        front = rear = temp; 
	        return; 
	    } 

	    // Add the new node at the end of queue and change rear 
	    rear->next = temp; 
	    rear = temp; 
    } 

    // Function to remove a key from given queue q 
    struct QNode *deQueue() 
    { 
	    // If queue is empty, return NULL. 
	    if (front == NULL) 
	    return NULL; 

	    // Store previous front and move front one node ahead 
	    struct QNode *temp = front; 
	    front = front->next; 

	    // If front becomes NULL, then change rear also as NULL 
	    if (front == NULL) 
	    rear = NULL; 
	    return temp; 
    } 
    
    int size()
    {
        struct QNode *temp = front;
        int count = 0;
        while(temp!=NULL)
        {
            count = count +1;
            temp = temp->next;
        }
        return count;
    }

}; 

#endif
