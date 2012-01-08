#ifndef MULTIREADCIRCBUFFER_H
#define MULTIREADCIRCBUFFER_H

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

class MultiReadCircBuffer {
public:
  MultiReadCircBuffer(void* myBuffer, int myBufferLen, bool myAllowOverwrite);
  MultiReadCircBuffer(void* myBuffer, int myBufferLen,
		 bool myAllowOverwrite, uint8_t myNumReaders,
		 int* mySizes, uint8_t** myReadPtrs);

  inline bool getAllowOverwrite(void) const;
  inline int getCapacity(void) const;
  inline int getNumReaders(void) const;
  inline int getSize(uint8_t reader = 0) const;

  // @return number of bytes written
  int write(const uint8_t* src, int srcLen);
  int write(const uint8_t* src, int srcLen, bool& dataOverWritten);
  inline int write(const void* src, int srcLen);
  inline int write(const void* src, int srcLen, bool& dataOverWritten);

  // @return number of bytes actually read
  int read(uint8_t* dest, int destLen, uint8_t reader = 0);
  inline int read(void* dest, int destLen, uint8_t reader = 0);

#ifdef DEBUG
  inline void status(const char *file = NULL, int line = 0) const;
#endif
  
private:
  uint8_t* buffer;
  int bufferLen;
  bool allowOverwrite;
  uint8_t* volatile writePtr;
  uint8_t numReaders;
  int* volatile sizes;
  uint8_t** volatile readPtrs;
  // For simple case where numReaders == 1 don't require external arrays
  int size0;
  uint8_t* readPtr0; 

  inline const uint8_t* getEndOfBuffer(void) const;
  inline int getBytesBeforeWrap(uint8_t* ptr) const;

  // Declare but do not define, copying not permitted
  MultiReadCircBuffer(MultiReadCircBuffer const &a);
  const MultiReadCircBuffer& operator=(MultiReadCircBuffer const &a);
};

inline bool MultiReadCircBuffer::getAllowOverwrite(void) const
{
  return allowOverwrite;
}


inline int MultiReadCircBuffer::getCapacity(void) const
{
  return bufferLen;
}


inline int MultiReadCircBuffer::getNumReaders(void) const
{
  return numReaders;
}


inline int MultiReadCircBuffer::getSize(uint8_t reader) const
{
  if (reader >= numReaders)
    return -1;
  return sizes[reader];
}


inline const uint8_t* MultiReadCircBuffer::getEndOfBuffer(void) const
{
  return buffer + bufferLen;
}


inline int MultiReadCircBuffer::getBytesBeforeWrap(uint8_t* ptr) const
{
  return getEndOfBuffer() - ptr;
}

inline int MultiReadCircBuffer::write(const void* src, int srcLen)
{
  return write((const uint8_t*)src, srcLen);
}


inline int MultiReadCircBuffer::write(const void* src, int srcLen,
				      bool& dataOverWritten)
{
  return write((const uint8_t*)src, srcLen, dataOverWritten);
}


inline int MultiReadCircBuffer::read(void* dest, int destLen, uint8_t reader)
{
  return read((uint8_t*)dest, destLen, reader);
}


#ifdef DEBUG
inline void MultiReadCircBuffer::status(const char *file, int line) const
{
  cout << "--------------- ";
  if (file != NULL) {
    cout << "File: " << file;
    if (line)
      cout << ':' << line;
  }
  cout << endl;

  cout << "Capacity:        " << bufferLen << endl
       << "Allow overwrite: " << allowOverwrite << endl
       << "Num readers:     " << (int)numReaders << endl;
  for (int i = 0; i < numReaders; ++i) {
    cout << '#' << i << " size: " << sizes[i]
	 << ", ptr: " << (readPtrs[i]-buffer) << endl;
  }
  cout << "write ptr: " << (writePtr-buffer) << endl;
}
#endif



#endif
