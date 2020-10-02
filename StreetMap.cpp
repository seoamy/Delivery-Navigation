//
//  StreetMap.cpp
//  Project4
//
//  Created by Amy Seo on 3/6/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<std::string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> streetMap;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if (!infile)
    {
        return false; // file could not be opened
    }
    string streetName;
    while (getline(infile, streetName)) // while there is a street to load
    {
        int numSegments;
        infile >> numSegments;
        for (int i = 0; i < numSegments; i++)
        {
            string g1lat, g1long, g2lat, g2long;
            infile >> g1lat;
            infile >> g1long;
            infile >> g2lat;
            infile >> g2long;
            
            GeoCoord start = GeoCoord(g1lat, g1long);
            GeoCoord end = GeoCoord(g2lat, g2long);
            
            StreetSegment forward = StreetSegment(start, end, streetName);
            StreetSegment reverse = StreetSegment(end, start, streetName);
            
            vector<StreetSegment> *addForwardHere = streetMap.find(start);
            vector<StreetSegment> *addReverseHere = streetMap.find(end);
            if (addForwardHere == nullptr)
            {
                vector<StreetSegment> s;
                s.push_back(forward);
                streetMap.associate(start, s);
            }
            else
            {
                addForwardHere->push_back(forward);
            }
            if (addReverseHere == nullptr)
            {
                vector<StreetSegment> e;
                e.push_back(reverse);
                streetMap.associate(end, e);
            }
            else
            {
                addReverseHere->push_back(reverse);
            }
        }
        string dummy; // this dummy string takes in the newline so that the loop can continue to the next line
        getline(infile, dummy);
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    const vector<StreetSegment> *allStreets = streetMap.find(gc); // returns a pointer to a vector of streetsegments
    if (allStreets == nullptr)
        return false;
    else
    {
        segs.clear();
        segs = *allStreets;
        return true;
    }
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    return m_impl->getSegmentsThatStartWith(gc, segs);
}
