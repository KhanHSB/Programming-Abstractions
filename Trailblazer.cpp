// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own, along with
// comments on every function and on complex code sections.
// TODO: write comment header for this file; remove this comment
// ASSIGNMENT IN C++

#include "Trailblazer.h"
#include "queue.h"
#include "priorityqueue.h"
#include "set.h"
#include "hashmap.h"
#include <utility>

using namespace std;

static const double SUFFICIENT_DIFFERENCE = 0.2;

struct CurrentPath
{
    RoadNode* node;
    Path path;    
};

// Check if path has sufficient difference with shortestPath
bool isSufficientDiff(const Path& shortestPath, const Path& path)
{
    int diff = 0;
    for (const auto &p: shortestPath)
    {
        if (!path.contains(p)) // count differences
            ++diff;
    }

    if (static_cast<double>(diff) / path.size() > SUFFICIENT_DIFFERENCE)
        return true;

    return false;
}

// Returns cost of the path
double getCost(const RoadGraph& graph, const Path& path)
{
    double cost = 0;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        cost += graph.edgeBetween(path[i], path[i +1])->cost();
    }
    return cost;
}

// Returns best alternative path
Path getBestAltPath(const RoadGraph& graph, Vector<Path>& alterPaths, const Path& shortestPath)
{

    // remove insufficient differenced
    int i = 0;
    while ( i < alterPaths.size())
    {
        if (!isSufficientDiff(shortestPath, alterPaths[i]))
            alterPaths.remove(i);
        else
            ++i;
    }

    if (alterPaths.isEmpty())
        return {};

    // find path index what has minimal cost
    double minCost = getCost(graph, alterPaths[0]);
    double minIndex = 0;
    for (i = 1; i < alterPaths.size(); ++i)
    {
        double cost = getCost(graph, alterPaths[i]);
        if ( cost < minCost)
        {
            minCost = cost;
            minIndex = i;
        }
    }

    return alterPaths[minIndex];
}

Path breadthFirstSearch(const RoadGraph& graph, RoadNode* start, RoadNode* end)
{
    Queue<CurrentPath> pathQueue;
    Set<RoadNode*> visitedSet;

    visitedSet.add(start);


    pathQueue.enqueue({start, {start}});

    while(!pathQueue.isEmpty())
    {
        CurrentPath currPath = pathQueue.dequeue();
        RoadNode* u = currPath.node;
        u->setColor(Color::GREEN);

        if (u == end)
            return currPath.path;

        for (const auto &v: graph.neighborsOf(u))
        {
            if (!visitedSet.contains(v))
            {
                v->setColor(Color::YELLOW);

                CurrentPath nextPath;
                nextPath.node = v;
                nextPath.path = currPath.path;
                nextPath.path.add(v);
                pathQueue.enqueue(nextPath);
                //visitedSet.add(v);
            }
        }
        visitedSet.add(u);
    }

    return {};
}

Path dijkstrasAlgorithm(const RoadGraph& graph, RoadNode* start, RoadNode* end)
{
    PriorityQueue<CurrentPath> pathQueue;
    Set<RoadNode*> visitedSet;    
    HashMap<RoadNode*, double> costMap;

    pathQueue.enqueue({start, {start}}, 0);
    visitedSet.add(start);
    costMap.add(start, 0);

    while (!pathQueue.isEmpty())
    {
        CurrentPath currPath = pathQueue.dequeue();
        RoadNode* u = currPath.node;
        double cost = costMap.get(u);

        u->setColor(Color::GREEN);


        if (u == end)
            return currPath.path;

        for (const auto &v: graph.neighborsOf(u))
        {
            if (!visitedSet.contains(v))
            {
                v->setColor(Color::YELLOW);

                CurrentPath nextPath;
                nextPath.node = v;
                nextPath.path = currPath.path;
                nextPath.path.add(v);

                double oldCost = costMap.get(v);
                if (oldCost == 0.0)
                    oldCost = INFINITY;

                double newCost = graph.edgeBetween(u, v)->cost() + cost;

                if (newCost < oldCost)
                    costMap.put(v, newCost);

                pathQueue.enqueue(nextPath, min(newCost, oldCost));
            }
        }
        visitedSet.add(u);
    }

    return {};
}

Path aStar(const RoadGraph& graph, RoadNode* start, RoadNode* end)
{
    PriorityQueue<CurrentPath> pathQueue;
    Set<RoadNode*> visitedSet;
    HashMap<RoadNode*, double> gMap; // hashtable for g() function value
    HashMap<RoadNode*, double> fMap; // hashtable for f()

    gMap.add(start, 0);
    fMap.add(start, 0 + graph.crowFlyDistanceBetween(start, end));

    pathQueue.enqueue({start, {start}}, fMap.get(start));
    visitedSet.add(start);

    while (!pathQueue.isEmpty())
    {
        CurrentPath currPath = pathQueue.dequeue();
        RoadNode* u = currPath.node;
        double gCurr = gMap.get(u);

        u->setColor(Color::GREEN);

        if (u == end)
            return currPath.path;

        visitedSet.add(u);

        for (const auto &v: graph.neighborsOf(u))
        {
            double gTent = gCurr + graph.edgeBetween(u, v)->cost();

            double gv = gMap.get(v);
            if (gv == 0.0)
                gv = INFINITY;

            if (visitedSet.contains(v) && gTent >= gv)
                continue;

            v->setColor(Color::YELLOW);

            CurrentPath nextPath;
            nextPath.node = v;
            nextPath.path = currPath.path;
            nextPath.path.add(v);

            double h = (graph.crowFlyDistanceBetween(v, end)) / graph.maxRoadSpeed();

            gMap.put(v, gTent);
            fMap.put(v, gTent + h);

            pathQueue.enqueue(nextPath, gTent + h);
        }
    }

    return {};
}

Path alternativeRoute(const RoadGraph& graph, RoadNode* start, RoadNode* end)
{
    Path shortestPath = aStar(graph, start, end);

    Vector<Path> alterPaths;

    // for each edge in shortest path
    for (int i = 0; i < shortestPath.size() - 1; ++i)
    {
        // set next excluded edge
        RoadEdge* excludedEdge = graph.edgeBetween(shortestPath[i], shortestPath[i + 1]);

        // perform dijkstrasAlgorithm

        PriorityQueue<CurrentPath> pathQueue;
        Set<RoadNode*> visitedSet;
        HashMap<RoadNode*, double> costMap;

        pathQueue.enqueue({start, {start}}, 0);
        visitedSet.add(start);
        costMap.add(start, 0);

        bool bStop = false;
        while (!pathQueue.isEmpty() && !bStop)
        {
            CurrentPath currPath = pathQueue.dequeue();

            RoadNode* u = currPath.node;
            double cost = costMap.get(u);

            u->setColor(Color::GREEN);

            if (u == end)
            {
                alterPaths.add(currPath.path);
                bStop = true;
            }
            else
            {
                for (const auto &v: graph.neighborsOf(u))
                {
                    if (!visitedSet.contains(v) && excludedEdge != graph.edgeBetween(u, v))
                    {
                        v->setColor(Color::YELLOW);

                        CurrentPath nextPath;
                        nextPath.node = v;
                        nextPath.path = currPath.path;
                        nextPath.path.add(v);

                        double oldCost = costMap.get(v);
                        if (oldCost == 0.0)
                            oldCost = INFINITY;

                        double newCost = graph.edgeBetween(u, v)->cost() + cost;

                        if (newCost < oldCost)
                            costMap.put(v, newCost);

                        pathQueue.enqueue(nextPath, min(newCost, oldCost));
                    }
                }
                visitedSet.add(u);
            }

        }
    }


    return getBestAltPath(graph, alterPaths, shortestPath);
}

