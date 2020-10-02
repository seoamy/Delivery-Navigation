//
//  support.hpp
//  Project4
//
//  Created by Amy Seo on 3/10/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//

#ifndef support_hpp
#define support_hpp

#include "provided.h"
#include <vector>
class ComparableStreetSegment
{
public:
    ComparableStreetSegment(StreetSegment s, GeoCoord e)
    {
        m_street = s;
        endGoal = e;
        distanceFromTarget = distanceEarthMiles(m_street.end, e);
    }
    double getDistanceFromTarget()
    {
        return distanceFromTarget;
    }
    StreetSegment getStreet()
    {
        return m_street;
    }
private:
    GeoCoord endGoal;
    StreetSegment m_street;
    double distanceFromTarget;
};

bool compareStreetSegments(ComparableStreetSegment &s1, ComparableStreetSegment &s2);
void prioritizeCloserStreets(std::vector<StreetSegment> &streets, GeoCoord end);

#endif /* support_hpp */
