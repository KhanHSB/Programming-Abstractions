/*
 * HeapPatientQueue.cpp
 *
 * This file implements the members of the HeapPatientQueue class.
 */

#include "HeapPatientQueue.h"

#include <utility>
#include <sstream>

using namespace std;

// Constructor. Creates empty queue
HeapPatientQueue::HeapPatientQueue()
    : elements(nullptr), capacity(0), count(0)
{
}

// Destructor. Frees the memory allocated for array
HeapPatientQueue::~HeapPatientQueue()
{
    clear();
}

// Removes all elements from the patient queue
void HeapPatientQueue::clear()
{
    delete[] elements;
    elements = nullptr;
    capacity = count = 0;
}

// Returns the name of the most urgent patient.
// Throws string exception if the queue does not contain any patients.
string HeapPatientQueue::frontName()
{
    if (isEmpty())
            throw frontNameExceptStr();
    return elements[1].name;
}

// Returns the integer priority of the most urgent patient
// Throws string exception if the queue does not contain any patients.
int HeapPatientQueue::frontPriority()
{
    if (isEmpty())
            throw frontPriorityExceptStr();
    return elements[1].priority;
}

// Returns true if your patient queue does not contain any elements
// and false if it does contain at least one patient.
bool HeapPatientQueue::isEmpty()
{
    return (count == 0);
}

// Adds the given person into the patient queue with the given priority.
void HeapPatientQueue::newPatient(string name, int priority)
{
    if (++count >= capacity)
        expand();    

    elements[count].name = name;
    elements[count].priority = priority;    

    moveUp(count); // find appropriate position
}

// Removes the patient with the most urgent priority from the queue,
// and you returns their name as a string.
// Throws string exception if the queue does not contain any patients.
string HeapPatientQueue::processPatient()
{
    if (isEmpty())
        throw processPatientExceptStr();

    string name = elements[1].name;

    swap(elements[1], elements[count]);

    --count;

    moveDown(1); // find appropriate position

    return name;
}

// Modifies the priority of a given existing patient in the queue.
// Throws string exception if the given patient is present in the queue and already
// has a more urgent priority than the given new priority.
// Throws string exception if the given patient is not already in the queue
void HeapPatientQueue::upgradePatient(string name, int newPriority)
{
    int found = findName(name, 1);

    if (found == 0)
        throw upgradeNoPatientExceptStr(name, newPriority);

    if (elements[found].priority <= newPriority)
        throw upgradeWrongPriorityExceptStr(name, newPriority, elements[found].priority);

    elements[found].priority = newPriority;

    moveUp(found); // find appropriate position
}

// Returns string representation of the queue
// in the form of "{priority1:value1, priority2:value2}"
string HeapPatientQueue::toString()
{
    if (isEmpty())
        return "{}";

    stringstream str;

    str << "{" << elements[1].priority << ":";
    str << elements[1].name;

    for (int i = 2; i <= count; ++i)
    {
        str <<  ", " << elements[i].priority << ":" << elements[i].name;
    }
    str << "}";

    return str.str();
}

// expand array if there are not enough space
void HeapPatientQueue::expand()
{
    if (capacity == 0)
        capacity = 2;
    else
        capacity *= 2;

    // create new array
    Patient *newElements = new Patient[capacity];

    // copy elements
    for (int i = 1; i < count; ++i)
        newElements[i] = elements[i];

    // delete old array
    delete[] elements;

    elements = newElements;
}

// recursively move node up on the tree
void HeapPatientQueue::moveUp(int child)
{
    int parent = child / 2;

    if (parent < 1 || compare(parent, child))
        return;

    swap(elements[child], elements[parent]);

    moveUp(parent); // recursion
}

// recursively move node down on the tree
void HeapPatientQueue::moveDown(int parent)
{
    int urgent = parent;

    int left = parent * 2; // left child
    int right = parent * 2 + 1; // right child


    if (left <= count && compare(left,urgent))
        urgent = left;

    if (right <= count && compare(right, urgent))
        urgent = right;

    if (urgent != parent)
    {
        swap(elements[parent], elements[urgent]);

        moveDown(urgent); // recursion
    }
}

// compare two elements
bool HeapPatientQueue::compare(int p1, int p2)
{
    if (elements[p1].priority < elements[p2].priority)
        return true;
    if (elements[p2].priority < elements[p1].priority)
        return false;

    // if priorities equal then compare by name
    if (elements[p1].name < elements[p2].name)
        return true;

    return false;
}

// Recursive function to find patient with given name
int HeapPatientQueue::findName(string name, int parent)
{
    if (parent > count)
        return 0;

    if (elements[parent].name == name)
        return parent;

    int left = findName(name, parent * 2); // find the name in the left subtree
    int right = findName(name, parent * 2 + 1); // find the name in the left subtree

    if (left == 0 && right == 0)
        return 0;

    if (left == 0)
        return right;

    if (right == 0)
        return left;

     // compare nodes if found several patients
    if (compare(left, right))
        return left;

    return right;
}

string HeapPatientQueue::processPatientExceptStr()
{
    return "A patient can not be processed: the queue is empty.";
}

string HeapPatientQueue::frontNameExceptStr()
{
    return "Function frontName() failed: the queue is empty.";
}

string HeapPatientQueue::frontPriorityExceptStr()
{
    return "Function frontPriority() failed: the queue is empty.";
}

string HeapPatientQueue::upgradeWrongPriorityExceptStr(string name, int newPriority, int currPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "current prioritet " << currPriority;
    str << " is more urgent than new prioritet " << newPriority;
    str << ".\n";

    return str.str();
}

string HeapPatientQueue::upgradeNoPatientExceptStr(string name, int newPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "there is no patient with that name.\n";

    return str.str();
}
