#include "MosaikString.h"

// constructor
CMosaikString::CMosaikString(void)
: mData(NULL)
, mAllocatedLength(0)
, mLength(0)
{}

// destructor
CMosaikString::~CMosaikString(void) {
	if(mData) delete [] mData;
}

// copy constructor
CMosaikString::CMosaikString(const CMosaikString& ms) 
: mData(NULL)
, mAllocatedLength(ms.mLength + GROWTH_FACTOR)
, mLength(ms.mLength)
{
	mData = new char[mAllocatedLength];
	memcpy(mData, ms.mData, mLength);
	mData[mLength] = 0;
}

// copy constructor
CMosaikString::CMosaikString(const char* c_str)
: mData(NULL)
, mAllocatedLength(0)
, mLength(0)
{
	// don't assign a null character or self assigned string
	if(!c_str || (mData == c_str)) return;

	// copy the string
	mLength = strlen(c_str);
	mAllocatedLength = mLength + GROWTH_FACTOR;
	mData = new char[mAllocatedLength];
	memcpy(mData, c_str, mLength);
	mData[mLength] = 0;
}

// copy constructor
CMosaikString::CMosaikString(const char* c_str, const unsigned int len)
: mData(NULL)
, mAllocatedLength(0)
, mLength(0)
{
	// don't assign a null character or self assigned string
	if(!c_str || (mData == c_str)) return;

	// copy the string
	mLength = len;
	mAllocatedLength = len + GROWTH_FACTOR;
	mData = new char[mAllocatedLength];
	memcpy(mData, c_str, len);
	mData[len] = 0;
}

// assignment operator
CMosaikString& CMosaikString::operator=(const CMosaikString& ms) {

	// return if this is a self assignment
	if(this == &ms) return *this;

	// skip strings that are zero in length
	if(ms.mLength == 0) {
		mLength = 0;
		return *this;
	}

	// copy the string
	Reserve(ms.mLength);
	mLength = ms.mLength;
	memcpy(mData, ms.mData, mLength);
	mData[mLength] = 0;

	return *this;
}

// assignment operator
CMosaikString& CMosaikString::operator=(const char* c_str) {

	// don't assign a null character or self assigned string
	if(!c_str || (mData == c_str)) return *this;

	// copy the string
	const unsigned short numBytes = strlen(c_str);
	Reserve(numBytes);
	mLength = numBytes;
	memcpy(mData, c_str, numBytes);
	mData[numBytes] = 0;

	return *this;
}

// less than operator
bool CMosaikString::operator<(const CMosaikString& ms) const {
	if((mLength == 0) || (ms.mLength == 0)) return true;
	if(strcmp(mData, ms.mData) < 0)         return true;
	return false;
}

// not equal operator
bool CMosaikString::operator!=(const CMosaikString& ms) const {
	if((mLength == 0) || (ms.mLength == 0)) return true;
	if(strcmp(mData, ms.mData) != 0)        return true;
	return false;
}

// equal operator
bool CMosaikString::operator==(const CMosaikString& ms) const {
	if((mLength == 0) || (ms.mLength == 0)) return false;
	if(strcmp(mData, ms.mData) == 0)        return true;
	return false;
}

// element operator
char& CMosaikString::operator[](const int index) {
	return mData[index];
}

std::ostream& operator<<(std::ostream& out, const CMosaikString& ms) {
	if(ms.mLength == 0) return out;
	return out << ms.mData;
}

// appends the specified string to the current string
void CMosaikString::Append(const char* s) {

	// check the allocated room
	const unsigned int suffixLength  = strlen(s);
	const unsigned int currentLength = mLength;
	unsigned int newLength = suffixLength + currentLength;

	if((newLength + 1) > mAllocatedLength) {

		// save the old string
		char* newData = new char[currentLength + 1];
		memcpy(newData, mData, currentLength);
		newData[currentLength] = 0;

		// copy the old string
		Reserve(newLength);
		memcpy(mData, newData, currentLength);

		// clean up
		delete [] newData;

	}

	// copy the suffix
	memcpy(mData + currentLength, s, suffixLength);

	mLength = newLength;
	mData[newLength] = 0;
}

// copies the specified c-style string
void CMosaikString::Copy(const char* string, const unsigned int numBytes) {
	Reserve(numBytes);
	memcpy(mData, string, numBytes);
	mData[numBytes] = 0;
	mLength = numBytes;
}

// returns a pointer to the data
char* CMosaikString::Data(void) {
	return mData;
}

// returns a const pointer to the data
const char* CMosaikString::CData(void) const {
	return (const char*)mData;
}

// decrements each character in the string by the specified amount
void CMosaikString::Decrement(const char amount) {
	for(unsigned int i = 0; i < mLength; i++) mData[i] -= amount;
}

// fills the string with numBytes copies of the ch
void CMosaikString::Fill(const char ch, const unsigned int numBytes) {
	Reserve(numBytes);
	memset(mData, ch, numBytes);
	mData[numBytes] = 0;
	mLength = numBytes;
}

// returns the hash value
size_t CMosaikString::GetHash(void) const {

	// N.B. caching the hash value doesn't help
	// Hsieh hash: slightly faster than Java string hash
	size_t hash = 0;
	if(mLength == 0) return hash;

	const char* data = mData;
	unsigned int len = mLength;

	unsigned int rem = len & 3;
	len >>= 2;

	unsigned int tmp;
	for(; len > 0; len--) {
		hash  += get16bits(data);
		tmp    = (get16bits(data + 2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 4;
		hash  += hash >> 11;
	}

	switch(rem) {
		case 3:
			hash += get16bits(data);
			hash ^= hash << 16;
			hash ^= data[2] << 18;
			hash += hash >> 11;
			break;
		case 2:
			hash += get16bits(data);
			hash ^= hash << 11;
			hash += hash >> 17;
			break;
		case 1:
			hash += *data;
			hash ^= hash << 10;
			hash += hash >> 1;
	}

	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

// increments each character in the string by the specified amount
void CMosaikString::Increment(const char amount) {
	for(unsigned int i = 0; i < mLength; i++) mData[i] += amount;
}

// joins two strings (used by the Smith-Waterman caching algorithm)
void CMosaikString::Join(const char* s1, const unsigned int s1Length, const char* s2, const unsigned int s2Length) {

	const unsigned int JOINED_LENGTH = s1Length + s2Length + 1;
	Reserve(JOINED_LENGTH);

	unsigned int bufferOffset = s1Length;
	memcpy(mData, s1, s1Length);
	mData[bufferOffset++] = '+';
	memcpy(mData + bufferOffset, s2, s2Length);

	mLength = JOINED_LENGTH;
	mData[mLength] = 0;
}

// returns the size of the data
unsigned int CMosaikString::Length(void) const {
	return mLength;
}

// prepends the specified string to the current string
void CMosaikString::Prepend(const CMosaikString& ms) {

	// check the allocated room
	const unsigned int prefixLength  = ms.mLength;
	const unsigned int currentLength = mLength;
	unsigned int newLength = prefixLength + currentLength;

	if((newLength + 1) > mAllocatedLength) {

		// save the old string
		char* newData = new char[currentLength + 1];
		memcpy(newData, mData, currentLength);
		newData[currentLength] = 0;

		// copy the prefix
		Reserve(newLength);
		memcpy(mData, ms.mData, prefixLength);

		// copy the old string
		memcpy(mData + prefixLength, newData, currentLength);

		// clean up
		delete [] newData;

	} else {

		memmove_s(mData + prefixLength, mAllocatedLength - prefixLength, mData, currentLength);
		memcpy(mData, ms.mData, prefixLength);
	}

	mLength = newLength;
	mData[newLength] = 0;
}

// removes all occurrences of the specified character
void CMosaikString::Remove(const char ch) {

	unsigned int currentRemovePosition = 0;
	for(unsigned int i = 0; i < mLength; i++) {

		// copy the characters if the coordinates are different
		if(i != currentRemovePosition) mData[currentRemovePosition] = mData[i];

		// increment the remove position
		if(mData[i] != ch) currentRemovePosition++;
	}

	mLength = currentRemovePosition;
	mData[mLength] = 0;
}

// replaces all occurrences of the first parameter with the second parameter
void CMosaikString::Replace(const char oldCh, const char newCh) {
	for(unsigned int i = 0; i < mLength; i++) {
		if(mData[i] == oldCh) mData[i] = newCh;
	}
}

// reserve the specified number of bytes
void CMosaikString::Reserve(const unsigned int numBytes) {

	if((numBytes + 1) > mAllocatedLength) {
		mAllocatedLength = numBytes + GROWTH_FACTOR + 1;
		if(mData) delete [] mData;
		mData = new char[mAllocatedLength];
	}

	// reset the data
	mLength = 0;
	mData[0] = 0;
}

// reverses the contents of the string
void CMosaikString::Reverse(void) {
	for(unsigned int y = mLength; y >= (mLength / 2) + 1; y--)
		swapByte(mData[mLength - y], mData[y - 1]);
}

// sets the length to the specified size
void CMosaikString::SetLength(const unsigned int length) {

	if(mAllocatedLength == 0) {
		mLength = 0;
		return;
	}

	if(length < mAllocatedLength) mLength = length;
	else mLength = mAllocatedLength - 1;
	mData[mLength] = 0;
}

// trims the first specified number of bytes
void CMosaikString::TrimBegin(unsigned int numBytes) {

	if(numBytes > mLength) {
		mLength = 0;
		mData[0] = 0;
		return;
	}

	unsigned int newLength = mLength - numBytes;
	memmove_s(mData, mAllocatedLength, mData + numBytes, newLength);
	mLength = newLength;
	mData[mLength] = 0;
}

// trims the last specified number of bytes
void CMosaikString::TrimEnd(unsigned int numBytes) {

	if(numBytes > mLength) {
		mLength = 0;
		mData[0] = 0;
		return;
	}

	mLength = mLength - numBytes;
	mData[mLength] = 0;
}

// converts the string to uppercase
void CMosaikString::Uppercase(void) {
	for(unsigned int i = 0; i < mLength; i++) mData[i] = toupper(mData[i]);
}
