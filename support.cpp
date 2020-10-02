//
//  support.cpp
//  Project4
//
//  Created by Amy Seo on 3/10/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//

#include "support.h"
#include <algorithm>

bool compareStreetSegments(ComparableStreetSegment &s1, ComparableStreetSegment &s2)
{
    return s1.getDistanceFromTarget() < s2.getDistanceFromTarget();
}
void prioritizeCloserStreets(std::vector<StreetSegment> &streets, GeoCoord end)
{
    std::vector<StreetSegment>::iterator itr = streets.begin();
    std::vector<ComparableStreetSegment> compare;
    while (itr != streets.end())
    {
        compare.push_back(ComparableStreetSegment(*itr, end)); // need to make this an object that wouldn't change when itr changes
        itr++;
    }
    
    std::sort(compare.begin(), compare.end(), compareStreetSegments);
    streets.clear();
    std::vector<ComparableStreetSegment>::iterator compareItr = compare.begin();
    while (compareItr != compare.end())
    {
        streets.push_back(compareItr->getStreet());
        compareItr++;
    }
}
