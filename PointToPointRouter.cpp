//
//  PointToPointRouter.cpp
//  Project4
//
//  Created by Amy Seo on 3/6/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
#include "ExpandableHashMap.h"
#include "provided.h"
#include <list>
#include <queue>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
                                             const GeoCoord& start,
                                             const GeoCoord& end,
                                             list<StreetSegment>& route,
                                             double& totalDistanceTravelled) const;
private:
    const StreetMap* m_map; // holds all the data of street segments
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_map = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
                                                                 const GeoCoord& start,
                                                                 const GeoCoord& end,
                                                                 list<StreetSegment>& route,
                                                                 double& totalDistanceTravelled) const
{
    if (start == end) // if starting and ending geocoordinates are the same
    {
        route.clear();
        totalDistanceTravelled = 0;
        return DELIVERY_SUCCESS; // since u are already there, u can deliver
    }
    
    vector<StreetSegment> streets; // holds all the street segments
    if (!m_map->getSegmentsThatStartWith(start, streets) || !m_map->getSegmentsThatStartWith(end, streets)) // if starting or ending coords are invalid
    {
        return BAD_COORD;
    }
    
    m_map->getSegmentsThatStartWith(start, streets); // vector of street segments in streets
    vector<StreetSegment>::iterator itr = streets.begin();
    queue<GeoCoord> pathQueue; //  queue of coordinates to visit
    ExpandableHashMap<GeoCoord, GeoCoord> locationOfPreviousWayPoint;
    while (itr != streets.end())
    {
        pathQueue.push(itr->end);
        locationOfPreviousWayPoint.associate(itr->end, start);
        itr++;
    }
    
    while (!pathQueue.empty())
    {
        GeoCoord current = pathQueue.front();
        pathQueue.pop();
        if (current == end) // if the ending geoCoord is found, put all the streetsegements into the route list
        {
            GeoCoord *prev = locationOfPreviousWayPoint.find(current);
            while (current != start)
            {
                vector<StreetSegment> streetsForName;
                m_map->getSegmentsThatStartWith(*prev, streetsForName);
                vector<StreetSegment>::iterator itr = streetsForName.begin();
                while (itr!=streetsForName.end())
                {
                    if (itr->start == *prev && itr->end == current)
                    {
                        route.push_front(*itr);
                        break;
                    }
                    itr++;
                }
                totalDistanceTravelled += distanceEarthMiles(*prev, current);
                current = *prev;
                prev = locationOfPreviousWayPoint.find(*prev);
            }
            break;
        }
        
        // if ending coordinate not found, then find street segments that start with the current coordinate
        vector<StreetSegment> streets;
        m_map->getSegmentsThatStartWith(current, streets);
        vector<StreetSegment>::iterator itr = streets.begin();
        while (itr != streets.end())
        {
            if (locationOfPreviousWayPoint.find(itr->end) == nullptr)
            {
                pathQueue.push(itr->end);
                locationOfPreviousWayPoint.associate(itr->end, current);
            }
            itr++;
        }
    }

    if (route.empty())
        return NO_ROUTE;
    else
        return DELIVERY_SUCCESS;
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
                                                             const GeoCoord& start,
                                                             const GeoCoord& end,
                                                             list<StreetSegment>& route,
                                                             double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}

