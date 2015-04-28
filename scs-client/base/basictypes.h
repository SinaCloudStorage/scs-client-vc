#ifndef BASE_BASICTYPES_H_
#define BASE_BASICTYPES_H_


#include <limits.h>         // So we can set the bounds of our types
#include <stddef.h>         // For size_t
#include <string.h>         // for memcpy

#include <string>

typedef std::string AnsiString;
typedef std::wstring UnicodeString;

#ifdef _UNICODE
typedef UnicodeString String;
#else
typedef AnsiString String;
#endif // _UNICODE



#define GG_INT64(x)  x##I64
#define GG_UINT64(x) x##UI64

typedef signed char         schar;
typedef signed char         int8;
typedef short               int16;
typedef int                 int32;
typedef long long           int64;

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

const uint8  kuint8max  = (( uint8) 0xFF);
const uint16 kuint16max = ((uint16) 0xFFFF);
const uint32 kuint32max = ((uint32) 0xFFFFFFFF);
const uint64 kuint64max = ((uint64) GG_INT64(0xFFFFFFFFFFFFFFFF));
const  int8  kint8min   = ((  int8) 0x80);
const  int8  kint8max   = ((  int8) 0x7F);
const  int16 kint16min  = (( int16) 0x8000);
const  int16 kint16max  = (( int16) 0x7FFF);
const  int32 kint32min  = (( int32) 0x80000000);
const  int32 kint32max  = (( int32) 0x7FFFFFFF);
const  int64 kint64min  = (( int64) GG_INT64(0x8000000000000000));
const  int64 kint64max  = (( int64) GG_INT64(0x7FFFFFFFFFFFFFFF));


template <class Dest, class Source>
inline Dest bit_cast(const Source& source)
{
  // Compile time assertion: sizeof(Dest) == sizeof(Source)
  // A compile error here means your Dest and Source have different sizes.
  typedef char VerifySizesAreEqual [sizeof(Dest) == sizeof(Source) ? 1 : -1];

  Dest dest;
  memcpy(&dest, &source, sizeof(dest));
  return dest;
}

typedef wchar_t char16;
typedef std::wstring string16;

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#endif  // BASE_BASICTYPES_H_
