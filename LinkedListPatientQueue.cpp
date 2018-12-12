/*
 * LinkedListPatientQueue.cpp
 *
 * This file implements the members of the LinkedListPatientQueue class.
 */

#include "LinkedListPatientQueue.h"

#include <sstream>

using namespace std;

// Constructor. Creates empty list
LinkedListPatientQueue::LinkedListPatientQueue()
    : front(nullptr)
{
}

// Destructor. Frees the memory allocated for nodes
LinkedListPatientQueue::~LinkedListPatientQueue()
{
   clear();
}

// Removes all elements from the patient queue
void LinkedListPatientQueue::clear()
{
    // remove nodes from the front until empty list
    while (front != nullptr)
    {
        PatientNode *toDelete = front;
        front = front->next;
        delete toDelete;
    }
}

// Returns the name of the most urgent patient.
// Throws string exception if the queue does not contain any patients.
string LinkedListPatientQueue::frontName()
{
    if (isEmpty())
        throw frontNameExceptStr();

    return front->name;
}

// Returns the integer priority of the most urgent patient
// Throws string exception if the queue does not contain any patients.
int LinkedListPatientQueue::frontPriority()
{
    if (isEmpty())
        throw frontPriorityExceptStr();
    return front->priority;
}

// Returns true if your patient queue does not contain any elements
// and false if it does contain at least one patient.
bool LinkedListPatientQueue::isEmpty()
{
    return (front == nullptr);
}

// Adds the given person into the patient queue with the given priority.
void LinkedListPatientQueue::newPatient(string name, int priority)
{
    PatientNode *newPatientNode = new PatientNode(name, priority);

    if (isEmpty())
    {
        front = newPatientNode; // first node
    }
    else if (priority < front->priority) // insert at the front
    {
        // link nodes
        newPatientNode->next = front;
        front = newPatientNode;
    }
    else // insert in the middle
    {
        // find a node with a priority greater than or equal to given.
        PatientNode *found = findBeforePriority(priority);
        // link nodes
        newPatientNode->next = found->next;
        found->next = newPatientNode;
    }
}

// Removes the patient with the most urgent priority from the queue,
// and you returns their name as a string.
// Throws string exception if the queue does not contain any patients.
string LinkedListPatientQueue::processPatient()
{
    if (isEmpty())
        throw processPatientExceptStr();

    // remove node from the front of the list
    string name = front->name;
    PatientNode *toDelete = front;
    front = front->next;
    delete toDelete;

    return name;
}

// Modifies the priority of a given existing patient in the queue.
// Throws string exception if the given patient is present in the queue and already
// has a more urgent priority than the given new priority.
// Throws string exception if the given patient is not already in the queue
void LinkedListPatientQueue::upgradePatient(string name, int newPriority)
{
    if (isEmpty())
        throw upgradeNoPatientExceptStr(name, newPriority);

    if (front->name == name) // check front node
    {
        if (front->priority <= newPriority)
            throw upgradeWrongPriorityExceptStr(name, newPriority, front->priority);
        front->priority = newPriority;
    }
    else
    {
        // find the node preceding the node with this name
        PatientNode *found = findBeforeName(name);

        if (found == nullptr) // no patient
            throw upgradeNoPatientExceptStr(name, newPriority);

        // node to ugrade
        PatientNode *toUpgrade = found->next;

        if (toUpgrade->priority <= newPriority) // wrong priority
            throw upgradeWrongPriorityExceptStr(name, newPriority, toUpgrade->priority);

        // unlink nodes
        found->next = toUpgrade->next;
        toUpgrade->priority = newPriority;

        // find a new place for the node
        if (newPriority < front->priority) // insert at front
        {
            toUpgrade->next = front;
            front = toUpgrade;
        }
        else // insert in middle
        {
            found = findBeforePriority(newPriority);
            // link nodes
            toUpgrade->next = found->next;
            found->next = toUpgrade;
        }
    }
}

// Returns string representation of the queue
// in the form of "{priority1:value1, priority2:value2}"
string LinkedListPatientQueue::toString()
{
    if (isEmpty())
        return "{}";

    stringstream str;

    PatientNode *iter = front;

    str << "{" << *iter;

    for (iter = iter->next; iter != nullptr; iter = iter->next)
    {
        str <<  ", " << *iter;
    }
    str << "}";

    return str.str();
}

// Finds a node preceding the node with this name
// Returns found node or nullptr if not found
PatientNode *LinkedListPatientQueue::findBeforeName(string name)
{
    PatientNode *curr = front; // set iterator to the first node

    for (; curr->next != nullptr; curr = curr->next)
    {
        if (curr->next->name == name) // check next node
            return curr;
    }

    return nullptr;
}

// Finds a node preceding the node with priority greater than or equal to given.
// Returns found node
PatientNode *LinkedListPatientQueue::findBeforePriority(int priority)
{
    PatientNode *curr = front; // set iterator to the first node

    for (; curr->next != nullptr; curr = curr->next)
    {
        if (curr->next->priority > priority) // check next node
            break;
    }

    return curr;
}

string LinkedListPatientQueue::processPatientExceptStr()
{
    return "A patient can not be processed: the queue is empty.";
}

string LinkedListPatientQueue::frontNameExceptStr()
{
    return "Function frontName() failed: the queue is empty.";
}

string LinkedListPatientQueue::frontPriorityExceptStr()
{
    return "Function frontPriority() failed: the queue is empty.";
}

string LinkedListPatientQueue::upgradeWrongPriorityExceptStr(string name, int newPriority, int currPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "current prioritet " << currPriority;
    str << " is more urgent than new prioritet " << newPriority;
    str << ".\n";

    return str.str();
}

string LinkedListPatientQueue::upgradeNoPatientExceptStr(string name, int newPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "there is no patient with that name.\n";

    return str.str();
}
