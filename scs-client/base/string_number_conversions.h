#ifndef BASE_STRING_NUMBER_CONVERSIONS_H_
#define BASE_STRING_NUMBER_CONVERSIONS_H_

#include <string>
#include <vector>

#include "base/basictypes.h"

// Number -> string conversions ------------------------------------------------

std::string IntToString(int value);
string16 IntToString16(int value);

std::string UintToString(unsigned value);
string16 UintToString16(unsigned value);

std::string Int64ToString(int64 value);
string16 Int64ToString16(int64 value);

std::string Uint64ToString(uint64 value);
string16 Uint64ToString16(uint64 value);

// String -> number conversions ------------------------------------------------

// Perform a best-effort conversion of the input string to a numeric type,
// setting |*output| to the result of the conversion.  Returns true for
// "perfect" conversions; returns false in the following cases:
//  - Overflow/underflow.  |*output| will be set to the maximum value supported
//    by the data type.
//  - Trailing characters in the string after parsing the number.  |*output|
//    will be set to the value of the number that was parsed.
//  - Leading whitespace in the string before parsing the number. |*output| will
//    be set to the value of the number that was parsed.
//  - No characters parseable as a number at the beginning of the string.
//    |*output| will be set to 0.
//  - Empty string.  |*output| will be set to 0.
bool StringToInt(const std::string& input, int* output);
bool StringToInt(std::string::const_iterator begin,
                 std::string::const_iterator end,
                 int* output);
bool StringToInt(const char* begin, const char* end, int* output);

bool StringToInt(const string16& input, int* output);
bool StringToInt(string16::const_iterator begin,
                 string16::const_iterator end,
                 int* output);
bool StringToInt(const char16* begin, const char16* end, int* output);

bool StringToInt64(const std::string& input, int64* output);
bool StringToInt64(std::string::const_iterator begin,
                   std::string::const_iterator end,
                   int64* output);
bool StringToInt64(const char* begin, const char* end, int64* output);

bool StringToInt64(const string16& input, int64* output);
bool StringToInt64(string16::const_iterator begin,
                   string16::const_iterator end,
                   int64* output);
bool StringToInt64(const char16* begin, const char16* end, int64* output);

// Hex encoding ----------------------------------------------------------------

// Returns a hex string representation of a binary buffer. The returned hex
// string will be in upper case. This function does not check if |size| is
// within reasonable limits since it's written with trusted data in mind.  If
// you suspect that the data you want to format might be large, the absolute
// max size for |size| should be is
//   std::numeric_limits<size_t>::max() / 2
std::string HexEncode(const void* bytes, size_t size);

// Best effort conversion, see StringToInt above for restrictions.
bool HexStringToInt(const std::string& input, int* output);
bool HexStringToInt(std::string::const_iterator begin,
                    std::string::const_iterator end,
                    int* output);
bool HexStringToInt(const char* begin, const char* end, int* output);

// Similar to the previous functions, except that output is a vector of bytes.
// |*output| will contain as many bytes as were successfully parsed prior to the
// error.  There is no overflow, but input.size() must be evenly divisible by 2.
// Leading 0x or +/- are not allowed.
bool HexStringToBytes(const std::string& input, std::vector<uint8>* output);

#endif  // BASE_STRING_NUMBER_CONVERSIONS_H_

