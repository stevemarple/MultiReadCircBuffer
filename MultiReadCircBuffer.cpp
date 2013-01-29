#include <MultiReadCircBuffer.h>
#include <string.h>

MultiReadCircBuffer::MultiReadCircBuffer(void* myBuffer, int myBufferLen,
					 bool myAllowOverwrite,
					 bool myUseInterrupts, int myBlockSize)
{
  buffer = (uint8_t*)myBuffer;
  bufferLen = myBufferLen;
  numReaders = 1;
  readPtrs = &readPtr0;
  writePtr = buffer;
  readPtrs[0] = buffer;
  sizes = &size0;
  sizes[0] = 0;
  allowOverwrite = myAllowOverwrite;
  useInterrupts = myUseInterrupts;
  if (myBlockSize > 1)
    blockSize = myBlockSize;
  else
    blockSize = 0;
}


MultiReadCircBuffer::MultiReadCircBuffer(void* myBuffer, int myBufferLen,
					 bool myAllowOverwrite,
					 bool myUseInterrupts, int myBlockSize,
					 uint8_t myNumReaders, int* mySizes,
					 uint8_t** myReadPtrs)
{
  buffer = (uint8_t*)myBuffer;
  bufferLen = myBufferLen;
  numReaders = myNumReaders;
  readPtrs = (uint8_t**)myReadPtrs;
  writePtr = buffer;
  sizes = mySizes;
  for (int i = 0; i < numReaders; ++i) {
    readPtrs[i] = buffer;
    sizes[i] = 0;
  }
  allowOverwrite = myAllowOverwrite;
  useInterrupts = myUseInterrupts;
  if (myBlockSize > 1)
    blockSize = myBlockSize;
  else
    blockSize = 0;
}


int MultiReadCircBuffer::write(const uint8_t* src, int srcLen)
{
  bool overwritten;
  return write(src, srcLen, overwritten);
}


int MultiReadCircBuffer::write(const uint8_t* src, int srcLen,
			       bool& overwritten)
{
  boolean intEn = interruptsEnabled();
  int len = srcLen; // The length to copy

  if (blockSize)
    len -= (len % blockSize); // Round down len to a multiple of the blockSize

  // The length to return. Normally the same as the number of bytes
  // copied, except when len > bufferLen.
  int rlen = len; 
  
  // Assume overwritten is false (always the case when allowOverwrite
  // is false)
  overwritten = false;

  if (allowOverwrite) { 
    if (len > bufferLen) {
      // Pretend to write all data but only actually write the last
      // capacity bytes. This case always 'overwrites' data.
      overwritten = true;
      len = bufferLen;
      src += (srcLen - len);
    }
    if (useInterrupts)
      noInterrupts();
  }
  else {
    // Reduce len to smallest size that can be written without
    // overwriting any data. This section must run with interrupts
    // off.
    if (useInterrupts)
      noInterrupts();
    for (int i = 0; i < numReaders; ++i) {
      int n = bufferLen - getSize(i);
      if (n < len)
	rlen = len = n;
    }
    if (useInterrupts && intEn)
      interrupts();
  }
  
  // Copy up until the end of the buffer (or sooner)
  int bbw = getBytesBeforeWrap(writePtr);
  int len1 = (len < bbw ? len : bbw);
  memcpy(writePtr, src, len1);
  if (len1 == bbw)
    writePtr = buffer; // Must wrap pointer
  else
    writePtr += len1;

  // Copy the remainder from the start of the buffer
  if (len > bbw) {
    int len2 = len - bbw;
    memcpy(writePtr, src + len1, len2);
    writePtr = buffer + len2;
  }

  // Update sizes and readPtrs
  if (useInterrupts)
    noInterrupts(); // no interrupts even for allowOverwrite == false
  for (int i = 0; i < numReaders; ++i)
    if ((sizes[i] += len) > bufferLen) {
      overwritten = true;
      readPtrs[i] = writePtr;
      sizes[i] = bufferLen;
    }
  
  // Restore interrupt status
  if (useInterrupts && intEn)
    interrupts();
  return rlen;
}


int MultiReadCircBuffer::read(uint8_t* dest, int destLen, uint8_t reader)
{
  if (blockSize)
    // Round down destLen to a multiple of the blockSize
    destLen -= (destLen % blockSize); 

  // If allowOverwrite == false then only call to getSize() must run
  // with interrupts turned off. Otherwise whole function must run
  // with interrupts turned off.
  boolean intEn = interruptsEnabled();
  if (useInterrupts)
    noInterrupts();
  int size = getSize(reader); 

  if (useInterrupts && allowOverwrite == false && intEn)
    interrupts();
  
  if (destLen > size)
    destLen = size;

  // Read up until the end of the buffer
  int bbw = getBytesBeforeWrap(readPtrs[reader]);
  int len1 = (destLen < bbw ? destLen : bbw);
  memcpy(dest, readPtrs[reader], len1);
  if (len1 == bbw)
    readPtrs[reader] = buffer; // Must wrap pointer
  else
    readPtrs[reader] += len1;
  
  // Copy the remainder
  if (destLen > bbw) {
    int len2 = destLen - bbw;
    memcpy(dest + len1, readPtrs[reader], len2);
    readPtrs[reader] += len2;
  }

  // Update sizes and readPtrs
  if (useInterrupts)
    noInterrupts();
  sizes[reader] -= destLen;
  
  if (useInterrupts && intEn)
    interrupts();
  return destLen;
}


int MultiReadCircBuffer::read(Print& stream, int destLen, uint8_t reader)
{
  if (blockSize)
    // Round down destLen to a multiple of the blockSize
    destLen -= (destLen % blockSize); 

  // If allowOverwrite == false then only call to getSize() must run
  // with interrupts turned off. Otherwise whole function must run
  // with interrupts turned off.
  boolean intEn = interruptsEnabled();
  if (useInterrupts)
    noInterrupts();
  int size = getSize(reader); 

  if (useInterrupts && allowOverwrite == false && intEn)
    interrupts();
  
  if (destLen > size)
    destLen = size;

  // Read up until the end of the buffer
  int bbw = getBytesBeforeWrap(readPtrs[reader]);
  int len1 = (destLen < bbw ? destLen : bbw);
  // ***** memcpy(dest, readPtrs[reader], len1);
  stream.write(readPtrs[reader], len1);
  if (len1 == bbw)
    readPtrs[reader] = buffer; // Must wrap pointer
  else
    readPtrs[reader] += len1;
  
  // Copy the remainder
  if (destLen > bbw) {
    int len2 = destLen - bbw;
    // ***** memcpy(dest + len1, readPtrs[reader], len2);
    stream.write(readPtrs[reader], len2);
    readPtrs[reader] += len2;
  }

  // Update sizes and readPtrs
  if (useInterrupts)
    noInterrupts();
  sizes[reader] -= destLen;
  
  if (useInterrupts && intEn)
    interrupts();
  return destLen;
}


int MultiReadCircBuffer::peek(uint8_t* dest, int destLen, uint8_t reader)
{
  boolean intEn = interruptsEnabled();
  if (useInterrupts)
    noInterrupts();
  uint8_t* rptr = readPtrs[reader];
  int size = sizes[reader];
  int r = read(dest, destLen, reader);

  // Restore read values
  readPtrs[reader] = rptr;
  sizes[reader] = size;
  
  if (intEn)
    interrupts();
  return r;
}

int MultiReadCircBuffer::skip(int len, uint8_t reader)
{
  boolean intEn = interruptsEnabled();
  if (useInterrupts)
    noInterrupts();

  if (len > sizes[reader])
    len = sizes[reader];
      
  readPtrs[reader] += len;
  if (readPtrs[reader] >= getEndOfBuffer())
    readPtrs[reader] -= bufferLen;
  sizes[reader] -= len;
  
  if (intEn)
    interrupts();
  return len;
}
