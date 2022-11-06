#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <type_traits>


#define WITH_EIGEN

#ifdef WITH_EIGEN
#include <Eigen/Dense>
#endif

#define OSC_BUNDLE_TYPETAG '.'
#define OSC_TIMETAG_TYPETAG 't'

class MapOSC;

class OSCAtom
{

public:
    OSCAtom(float val )         : type('f'), f_val(val) {}
    OSCAtom(double val )        : type('d'), d_val(val) {}
    OSCAtom(int32_t val )       : type('i'), i_val(val) {}
    OSCAtom(int64_t val )       : type('h'), l_val(val) {}
    OSCAtom(char val )          : type('c'), c_val(val) {}
    OSCAtom(bool val )          : type('b'), b_val(val) {}
    OSCAtom(const std::string val )  : type('s'), str_val(val) {}
    OSCAtom(const char * val )  : type('s'), str_val(val) {}
    OSCAtom(const MapOSC & val) : type('.'), map_val( std::make_unique<MapOSC>( val ) ) {}
    OSCAtom(const OSCAtom & other);
    ~OSCAtom(){}

    MapOSC newMap();

    template <typename T> T get();

    inline int getInt() { return (int)get<int32_t>(); }
    inline float getFloat(){ return (float)get<float>(); }

    inline  char typetag() {
       if( type == 'b' )
           return b_val ? 'T' : 'F';
       else
           return type;
    }

    size_t getSizeInBytes();


private:

    const char type;

    union {
        float f_val;
        double d_val;
        int32_t i_val;
        int64_t l_val;
        char c_val;
        bool b_val;

    };

    std::unique_ptr<MapOSC> map_val;

    std::string str_val;

};


class OSCAtomVector
{

public:

    OSCAtomVector(){}
    OSCAtomVector( std::vector< std::unique_ptr<OSCAtom> > vec );

    OSCAtomVector( const OSCAtomVector & other );

    OSCAtomVector( std::vector< OSCAtom > vec )
    {
        for( auto & v : vec )
            obj_vec.emplace_back(std::make_unique<OSCAtom>(v));
    }
    OSCAtomVector& operator=(const OSCAtomVector& other)
    {
        if (this != &other) // not a self-assignment
        {
            if (obj_vec.size() != other.obj_vec.size()) // resource cannot be reused
            {
                for( size_t i = 0; i < other.obj_vec.size(); i++ )
                {
                    obj_vec.emplace_back( std::make_unique<OSCAtom>( *other.obj_vec[i] ) );
                }
            }
        }
        return *this;
    }
        
    OSCAtomVector(float val ) {   obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(double val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(int32_t val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(int64_t val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(char val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(const char * val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(const std::string& val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    OSCAtomVector(const MapOSC & val ) {obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }

    inline void clear(){
        obj_vec.clear();
    }

    inline void appendValue(float val ) {   obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(double val ) {  obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(int32_t val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(int64_t val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(char val ) {    obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(const char * val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(const std::string& val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(bool val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }
    inline void appendValue(const MapOSC & val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>(val)); }

    inline void appendValue(size_t val ) { obj_vec.emplace_back(std::make_unique<OSCAtom>((int32_t)val)); }


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


#ifdef WITH_EIGEN
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
#endif


    template <typename T>
    T get(size_t idx = 0) const {
        return obj_vec[idx]->get<T>();
    }

    int getInt()const { return get<int>(); }
    float getFloat()const { return get<float>(); }


    char typetag(size_t idx = 0) const {
        return obj_vec[idx]->typetag();
    }

    OSCAtom& operator[](size_t idx) { return *obj_vec[idx]; }

    inline const std::vector< std::unique_ptr<OSCAtom> > & getAtomVector() const { return obj_vec; }

    inline std::vector<char> typetags() const
    {
        std::vector<char> tags;
        for( auto& it : obj_vec )
        {
            tags.emplace_back( it->typetag() );
        }
        return tags;
    }

    inline void reserve(size_t size){ obj_vec.reserve(size); }
    inline size_t size() const { return obj_vec.size(); }

    void print(int tabs = 0) const;


private:

    std::vector< std::unique_ptr<OSCAtom> > obj_vec;

};


class MapOSC
{

public:

    MapOSC(){}
    MapOSC( const MapOSC & other );
    MapOSC( long len, char * ptr ) { inputOSC(len, ptr); }
    /*
    MapOSC (std::unordered_map<std::string, OSCAtomVector > ){
        printf("test");
    }
    */
    void inputOSC( long len, char * ptr );

    inline void addMessage(const char* address)
    {
        if (!address_lookup.count(address))
            address_lookup.emplace(address, OSCAtomVector());
    }

    template <typename... Ts>
    void addMessage (const std::string& address, Ts&&... args)
    {
     //   using expand = int[];


        if( !address_lookup.count(address) )
            address_lookup.emplace(address, OSCAtomVector());
        else
           address_lookup[address].clear();

        (address_lookup[address].appendValue( std::forward<Ts>(args) ), ...);
//        (void)expand{0, ((void)address_lookup[address].appendValue( std::forward<Ts>(args) ), 0) ... }; // (pre-c++17)

    }

    template <typename... Ts>
    void addMessage (const char * address, Ts&&... args)
    {
       // using expand = int[];

        if( !address_lookup.count(address) )
            address_lookup.emplace(address, OSCAtomVector());
        else
           address_lookup[address].clear();

        (address_lookup[address].appendValue( std::forward<Ts>(args) ), ...);
//        (void)expand{0, ((void)address_lookup[address].appendValue( std::forward<Ts>(args) ), 0) ... }; // (pre-c++17)
    }

    OSCAtomVector& operator[](std::string& addr) { return address_lookup[addr]; }
    OSCAtomVector& operator[](const char * addr) { return address_lookup[addr]; }

    const OSCAtomVector& at(const char * addr) const { return address_lookup.at(addr); }


    inline bool addressExists(const char * address) {
        return address_lookup.count(address);
    }

    inline OSCAtomVector& getMessage(const char * address) {
       return address_lookup[address];
    }

    std::string getSerializedString() const;

    size_t getSerializedSizeInBytes() const;
    void serializeIntoBuffer(char *ptr, size_t size ) const;

    void print(int tabs = 0) const;

    const std::unordered_map<std::string, OSCAtomVector >& getMap(){ return address_lookup; }

    inline void reserve(size_t n){ address_lookup.reserve(n); }
    inline size_t size() const { return address_lookup.size(); }

private:

    std::unordered_map<std::string, OSCAtomVector > address_lookup;

};
