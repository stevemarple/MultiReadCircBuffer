#include <iostream>
#include <fstream>

#include "MultiReadCircBuffer.h"

using namespace std;
/*
const char* crusoe = "I was born in the year 1632, in the city of York, of a good family, "
"though not of that country, my father being a foreigner of Bremen, who "
"settled first at Hull.  He got a good estate by merchandise, and leaving "
"off his trade, lived afterwards at York, from whence he had married my "
"mother, whose relations were named Robinson, a very good family in that "
"country, and from whom I was called Robinson Kreutznaer; but, by the "
"usual corruption of words in England, we are now called - nay we call "
"ourselves and write our name - Crusoe; and so my companions always called "
"me."
""
"I had two elder brothers, one of whom was lieutenant-colonel to an "
"English regiment of foot in Flanders, formerly commanded by the famous "
"Colonel Lockhart, and was killed at the battle near Dunkirk against the "
"Spaniards.  What became of my second brother I never knew, any more than "
"my father or mother knew what became of me."
""
"Being the third son of the family and not bred to any trade, my head "
"began to be filled very early with rambling thoughts.  My father, who was "
"very ancient, had given me a competent share of learning, as far as "
"house-education and a country free school generally go, and designed me "
"for the law; but I would be satisfied with nothing but going to sea; and "
"my inclination to this led me so strongly against the will, nay, the "
"commands of my father, and against all the entreaties and persuasions of "
"my mother and other friends, that there seemed to be something fatal in "
"that propensity of nature, tending directly to the life of misery which "
"was to befall me."
""
"My father, a wise and grave man, gave me serious and excellent counsel "
"against what he foresaw was my design.  He called me one morning into his "
"chamber, where he was confined by the gout, and expostulated very warmly "
"with me upon this subject.  He asked me what reasons, more than a mere "
"wandering inclination, I had for leaving father's house and my native "
"country, where I might be well introduced, and had a prospect of raising "
"my fortune by application and industry, with a life of ease and pleasure. "
"He told me it was men of desperate fortunes on one hand, or of aspiring, "
"superior fortunes on the other, who went abroad upon adventures, to rise "
"by enterprise, and make themselves famous in undertakings of a nature out "
"of the common road; that these things were all either too far above me or "
"too far below me; that mine was the middle state, or what might be called "
"the upper station of low life, which he had found, by long experience, "
"was the best state in the world, the most suited to human happiness, not "
"exposed to the miseries and hardships, the labour and sufferings of the "
"mechanic part of mankind, and not embarrassed with the pride, luxury, "
"ambition, and envy of the upper part of mankind.  He told me I might "
"judge of the happiness of this state by this one thing - viz. that this was "
"the state of life which all other people envied; that kings have "
"frequently lamented the miserable consequence of being born to great "
"things, and wished they had been placed in the middle of the two "
"extremes, between the mean and the great; that the wise man gave his "
"testimony to this, as the standard of felicity, when he prayed to have "
"neither poverty nor riches."
"";
*/

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
  //uint8_t buffer[bufferLen];
  uint8_t* buffer = new uint8_t[bufferLen];
  //uint8_t* buffer = (uint8_t*)malloc(sizeof(uint8_t) * bufferLen);
  //MultiReadCircBuffer circBuff(buffer, bufferLen, false);
  const uint8_t numReaders = 3;
  //int sizes[numReaders];
  int* sizes = new int[numReaders];
  //uint8_t* readPtrs[numReaders];
  uint8_t** readPtrs = new uint8_t*[numReaders];
  
  MultiReadCircBuffer circBuff(&buffer[0], bufferLen, true, 16,
			       numReaders, sizes, readPtrs);

  circBuff.status(__FILE__, __LINE__);
  // char *p = (char*)crusoe;
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

