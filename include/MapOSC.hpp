#pragma once

//#include "osc_bundle_s.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
#include <type_traits>

#include <Eigen/Dense>

using AtomVar_t = std::variant<float, double, int32_t, int64_t, char, bool, std::string> ;

// some magic from cppreference for std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct MapAtom
{
    AtomVar_t m_atom;

    char getOSCTypeTag();
    size_t getSizeInBytes();
    
    template <typename T>
    inline T get()
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return std::visit( overloaded {
                    [](std::string& arg) { return arg; },
                    [](auto& arg) { return std::to_string(arg); }
            }, m_atom);
        }
        else
        {
            return std::visit( overloaded {
                    [](auto& arg) { return (T)arg; }, // cast to type
                    [](std::string& arg) { return (T)0; } // zero if string
            }, m_atom);
        }
    }
    
    inline int getInt()
    {
        return (int)get<int32_t>();
    }
    
    inline float getFloat(){
        return (float)get<float>();
    }
  
};

/**
 * @brief The MapOSCArray struct
 *
 * equivilant to a OSC message value, without the address
 * the address is set when stored in the main MapOSC object
 *
 * all "values" are upgraded to vectors for simplicity
 *
 */
struct MapOSCArray
{
    std::vector<MapAtom> vec;
    
    MapAtom& operator[](size_t idx) { return vec[idx]; }

    inline size_t size(){ return vec.size(); }
    inline void reserve(size_t size){ vec.reserve(size); }
    
    std::vector<MapAtom>& getAtomVector(){ return vec; }

    template <typename T>
    inline T get(size_t idx = 0) {
        return vec[idx].get<T>();
    }
        
    inline int getInt(){
        return vec[0].get<int32_t>();
    }
    
    inline float getFloat(){
        return vec[0].get<float>();
    }
    

    template <typename Derived>
    void appendValue(const Eigen::ArrayBase<Derived> &val)
    {
        if( val.size() > 0 )
        {
           for( size_t i = 0; i < val.rows(); ++i)
           {
               appendValue( val(i) );
           }
        }
    }
    
    template <typename Derived>
    void appendValue(Eigen::ArrayBase<Derived> &val)
    {
        if( val.size() > 0 )
        {
           for( size_t i = 0; i < val.rows(); ++i)
           {
               appendValue( val(i) );
           }
        }
    }
    
    template <typename Derived>
    void appendValue(std::vector<Derived> &val)
    {
       for( auto it = val.begin(); it != val.end(); it++ )
       {
           appendValue(*it);
       }
    }

    template <typename Derived>
    void appendValue(const std::vector<Derived> &val)
    {
        for( auto it = val.begin(); it != val.end(); it++ )
        {
            appendValue(*it);
        }
    }


    inline void appendValue(float val ) { vec.emplace_back(val); }
    inline void appendValue(double val ) { vec.emplace_back(val); }
    inline void appendValue(int32_t val ) { vec.emplace_back(val); }
    inline void appendValue(int64_t val ) { vec.emplace_back(val); }
    inline void appendValue(char val ) { vec.emplace_back(val); }
    inline void appendValue(const std::string& val ) { vec.emplace_back(val); }

};

class MapOSC
{
public:
    
    std::unordered_map<std::string, MapOSCArray > map;
    
    
    std::unordered_map<std::string, MapOSCArray >& getMap() { return map; }
    
    template <typename... Ts>
    void addMessage (const std::string& address, Ts&&... args)
    {
        using expand = int[];
       (void)expand{0, ((void)map[address].appendValue( std::forward<Ts>(args) ), 0) ... };
    }

    template <typename... Ts>
    void addMessage (const char * address, Ts&&... args)
    {
        using expand = int[];
        (void)expand{0, ((void)map[address].appendValue( std::forward<Ts>(args) ), 0) ... };
    }

    bool addressExists(const char * address) {
        return map.find(address) != map.end();
    }
    
    MapAtom& getMessage(const char * address, size_t idx = 0){
        return map[address][idx];
    }
        
    MapOSCArray& operator[](std::string& addr) { return map[addr]; }
    MapOSCArray& operator[](const char * addr) { return map[addr]; }
    
    void inputOSC( long len, char * ptr  );
     
    //t_osc_bundle_s *getBundle();
    
    
    size_t getMapOSCSize();
    
    /**
     need to call getMapOSCSize() first to get size of OSC buffer
     then allocate the memory for the buffer your self
     then you can send the char * ptr to serializeIntoBuffer() to do the serialization,
     along with the size you got from the getMapOSCSize() call.
     */
    void serializeIntoBuffer(char * ptr, size_t size );
    

};

// std::vector<MapAtom>
