//
//  DeliveryOptimizer.cpp
//  Project4
//
//  Created by Amy Seo on 3/6/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
#include "provided.h"
#include <algorithm>
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap*  m_Map;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_Map = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder( // greedy algorithm
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = 0; // initially set these to zero incase they hold a value
    newCrowDistance = 0;
    vector<DeliveryRequest> result; // make a new vector of the resulting optimized deliveries
    vector<DeliveryRequest>::iterator itr = deliveries.begin();
    vector<DeliveryRequest>::iterator prev = deliveries.begin();
    vector<DeliveryRequest>::iterator minItr = deliveries.begin();
    
    double minDist = distanceEarthMiles(depot, itr->location);
    oldCrowDistance += minDist;
    itr++;
    
    while (itr != deliveries.end())  // calculate initial crow distance and the delivery with minimum distance from depot
    {
        double iDist = distanceEarthMiles(prev->location, itr->location);
        if (iDist < minDist)
        {
            minDist = iDist;
            minItr = itr;
        }
        oldCrowDistance += iDist;
        prev = itr;
        itr++;
    }
    
    result.push_back(*minItr); // found first delivery (the closest one)
    newCrowDistance += distanceEarthMiles(depot, minItr->location);
    GeoCoord startCoord = minItr->location;
    deliveries.erase(minItr); // remove this delivery to sort the reminaing ones
    
    while (deliveries.size() != 0) // while there are deliveries left to organize
    {
        itr = deliveries.begin(); // set iterators to first delivery
        minItr = deliveries.begin();
        minDist = distanceEarthMiles(startCoord, itr->location); // set initial minDist to first delivery's distance from start
        itr++;
        while (itr != deliveries.end()) // while traversing through deliveries left
        {
            double iDist = distanceEarthMiles(startCoord, itr->location); // calculate distance from start to current delivery's location
            if (iDist < minDist) // if this distance is less than current minimum, set it to minimum
            {
                minDist = iDist;
                minItr = itr;
            }
            itr++;
        }
        result.push_back(*minItr); // add this minimum distance delivery to the result
        startCoord = minItr->location; //  set start to this minimum delivery's location for next delivery
        deliveries.erase(minItr); // remove delivery as an option
    }
    
    deliveries = result; // set resulting delivery order to the deliveries vector
    
    itr = deliveries.begin();
    prev = deliveries.begin();
    itr++;
    
    while (itr != deliveries.end()) //  calculate new crow distance 
    {
        newCrowDistance += distanceEarthMiles(itr->location, prev->location);
        prev = itr;
        itr++;
    }
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}

