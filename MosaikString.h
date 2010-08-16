#pragma once

#include <ostream>
#ifndef WIN32
#include <tr1/unordered_map>
#else
#include <unordered_map>
#endif
#include <stdio.h>
#include <string.h>
#include "SafeFunctions.h"

#define GROWTH_FACTOR 10

typedef unsigned short uint16_t;
#define swapByte(a, b) { char c = (a); (a) = (b); (b) = c; }
#define get16bits(d) (*((const uint16_t*)(d)))

class CMosaikString {
public:
	// constructor
	CMosaikString(void);
	// copy constructor
	CMosaikString(const CMosaikString& ms);
	// copy constructor
	CMosaikString(const char* c_str);
	// copy constructor
	CMosaikString(const char* c_str, const unsigned int len);
	// destructor
	~CMosaikString(void);
	// assignment operator
	CMosaikString& operator=(const CMosaikString& ms);
	// assignment operator
	CMosaikString& operator=(const char* c_str);
	// less than operator
	bool operator<(const CMosaikString& c) const;
	// not equal operator
	bool operator!=(const CMosaikString& c) const;
	// equal operator
	bool operator==(const CMosaikString& c) const;
	// element operator
	char& operator[](const int index);
	// appends the specified string to the current string
	void Append(const char* s);
	// returns a const pointer to the data
	const char* CData(void) const;
	// copies the specified c-style string
	void Copy(const char* string, const unsigned int numBytes);
	// returns a pointer to the data
	char* Data(void);
	// decrements each character in the string by the specified amount
	void Decrement(const char amount);
	// fills the string with numBytes copies of the ch
	void Fill(const char ch, const unsigned int numBytes);
	// returns the hash value
	size_t GetHash(void) const;
	// increments each character in the string by the specified amount
	void Increment(const char amount);
	// joins two strings (used by the Smith-Waterman caching algorithm)
	void Join(const char* s1, const unsigned int s1Length, const char* s2, const unsigned int s2Length);
	// returns the size of the data
	unsigned int Length(void) const;
	// prepends the specified string to the current string
	void Prepend(const CMosaikString& ms);
	// replaces all occurrences of the first parameter with the second parameter
	void Replace(const char oldCh, const char newCh);
	// removes all occurrences of the specified character
	void Remove(const char ch);
	// reserve the specified number of bytes (destructive)
	void Reserve(const unsigned int numBytes);
	// reverses the contents of the string
	void Reverse(void);
	// sets the length to the specified size
	void SetLength(const unsigned int length);
	// trims the first specified number of bytes
	void TrimBegin(unsigned int numBytes);
	// trims the last specified number of bytes
	void TrimEnd(unsigned int numBytes);
	// converts the string to uppercase
	void Uppercase(void);
	// output operators
	friend std::ostream& operator<<(std::ostream& out, const CMosaikString& ms);

private:
	// our underlying data
	char* mData;
	// the allocated length of the string
	unsigned int mAllocatedLength;
	// the string length
	unsigned int mLength;
};

// add the hashing routines to STL TR1
namespace std {
	namespace tr1 {
		template<>
		struct hash<CMosaikString> : public std::unary_function<CMosaikString, std::size_t> {
			std::size_t operator()(CMosaikString ms) const {
				return ms.GetHash();
			}
		};
	}
}
