#pragma once

#include <vector>
#include <Eigen/Dense>
#include <string>

namespace cvgl {

inline double clip(double v, double min, double max)
{
    return (v < min ? min : (v > max ? max : v));
}

inline Eigen::ArrayXd clip(const Eigen::ArrayXd& v, double min, double max)
{
    return v.unaryExpr([&](double x){return clip(x, min, max); });
}

inline double scale(double v, double in_min, double in_max, double out_min, double out_max)
{
    return (( (v - in_min) / (in_max - in_min) ) * (out_max - out_min)) + out_min;
}

inline Eigen::ArrayXd scale(const Eigen::ArrayXd& v, double in_min, double in_max, double out_min, double out_max)
{
    return v.unaryExpr([&](double x){ return scale(x, in_min, in_max, out_min, out_max); });
}

template <typename T>
inline std::vector<T> scale(const std::vector<T>& v, const T in_min, const T in_max, const T out_min, const T out_max)
{
    T in_range = in_max - in_min;
    T out_range = out_max - out_min;
    std::vector<T> ret( v.size() );
    for( auto& q : v )
    {
        ret.emplace_back( ( ((q - in_min) / in_range) * out_range) + out_min );
    }
    return ret;
}

inline double sum( std::vector<double> &vec )
{
    double _sum = 0;
    for (auto& n : vec){
        _sum += n;
    }
    return _sum;
}


inline int32_t round(double x)
{
    return int32_t(x + 0.5);
}

inline std::vector<double> dur2x( std::vector<double> & vec)
{
    std::vector<double> seq_x{0};
    for( size_t i = 0; i < vec.size()-1; ++i)
    {
        seq_x.emplace_back( seq_x.back() + vec[i] );
    }
    return seq_x;
}

inline Eigen::ArrayXd dur2x( Eigen::ArrayXd & vec)
{
    Eigen::ArrayXd seq_x( vec.size()+1 );
    seq_x(0) = 0;
    
    for( size_t i = 1; i < vec.size()+1; i++)
    {
        seq_x(i) = seq_x(i-1) + vec[i-1];
    }
    
    return seq_x;
}

inline Eigen::ArrayXd mtof( const Eigen::ArrayXd & v, double a4 = 440.)
{
    return a4 * pow(2., (v - 69.) / 12.) ;
}

inline double mtof( double v, double a4 = 440.)
{
    return a4 * pow(2., (v - 69.) / 12.) ;
}

inline double tanhScale( double x, double in_min, double in_max, double curveScalar = 1 )
{
    const double curveTanh = tanh(curveScalar);
    const double scaled_x = scale( x, in_min, in_max, -1., 1.);
    return scale( tanh( scaled_x * curveTanh ), -curveTanh, curveTanh, 0., 1. );
}

inline double scaleInterval( long step, std::vector<double> scale )
{
    if( step < 0 ) {
        return scale[ long( step % scale.size() ) ] + ( long( ((step+1) / (long)scale.size() ) - 1 ) * 12 );
    }
    else
        return scale[ long( step % scale.size() ) ] + long( (step / scale.size() ) * 12 );
}


inline double dbtoa(double db){ return pow(10., (db / 20.)); }


double ntom( const std::string & note );

double lineLookup( double t, std::vector<double> x, std::vector<double> y );

Eigen::ArrayXd lineLookup( const Eigen::ArrayXd& t, const std::vector<double>& x, const std::vector<double>& y );

template <typename T>
std::vector<T> aseq(T from, T to, T step = 1)
{
    std::vector<T> ret;
    T incr = from;
    while( incr <= to )
    {
        ret.emplace_back(incr);
        incr += step;
    }
    
    return ret;
}

}
