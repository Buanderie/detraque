#pragma once

#include "timestamped.h"
#include "utils.h"

#include <deque>
#include <mutex>
#include <iostream>

using namespace std;

template<class T>
class TemporalBuffer
{
public:
    TemporalBuffer( double maxTimeSpanSeconds = 1.0 )
        :_maxTimeSpan(maxTimeSpanSeconds), _newestTs(-DBL_MAX), _oldestTs(-DBL_MAX)
    {

    }

    virtual ~TemporalBuffer()
    {

    }

    bool push( TimeStamped< T > item )
    {

        std::unique_lock<std::mutex>( _mtx );

        double ts = item.ts();

        // Trivial insert if history is empty
        if( _ts.size() == 0 )
        {
            _newestTs = ts;
            _oldestTs = ts;
            _ts.push_back( ts );
            _samples.push_back( item.obj() );
            return true;
        }

        // Check if the ts the newest
        if( ts <= _newestTs )
        {
            return false;
            // Or insert properly ??
        }

        _newestTs = ts;

        // Remove previous entries until timespan is under _maxTimeSpan
        while( timespan() > _maxTimeSpan )
        {
            // Remove oldest
            _ts.erase( _ts.begin() );
            _samples.pop_front();
            _oldestTs = _ts[0];
        }

        // Insert the new measurement
        _ts.push_back( ts );
        _samples.push_back( item.obj() );
        _newestTs = ts;

        return true;
    }

    double timespan()
    {
        std::unique_lock<std::mutex>( _mtx );
        return _newestTs - _oldestTs;
    }

    bool sample( double ts, T& item )
    {
        std::unique_lock<std::mutex>( _mtx );

        deque<double>::iterator it = find(_ts.begin(), _ts.end(), ts );
        if(it!=_ts.end())
        {
            auto index = std::distance(_ts.begin(), it);
            item = _samples[ index ];
            return true;
        }
        return binarySearchSamples( ts, item );
    }

    virtual T interpolate( T& a, T& b, double it )
    {
        return a;
    }

    double first_ts()
    {
        std::unique_lock<std::mutex>( _mtx );
        return _oldestTs;
    }

    double last_ts()
    {
        std::unique_lock<std::mutex>( _mtx );
        return _newestTs;
    }

    void clear()
    {
        std::unique_lock<std::mutex>( _mtx );
        _ts.clear();
        _samples.clear();
        _newestTs = -DBL_MAX;
        _oldestTs = -DBL_MAX;
    }

private:
    bool binarySearchSamples(double ts, T& meas)
    {
        double ret = DBL_MIN;
        int low = 0, high = _ts.size() - 1, midpoint = 0;
        while (low <= high)
        {
            midpoint = low + (high - low)/2;
            if ( ts == _ts[midpoint] )
            {
                return midpoint;
            }
            else if ( ts < _ts[midpoint] )
                high = midpoint - 1;
            else
                low = midpoint + 1; //when key is > array[midpoint]
        }

        if( high < 0 || low < 0 )
            return false;

        if( high == _ts.size() - 1 && _ts[high] < ts )
        {
            // outside upper bound
            // std::cerr << "outside upper bound" << std::endl;
            return false;
        }
        else if( high < 0 )
        {
            // outside lower bound
            //std::cerr << "outside lower bound" << std::endl;
            return false;
        }
        else
        {
            // std::cerr << " in between !" << std::endl;
            // Interpolate between two orientations
            T a = _samples[high];
            T b = _samples[low];
            double at = _ts[high];
            double bt = _ts[low];
            double interpolation_t = ((1.0) / (bt - at)) * (ts - at);
            meas = interpolate( a, b, interpolation_t );
            return true;
        }
        return false;
    }

protected:
    double _maxTimeSpan;
    double _oldestTs;
    double _newestTs;

    std::deque< double > _ts;
    std::deque< T > _samples;

    std::mutex _mtx;

};
