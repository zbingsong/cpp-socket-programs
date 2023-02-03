#include <cstring>
#include "crc_encoder.h"

const char* CrcEncoder::GENERATOR = "10001000000100001";
const unsigned int CrcEncoder::R_LENGTH = strlen(CrcEncoder::GENERATOR) - 1;

char* CrcEncoder::getRemainder(const char* data)
{ 
  char* paddedData = this->copyAndPadData(data);
  char* divideHead = paddedData;
  while (strlen(divideHead) > R_LENGTH) {
    this->divideStep(divideHead);
  }
  char* remainder = new char[R_LENGTH + 1];
  strcpy(remainder, divideHead);

  delete[] paddedData;
  paddedData = NULL;
  divideHead = NULL;

  return remainder;
}

char* CrcEncoder::getCodeword(const char* data, const char* remainder)
{
  char* codeword = new char[strlen(data) + R_LENGTH + 1];
  strcpy(codeword, data);
  strcat(codeword, remainder);
  return codeword;
}

bool CrcEncoder::verifyCodeword(const char* codeword)
{
  unsigned int dataSize = strlen(codeword) - R_LENGTH;
  char* data = new char[dataSize + 1];
  strncpy(data, codeword, dataSize);
  *(data + dataSize) = '\0';
  char* calcRemainder = this->getRemainder(data);
  bool result = (strcmp(calcRemainder, codeword + dataSize) == 0);
  delete[] data;
  delete[] calcRemainder;
  data = NULL;
  calcRemainder = NULL;
  return result;
}
  
void CrcEncoder::divideStep(char* & divideHead)
{
  if (*divideHead != '0') {
    for (unsigned int i = 0; i <= R_LENGTH; i ++) {
      if (*(divideHead + i) == *(GENERATOR + i)) {
        *(divideHead + i) = '0';
      } else {
        *(divideHead + i) = '1';
      }
    }
  }
  divideHead ++;
}

char* CrcEncoder::copyAndPadData(const char* data)
{
  char* paddedData = new char[strlen(data) + R_LENGTH + 1];
  strcpy(paddedData, data);
  memset(paddedData + strlen(data), '0', R_LENGTH);
  *(paddedData + strlen(data) + R_LENGTH) = '\0';
  return paddedData;
}
