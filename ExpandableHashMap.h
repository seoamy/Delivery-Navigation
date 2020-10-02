//
//  main.cpp
//  Project4
//
//  Created by Amy Seo on 3/5/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
//
//  main.cpp
//  Proj4
//
//  Created by Amy Seo on 3/4/20.
//  Copyright © 2020 Amy Seo. All rights reserved.
//
// ExpandableHashMap.h

// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <list>
#include <iostream>
template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

      // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Bucket
    {
        Bucket(KeyType k, ValueType v)
        {
            key = k;
            value = v;
        }
        
        KeyType key;
        ValueType value;
    };
    
    // private member variables
    int m_size;
    int m_buckets;
    double m_maximumLoadFactor;
    std::list<Bucket>* m_hashMap;
    
    // private member functions
    unsigned int getBucketNumber(const KeyType& key) const
    {
        unsigned int hasher(const KeyType& k); // prototype
        unsigned int h = hasher(key);
        return h % m_buckets;
    }
    
    double getLoadFactor() const
    {
        return (m_size * 1.0)/m_buckets;
    }
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    m_maximumLoadFactor = maximumLoadFactor;
    m_buckets = 8;
    m_size = 0;
    m_hashMap = new std::list<Bucket> [m_buckets];
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
    delete [] m_hashMap;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
    m_buckets = 8;
    m_size = 0;
    delete [] m_hashMap;
    m_hashMap = new std::list<Bucket> [m_buckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
    return m_size;
}

// The associate method associates one item (key) with another (value).
// If no association currently exists with that key, this method inserts
// a new association into the hashmap with that key/value pair. If there is
// already an association with that key in the hashmap, then the item
// associated with that key is replaced by the second parameter (value).
// Thus, the hashmap must contain no duplicate keys.
template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    ValueType* pointerToVal = find(key);
    if (pointerToVal == nullptr) // if no key exists, add this bucket to the hashmap
    {
        m_size++;
        if (getLoadFactor() > m_maximumLoadFactor)
        {
            int oldBuckets = m_buckets;
            m_buckets = m_buckets * 2;
            std::list<Bucket>* newMap = new std::list<Bucket>[m_buckets];
            for (int i = 0; i < oldBuckets; i++)
            {
                if (!m_hashMap[i].empty())
                {
                    typename std::list<Bucket>::iterator itr;
                    itr = m_hashMap[i].begin();
                    while (itr != m_hashMap[i].end())
                    {
                        unsigned int b = getBucketNumber(itr->key);
                        newMap[b].push_back(Bucket(itr->key, itr->value));
                        itr++;
                    }
                }
            }
            delete [] m_hashMap;
            m_hashMap = newMap;
        }
        unsigned int i = getBucketNumber(key);
        m_hashMap[i].push_back(Bucket(key, value));
    }
    
    else
    {
        unsigned int i = getBucketNumber(key);
        typename std::list<Bucket>::iterator itr = m_hashMap[i].begin();
        while (itr != m_hashMap[i].end())
        {
            if ((*itr).key == key)
            {
                (*itr).value = value;
                return;
            }
            itr++;
        }
    }
}

// If no association exists with the given key, return nullptr; otherwise,
// return a pointer to the value associated with that key. This pointer can be
// used to examine that value, and if the hashmap is allowed to be modified, to
// modify that value directly within the map (the second overload enables
// this). Using a little C++ magic, we have implemented it in terms of the
// first overload, which you must implement.
template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
    unsigned int i = getBucketNumber(key);
    typename std::list<Bucket>::iterator itr;
    itr = m_hashMap[i].begin();
    while (itr != m_hashMap[i].end())
    {
        if (itr->key == key)
        {
            return &(itr->value);
        }
        itr++;
    }
    return nullptr;
}
