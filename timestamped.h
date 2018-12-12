#pragma once

template< class T >
class TimeStamped
{
public:

    TimeStamped()
    {

    }

    TimeStamped( const T& obj, double timestamp )
    {
        _obj = obj;
        _timestamp = timestamp;
    }

    virtual ~TimeStamped()
    {

    }

    TimeStamped<T> & operator = (const TimeStamped<T>& other )
    {
        _timestamp = other._timestamp;
        _obj = other._obj;
    }

    double ts()
    {
        return get_timestamp();
    }

    void set_timestamp( double value )
    {
        _timestamp = value;
    }

    double get_timestamp()
    {
        return _timestamp;
    }

    T& obj()
    {
        return _obj;
    }

private:

protected:
    double _timestamp;
    T _obj;

};
