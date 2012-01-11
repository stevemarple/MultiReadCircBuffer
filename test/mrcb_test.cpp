#include <iostream>
#include <fstream>

#include "MultiReadCircBuffer.h"
using namespace std;

int writeBuf(MultiReadCircBuffer &buf, void *src, int srcLen)
{
  int n = buf.write(src, srcLen);
  cout << "Wrote " << n << " of " << srcLen << " bytes" << endl;
  return n;
}

int readBuf(MultiReadCircBuffer &buf, int len, uint8_t reader)
{
  char *cp = new char[len];

  int n = buf.read(cp, len, reader);
  cout << '#' << (int)reader <<  " read " << n << " of " << len
       << " bytes, \"";
  for (int i = 0; i < n; ++i)
    cout << cp[i];
  cout << '"' << endl;
  delete[] cp;
  return n;
}

const char* readFile(const char* filename)
{
  ifstream file;
  file.open (filename);
  if (file.is_open()) {
    // get length of file:
    file.seekg (0, ios::end);
    int len = file.tellg();
    file.seekg (0, ios::beg);

    char* s = new char[len];
    file.read(s, len);
    file.close();
    return s;
  }
  else {
    cout << "Unable to open file";
    exit(1);
    return NULL;
  }
}

int main(void)
{
  const int bufferLen = 256;
  uint8_t* buffer = new uint8_t[bufferLen];
  const uint8_t numReaders = 3;
  int* sizes = new int[numReaders];
  uint8_t** readPtrs = new uint8_t*[numReaders];
  
  MultiReadCircBuffer circBuff(&buffer[0], bufferLen, true, false, 16,
			       numReaders, sizes, readPtrs);

  circBuff.status(__FILE__, __LINE__);
  const char* crusoe = readFile("robinson_crusoe.bin"); 
  char *p = (char*)crusoe; 
  
  p += writeBuf(circBuff, p, 100);
  circBuff.status(__FILE__, __LINE__);

  readBuf(circBuff, 200, 0);
  readBuf(circBuff, 100, 1);
  readBuf(circBuff, 80, 2);
  circBuff.status(__FILE__, __LINE__);
  
  readBuf(circBuff, 200, 0);
  readBuf(circBuff, 100, 1);
  readBuf(circBuff, 80, 2);
  circBuff.status(__FILE__, __LINE__);
  
  p += writeBuf(circBuff, p, 300);
  circBuff.status(__FILE__, __LINE__);

  readBuf(circBuff, 200, 0);
  readBuf(circBuff, 100, 1);
  readBuf(circBuff, 80, 2);
  circBuff.status(__FILE__, __LINE__);

  p += writeBuf(circBuff, p, 100);
  circBuff.status(__FILE__, __LINE__);
  readBuf(circBuff, 300, 0);
  readBuf(circBuff, 300, 1);
  readBuf(circBuff, 300, 2);
  circBuff.status(__FILE__, __LINE__);

  cout << "p: " << int(p - crusoe) << endl;
  cout << "done" << endl;
  return 0;
}

