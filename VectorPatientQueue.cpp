/*
 * VectorPatientQueue.cpp
 *
 * This file implements the members of the VectorPatientQueue class.
 */

#include "VectorPatientQueue.h"

#include <sstream>

using namespace std;

// Constructor. Creates empty queue
VectorPatientQueue::VectorPatientQueue()
    : pq(), timestamp(0)
{
}

// Destructor. Do anything
VectorPatientQueue::~VectorPatientQueue()
{
}

// Removes all elements from the patient queue
void VectorPatientQueue::clear()
{
    pq.clear(); // just clear Vector
    timestamp = 0;
}

// Returns the name of the most urgent patient.
// Throws string exception if the queue does not contain any patients.
string VectorPatientQueue::frontName()
{
    if (isEmpty())
            throw frontNameExceptStr();

    return pq[findMostUrgent()].name;
}

// Returns the integer priority of the most urgent patient
// Throws string exception if the queue does not contain any patients.
int VectorPatientQueue::frontPriority()
{
    if (isEmpty())
            throw frontPriorityExceptStr();

    return pq[findMostUrgent()].priority;
}

// Returns true if your patient queue does not contain any elements
// and false if it does contain at least one patient.
bool VectorPatientQueue::isEmpty()
{
    return pq.isEmpty(); // just query Vector
}

// Adds the given person into the patient queue with the given priority.
void VectorPatientQueue::newPatient(string name, int priority)
{
    Patient patient;
    patient.name = name;
    patient.priority = priority;
    patient.timestamp = ++timestamp;
    pq.push_back(patient);
}

// Removes the patient with the most urgent priority from the queue,
// and you returns their name as a string.
// Throws string exception if the queue does not contain any patients.
string VectorPatientQueue::processPatient()
{
    if (isEmpty())
        throw processPatientExceptStr();

    int urgent = findMostUrgent();

    string name = pq[urgent].name;

    pq.remove(urgent);

    return name;
}

// Modifies the priority of a given existing patient in the queue.
// Throws string exception if the given patient is present in the queue and already
// has a more urgent priority than the given new priority.
// Throws string exception if the given patient is not already in the queue
void VectorPatientQueue::upgradePatient(string name, int newPriority)
{
    int found = findName(name);

    if (found == NOT_FOUND)
        throw upgradeNoPatientExceptStr(name, newPriority);

    if (pq[found].priority <= newPriority)
        throw upgradeWrongPriorityExceptStr(name, newPriority, pq[found].priority);

    // upgrade
    pq[found].priority = newPriority;
    pq[found].timestamp = ++timestamp;
}

// Returns string representation of the queue
// in the form of "{priority1:value1, priority2:value2}"
string VectorPatientQueue::toString()
{
   if (isEmpty())
       return "{}";

   // use string stream for format string
   stringstream str;

   // so as not to check the last index in the loop
   // (so that there is no comma at the end),
   // first write the first element
   str << "{" << pq[0].priority << ":";
   str << pq[0].name;

   // write rest of elements
   for (int i = 1; i < pq.size(); ++i)
   {
       str <<  ", " << pq[i].priority << ":" << pq[i].name;
   }
   str << "}";

   return str.str();
}

// Returns index of most urgent patient
int VectorPatientQueue::findMostUrgent()
{
    int found = 0;
    // find element with the smallest priority
    for (int i = 0; i < pq.size(); ++i)
    {
        // if priorities is equal then compare timestamps
        if (pq[i].priority == pq[found].priority)
        {
            if (pq[i].timestamp < pq[found].timestamp)
                found = i;
        }
        else if (pq[i].priority < pq[found].priority)
            found = i;
    }

    return found;
}

// Returns index by patient name or -1 if not found
int VectorPatientQueue::findName(string name)
{
    int found = NOT_FOUND;

    for (int i = 0; i < pq.size(); ++i)
    {
        if (pq[i].name == name)
        {
            if (found != NOT_FOUND) // this name has already been found
            {
                // compare priorities
                if (pq[i].priority == pq[found].priority)
                {
                    // if priorities is equal then compare timestamps
                    if (pq[i].timestamp < pq[found].timestamp)
                        found = i;
                }
                else if (pq[i].priority < pq[found].priority)
                    found = i;

            }
            else // this name has not yet been found
                found = i;
        }
    }

    return found;
}

string VectorPatientQueue::processPatientExceptStr()
{
    return "A patient can not be processed: the queue is empty.";
}

string VectorPatientQueue::frontNameExceptStr()
{
    return "Function frontName() failed: the queue is empty.";
}

string VectorPatientQueue::frontPriorityExceptStr()
{
    return "Function frontPriority() failed: the queue is empty.";
}

string VectorPatientQueue::upgradeWrongPriorityExceptStr(string name, int newPriority, int currPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "current prioritet " << currPriority;
    str << " is more urgent than new prioritet " << newPriority;
    str << ".\n";

    return str.str();
}

string VectorPatientQueue::upgradeNoPatientExceptStr(string name, int newPriority)
{
    stringstream str;
    str << "Function upgradePatient(" << name;
    str << ", " << newPriority << ") failed: ";
    str << "there is no patient with that name.\n";

    return str.str();
}
