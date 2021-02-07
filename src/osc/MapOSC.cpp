#include "MapOSC.hpp"

#include <netinet/in.h>
#include <inttypes.h>
#include <arpa/inet.h>

/*
#include "osc_bundle_iterator_s.h"
#include "osc_message_iterator_s.h"
#include "osc.h"
#include "osc_util.h"
//#include "osc_mem.h"
*/

#define OSC_HEADER_SIZE 16 // bundle\0 + 8 byte timetag.
#define OSC_IDENTIFIER "#bundle\0"
#define OSC_IDENTIFIER_SIZE 8
#define OSC_EMPTY_HEADER "#bundle\0\0\0\0\0\0\0\0\0"

#define OSC_ID OSC_IDENTIFIER
#define OSC_ID_SIZE OSC_IDENTIFIER_SIZE

#define OSC_BUNDLE_TYPETAG '.'
#define OSC_TIMETAG_TYPETAG 't'


#define OSC_BYTE_SWAP16(x) \
    ((((x) & 0xff00) >> 8) | \
     (((x) & 0x00ff) << 8))

#define OSC_BYTE_SWAP32(x) \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >> 8) | \
     (((x) & 0x0000ff00) << 8) | \
     (((x) & 0x000000ff) << 24))

#define OSC_BYTE_SWAP64(x) \
    ((((x) & 0xff00000000000000LL) >> 56) | \
     (((x) & 0x00ff000000000000LL) >> 40) | \
     (((x) & 0x0000ff0000000000LL) >> 24) | \
     (((x) & 0x000000ff00000000LL) >> 8)  | \
     (((x) & 0x00000000ff000000LL) << 8)  | \
     (((x) & 0x0000000000ff0000LL) << 24) | \
     (((x) & 0x000000000000ff00LL) << 40) | \
     (((x) & 0x00000000000000ffLL) << 56))

#define OSC_BYTE_SWAP128(x) \
    (uint128_t){OSC_BYTE_SWAP64(((x).l2)), OSC_BYTE_SWAP64(((x).l1))}

#ifdef BYTE_ORDER
#define OSC_BYTE_ORDER BYTE_ORDER
#else
#ifdef __BYTE_ORDER
#define OSC_BYTE_ORDER __BYTE_ORDER
#endif
#endif

#ifdef LITTLE_ENDIAN
#define OSC_LITTLE_ENDIAN LITTLE_ENDIAN
#else
#ifdef __LITTLE_ENDIAN
#define OSC_LITTLE_ENDIAN __LITTLE_ENDIAN
#endif
#endif

// need to handle the case where these are not defined

#if OSC_BYTE_ORDER == OSC_LITTLE_ENDIAN
#define hton16(x) OSC_BYTE_SWAP16(x)
#define ntoh16(x) OSC_BYTE_SWAP16(x)
#define hton32(x) OSC_BYTE_SWAP32(x)
#define ntoh32(x) OSC_BYTE_SWAP32(x)
#define hton64(x) OSC_BYTE_SWAP64(x)
#define ntoh64(x) OSC_BYTE_SWAP64(x)
#define hton128(x) OSC_BYTE_SWAP128(x)
#define ntoh128(x) OSC_BYTE_SWAP128(x)

#define htonf32(x) OSC_BYTE_SWAP32(*((uint32_t *)&x))
#define ntohf32(x) OSC_BYTE_SWAP32(*((uint32_t *)&x))
#define htonf64(x) OSC_BYTE_SWAP64(*((uint64_t *)&x))
#define ntohf64(x) OSC_BYTE_SWAP64(*((uint64_t *)&x))

#else

#define hton16(x) (x)
#define ntoh16(x) (x)
#define hton32(x) (x)
#define ntoh32(x) (x)
#define hton64(x) (x)
#define ntoh64(x) (x)
#define hton128(x) (x)
#define ntoh128(x) (x)

#define htonf32(x) (x)
#define ntohf32(x) (x)
#define htonf64(x) (x)
#define ntohf64(x) (x)

#endif

using namespace std;

size_t osc_util_getPaddedStringLen(char *s)
{
    if(!s){
        return 0;
    }
    size_t n = strlen(s);
    n = (n + 4) & 0xfffffffc;
    return n;
}

size_t osc_util_getPaddedStringLen(const string& s)
{
    size_t n = s.length();
    n = (n + 4) & 0xfffffffc;
    return n;
}


size_t osc_util_getPaddingForNBytes(size_t n)
{
    return (n + 4) & 0xfffffffc;
}


// typedef std::variant<float, double, int32_t, int64_t, char, bool, std::string> AtomVar_t;
char MapAtom::getOSCTypeTag()
{
    // index matches order of types in variant template
    switch (m_atom.index()) {
        case 0: return 'f';
        case 1: return 'd';
        case 2: return 'i';
        case 3: return 'h';
        case 4: return 'c';
        case 5: return (std::get<5>(m_atom) ? 'T' : 'F');
        case 6: return 's';
        default:
            printf("unknown type\n");
            return '?';
    }
}

size_t MapAtom::getSizeInBytes()
{
    switch (getOSCTypeTag())
     {
         case 'f':
         case 'c':
         case 'i':
            return 4;
         case 'd':
         case 'h':
            return 8;
         case 's':
         {
            size_t len = MapAtom::get<std::string>().length();
            size_t plen = osc_util_getPaddingForNBytes(len);
            return plen;
         }
         default:
             return 0;
    }
}



void MapOSC::inputOSC( long len, char * ptr )
{

    // MapOSC is a bundle, so we can add elements directly

    string buf(ptr);
    long _n = OSC_HEADER_SIZE;

    while( _n < len )
    {
        int32_t msg_size = ntoh32(*((int32_t *)(ptr + _n)));

        int32_t addr_start = _n + 4;
        int32_t addr_end = buf.find_first_of('\0', addr_start);
        string addr = buf.substr(addr_start, addr_end);


        int32_t typetags_start = addr_start + osc_util_getPaddedStringLen(addr);
        int32_t typetags_end = buf.find_first_of('\0', typetags_start);
        string typetags = buf.substr(typetags_start, typetags_end);

        int32_t data_start = typetags_start + osc_util_getPaddedStringLen(typetags);
        size_t bytes_to_next = 0;

        size_t natoms = typetags.length() - 1;

        MapOSCArray newVec;
        newVec.reserve( natoms );

        char * dataPtr = NULL;

        for( size_t i = 0; i < typetags.length(); i++)
        {
            dataPtr = ptr + data_start + bytes_to_next;

            switch ( typetags[i] )
            {
                case 'f':
                {
                    uint32_t i = ntoh32(*((uint32_t *)(dataPtr)));
                    newVec.appendValue( *((float *)&i) );
                    bytes_to_next = 4;
                }
                break;
                case 'd':
                {
                    uint64_t i = ntoh64(*((uint64_t *)(dataPtr)));
                    newVec.appendValue( *((double *)&i) );
                    bytes_to_next = 8;
                }
                break;
                case 'i':
                    newVec.appendValue( (int32_t)ntoh32(*((int32_t *)(dataPtr))) );
                    bytes_to_next = 4;
                    break;
                case 's':
                    {
                        size_t str_end = buf.find_first_of('\0', data_start + bytes_to_next);
                        string str = buf.substr(data_start + bytes_to_next, str_end);
                        newVec.appendValue( str );
                        bytes_to_next = osc_util_getPaddedStringLen(str);
                    }
                    break;
                case 'c':
                    newVec.appendValue((char)ntoh32(*((int32_t *)(dataPtr))));
                    bytes_to_next = 4;
                    break;
//                    case 'C':
//                        osc_atom_u_setUInt8(atom_u, osc_atom_s_getUInt8(a));
//                        break;
//                    case 'u':
//                        osc_atom_u_setInt16(atom_u, osc_atom_s_getInt16(a));
//                        break;
//                    case 'U':
//                        osc_atom_u_setUInt16(atom_u, osc_atom_s_getUInt16(a));
//                        break;
                case 'h':
                    newVec.appendValue( (int64_t)ntoh64(*((int64_t *)(dataPtr))) );
                    bytes_to_next = 8;
                    break;
//                    case 'I':
//                        osc_atom_u_setUInt32(atom_u, osc_atom_s_getUInt32(a));
//                        break;
//                    case 'H':
//                        osc_atom_u_setUInt64(atom_u, osc_atom_s_getUInt64(a));
//                        break;
                case 'T':
                    newVec.appendValue( true );
                    break;
                case 'F':
                    newVec.appendValue( false );
                    break;
                case 'N':
                    newVec.appendValue( false );
                    break;
//                    case OSC_BUNDLE_TYPETAG:
//                        {
//                            //char bndl[osc_bundle_s_getStructSize()];
//                            //t_osc_bndl_s *b = (t_osc_bndl_s *)bndl;
//                            t_osc_bndl_s *b = osc_atom_s_getBndl(a);
//                            if(b){
//                                osc_atom_u_setBndl(atom_u, osc_bundle_s_getLen(b), osc_bundle_s_getPtr(b));
//                                osc_bundle_s_free(b);
//                            }
//                        }
//                        break;
//                    case OSC_TIMETAG_TYPETAG:
//                        {
//                            osc_atom_u_setTimetag(atom_u, osc_atom_s_getTimetag(a));
//                        }
//                        break;
//                    case 'b':
//                        osc_atom_u_setBlob(atom_u, a->data);
//                        break;
//                    case OSC_BUNDLE_TYPETAG:
//                        return getBundle() == src.getBundle();
                default:
                break;

            }
        }

        map.emplace( addr, newVec );

        _n += msg_size + 4;
    }

/*
    t_osc_bndl_it_s *it = osc_bndl_it_s_get(len, ptr);
    while(osc_bndl_it_s_hasNext(it)){
        t_osc_msg_s *m = osc_bndl_it_s_next(it);
                    
        MapOSCArray newVec;
        newVec.reserve( osc_message_s_getArgCount(m) );
        
        t_osc_msg_it_s *mit = osc_msg_it_s_get(m);
        
        int i = 0;
        while(osc_msg_it_s_hasNext(mit)){
            t_osc_atom_s *a = osc_msg_it_s_next(mit);

            switch ( osc_atom_s_getTypetag(a) )
            {
                case 'f':
                    newVec.appendValue( osc_atom_s_getFloat(a) );
                    break;
                case 'd':
                    newVec.appendValue( osc_atom_s_getDouble(a) );
                    break;
                case 'i':
                    newVec.appendValue( osc_atom_s_getInt32(a) );
                    break;
                case 's':
                    {
                        //char buf[ strlen(osc_atom_s_getData(a)) ];
                        //char *ptr = buf;
                        //osc_atom_s_getString(a, 0, &ptr); // allocates memory and makes a copy
//                        printf("test %s \n", std::string(osc_atom_s_getData(a)).c_str() );
                        newVec.appendValue( std::string(osc_atom_s_getData(a)) );
                        
                     //   osc_mem_free(buf);
                    }
                    break;
                case 'c':
                    newVec.appendValue((char)osc_atom_s_getInt8(a));
                    break;
//                    case 'C':
//                        osc_atom_u_setUInt8(atom_u, osc_atom_s_getUInt8(a));
//                        break;
//                    case 'u':
//                        osc_atom_u_setInt16(atom_u, osc_atom_s_getInt16(a));
//                        break;
//                    case 'U':
//                        osc_atom_u_setUInt16(atom_u, osc_atom_s_getUInt16(a));
//                        break;
                case 'h':
                    newVec.appendValue( osc_atom_s_getInt64(a) );
                    break;
//                    case 'I':
//                        osc_atom_u_setUInt32(atom_u, osc_atom_s_getUInt32(a));
//                        break;
//                    case 'H':
//                        osc_atom_u_setUInt64(atom_u, osc_atom_s_getUInt64(a));
//                        break;
                case 'T':
                    newVec.appendValue( true );
                    break;
                case 'F':
                    newVec.appendValue( false );
                    break;
                case 'N':
                    newVec.appendValue( false );
                    break;
//                    case OSC_BUNDLE_TYPETAG:
//                        {
//                            //char bndl[osc_bundle_s_getStructSize()];
//                            //t_osc_bndl_s *b = (t_osc_bndl_s *)bndl;
//                            t_osc_bndl_s *b = osc_atom_s_getBndl(a);
//                            if(b){
//                                osc_atom_u_setBndl(atom_u, osc_bundle_s_getLen(b), osc_bundle_s_getPtr(b));
//                                osc_bundle_s_free(b);
//                            }
//                        }
//                        break;
//                    case OSC_TIMETAG_TYPETAG:
//                        {
//                            osc_atom_u_setTimetag(atom_u, osc_atom_s_getTimetag(a));
//                        }
//                        break;
//                    case 'b':
//                        osc_atom_u_setBlob(atom_u, a->data);
//                        break;
//                    case OSC_BUNDLE_TYPETAG:
//                        return getBundle() == src.getBundle();
                default:
                break;
                    
            }
            
            i++;
        }
        osc_msg_it_s_destroy(mit);
        
        map.emplace( std::string(osc_message_s_getAddress(m)), newVec );
    }
    osc_bndl_it_s_destroy(it);
    */
}



size_t MapOSC::getMapOSCSize()
{
    size_t _n = OSC_HEADER_SIZE;
    
    // iterate messages in map
    for (auto& it : map)
    {
        _n += 4;
        _n += osc_util_getPaddedStringLen( (char *)it.first.c_str() );
        _n += osc_util_getPaddingForNBytes( it.second.size() + 1 );
        
        for( auto& at : it.second.getAtomVector() )
        {
            _n += at.getSizeInBytes();
        }
     
    }
    
    return _n;
}



size_t serializeVector( char *buf, size_t remaining_size, const char * address, std::vector<MapAtom>& vec )
{
    size_t _n = 0;
    size_t addresslen = strlen(address);
    size_t padded_address_len = osc_util_getPaddingForNBytes(addresslen);
    size_t padded_typetag_len = osc_util_getPaddingForNBytes(vec.size() + 1);
    size_t num_bytes_before_data = 4 + padded_address_len + padded_typetag_len;

    if(remaining_size < num_bytes_before_data){
        return 0;
    }
    _n += num_bytes_before_data;
    
    char *ptr = buf;
    memset(ptr, '\0', num_bytes_before_data);
    ptr += 4;
    
    memcpy(ptr, address, addresslen);
    ptr += padded_address_len;
    
    char *ttptr = ptr;
    ptr += padded_typetag_len;
    *ttptr++ = ',';
    
    for( auto& at : vec)
    {
        /*
       if( !(ttptr < (buf + n) ) )
       {
           printf("pointer out of range? %ld size(%ld) \n", n, sizeof(ttptr) );
           break; // throw error probably
           
       }
        */
        
        *ttptr++ = at.getOSCTypeTag();
        ptr = buf + _n;
        
        switch (at.getOSCTypeTag()) {
            case 'f':
            {
                float f = at.get<float>();
                *((int32_t *)ptr) = hton32(*((int32_t *)(&f)));
                _n += 4;
            }
                break;
            case 'd':
            {
                double d = at.get<double>();
                *((int64_t *)ptr) = hton64(*((int64_t *)(&d)));
                _n += 8;
            }
                break;
            case 'c':
            {
                char c = at.get<char>();
                *((int32_t *)ptr) = hton32(c);
                _n += 4;
            }
                break;
            case 'i':
            {
                int32_t i = at.get<int32_t>();
                *((int32_t *)ptr) = hton32(i);
                _n += 4;
            }
                break;
            case 'h':
            {
                int64_t l = at.get<int64_t>();
                *((int64_t *)ptr) = hton64(l);
                _n += 8;
            }
            case 's':
            {
                const string& str = at.get<std::string>().c_str();
                size_t len = str.length();
                size_t plen = osc_util_getPaddingForNBytes(len);
                memset(ptr, '\0', plen);
                memcpy(ptr, str.c_str(), len);
                _n += plen;
            }
            default:
                break;
        }
        
    }
    
    *((int32_t *)buf) = hton32((int32_t)_n - 4);
    
    return _n;
}

/*
t_osc_bundle_s* MapOSC::getBundle()
{
    size_t len = getMapOSCSize();
    //printf("allocating %ld bytes \n", len);
    char * ptr = (char *)osc_mem_alloc(len);
    
    size_t _n = 0;
    
    memcpy(ptr, OSC_EMPTY_HEADER, OSC_HEADER_SIZE);
    _n += OSC_HEADER_SIZE;
    
    for (auto& it : map)
    {
        _n += serializeVector(ptr + _n, len - _n, it.first.c_str(), it.second.getAtomVector() );
    }
    
   // printf("_n %ld \n", _n);

    return osc_bundle_s_alloc(_n, ptr);
   
}
 */

void MapOSC::serializeIntoBuffer(char *ptr, size_t size )
{
    size_t _n = 0;
    
    memset(ptr, '\0', size);
    memcpy(ptr, OSC_EMPTY_HEADER, OSC_HEADER_SIZE);
    _n += OSC_HEADER_SIZE;
    
    for (auto& it : map)
    {
        _n += serializeVector(ptr + _n,
                              size - _n,
                              it.first.c_str(), //address
                              it.second.getAtomVector() //vec
                              );
    }
    
   // printf("_n %ld \n", _n);
   
}



