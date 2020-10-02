//
//  DeliverPlanner.cpp
//  Project4
//
//  Created by Amy Seo on 3/6/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
#include "provided.h"
#include <vector>
#include <stack>

using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
                                        const GeoCoord& depot,
                                        const vector<DeliveryRequest>& deliveries,
                                        vector<DeliveryCommand>& commands,
                                        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_Map;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_Map = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
                                                         const GeoCoord& depot,
                                                         const vector<DeliveryRequest>& deliveries,
                                                         vector<DeliveryCommand>& commands,
                                                         double& totalDistanceTravelled) const
{
    DeliveryOptimizer optimize(m_Map);
    double oldDis, newDis;
    vector<DeliveryRequest> optimizedDeliveries = deliveries;
    optimize.optimizeDeliveryOrder(depot, optimizedDeliveries, oldDis, newDis);
    
    GeoCoord start = GeoCoord(depot);
    optimizedDeliveries.push_back(DeliveryRequest("HOME", start)); // add starting GeoCoord as last "delivery" to make sure u come back to depot
    vector<DeliveryRequest>::iterator itr = optimizedDeliveries.begin();
    list<StreetSegment> path;
    PointToPointRouter router(m_Map);
    
    while (itr != optimizedDeliveries.end())
    {
        double distance = 0;
        DeliveryResult result = router.generatePointToPointRoute(start, itr->location, path, distance);
        totalDistanceTravelled += distance;
        switch (result)
        {
            case NO_ROUTE:
                return NO_ROUTE;
                break;
            case BAD_COORD:
                return BAD_COORD;
                break;
            case DELIVERY_SUCCESS:
                break;
        }
        
        list<StreetSegment>::iterator segItr = path.begin();
        if (segItr == path.end())
        {
            DeliveryCommand deliver = DeliveryCommand(); // if not home, then make an actual delivery command
            deliver.initAsDeliverCommand(itr->item);
            commands.push_back(deliver);
            return DELIVERY_SUCCESS;
        }
        
        StreetSegment* prev = nullptr;
        bool directionFound = false;
        double directionAngle = 0;
        double turnAngle = 0;
        double localDist = 0;
        string dir = "";
        while (segItr != path.end()) // while there are street segments left
        {
            if (segItr->end == itr->location) // if the street segment ends at the delivery location
            {
                localDist += distanceEarthMiles(prev->start, segItr->end); // add the distance
                if (itr->item == "HOME") //  if the location is back to the depot
                {
                    DeliveryCommand proceed = DeliveryCommand();
                    proceed.initAsProceedCommand(dir, segItr->name, localDist);
                    commands.push_back(proceed); //  add a proceed command that ends at the depot
                    return DELIVERY_SUCCESS; // all deliveries were successful
                }
                else // generate a proceed & deliver command
                {
                    DeliveryCommand proceed = DeliveryCommand();
                    proceed.initAsProceedCommand(dir, segItr->name, localDist);
                    commands.push_back(proceed); //  add last proceed command on this street
                    DeliveryCommand deliver = DeliveryCommand();
                    deliver.initAsDeliverCommand(itr->item);
                    commands.push_back(deliver); // add delivery command
                    start = itr->location;
                    localDist = 0;
                    itr++; // increment to start next delivery
                    break;
                }
            }
            
            if (prev != nullptr)
            {
                localDist += distanceEarthMiles(prev->start, segItr->start);
                if (prev->name != segItr->name) // if not on the same street anymore, calculate final distance
                {
                    DeliveryCommand proceed = DeliveryCommand();
                    proceed.initAsProceedCommand(dir, prev->name, localDist);
                    commands.push_back(proceed); //  add last proceed command on this street
                    directionFound = false;
                    localDist = 0;
                    const StreetSegment constStreetCur = *segItr;
                    const StreetSegment constStreetPrev = *prev;
                    turnAngle = angleBetween2Lines(constStreetPrev, constStreetCur); // find angle between previous and current street
                    if (turnAngle >= 1 && turnAngle < 180) // add left turn command
                    {
                        DeliveryCommand leftTurn = DeliveryCommand();
                        leftTurn.initAsTurnCommand("left", constStreetCur.name);
                        commands.push_back(leftTurn);
                    }
                    else if (turnAngle >= 180 && turnAngle <= 359) // add right turn command
                    {
                        DeliveryCommand rightTurn = DeliveryCommand();
                        rightTurn.initAsTurnCommand("right", constStreetCur.name);
                        commands.push_back(rightTurn);
                    }
                }
            }
            
            if (!directionFound) //  if no direction has been set, initialize direction based on current street segment
            {
                const StreetSegment constStreet = *segItr;
                directionAngle = angleOfLine(constStreet);
                
                if (directionAngle >= 0 && directionAngle < 22.5)
                    dir = "east";
                
                else if (directionAngle < 67.5)
                    dir = "northeast";
                
                else if (directionAngle < 112.5)
                    dir = "north";
                
                else if (directionAngle < 157.5)
                    dir = "northwest";
                
                else if (directionAngle < 202.5)
                    dir = "west";
                
                else if (directionAngle < 247.5)
                    dir = "southwest";
                
                else if (directionAngle < 292.5)
                    dir = "south";
                
                else if (directionAngle < 337.5)
                    dir = "southeast";
                
                else if (directionAngle >= 337.5)
                    dir = "east";
                
                directionFound = true;
            }
            prev = &(*segItr); // set previous pointer to current street
            segItr++; // increment itr
        }
    }
    return NO_ROUTE; // if it never reaches the delivery back home, there is no route
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
                                                     const GeoCoord& depot,
                                                     const vector<DeliveryRequest>& deliveries,
                                                     vector<DeliveryCommand>& commands,
                                                     double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
