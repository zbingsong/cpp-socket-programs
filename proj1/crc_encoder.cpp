#include <cstring>
#include "crc_encoder.h"


const char* CrcEncoder::GENERATOR = "10001000000100001";
const unsigned int CrcEncoder::R_LENGTH = strlen(CrcEncoder::GENERATOR) - 1;


char* CrcEncoder::getRemainder(const char* data)
{
  // copy and pads data with trailing zeros
  char* paddedData = this->copyAndPadData(data);
  // set a pointer to the start of padded data
  char* divideHead = paddedData;
  // performs division
  // division WILL modify paddedData
  while (strlen(divideHead) > R_LENGTH) {
    this->divideStep(divideHead);
  }
  // what's left from division is the remainder
  char* remainder = new char[R_LENGTH + 1];
  strcpy(remainder, divideHead);

  // clean up and return remainder
  delete[] paddedData;
  paddedData = NULL;
  divideHead = NULL;
  return remainder;
}


char* CrcEncoder::getCodeword(const char* data, const char* remainder)
{
  // copy data into a new c string that's big enough to include
  // encoded data
  char* codeword = new char[strlen(data) + R_LENGTH + 1];
  strcpy(codeword, data);
  // concatenate remainder to data (encoding)
  strcat(codeword, remainder);
  return codeword;
}


bool CrcEncoder::verifyCodeword(const char* codeword)
{
  // extract original data from codeword
  unsigned int dataSize = strlen(codeword) - R_LENGTH;
  char* data = new char[dataSize + 1];
  strncpy(data, codeword, dataSize);
  *(data + dataSize) = '\0';
  // get computed remainder from extracted data
  char* calcRemainder = this->getRemainder(data);
  // compare computed remainder with remainder from codeword
  bool result = (strcmp(calcRemainder, codeword + dataSize) == 0);

  // clean up and return comparison result
  delete[] data;
  delete[] calcRemainder;
  data = NULL;
  calcRemainder = NULL;
  return result;
}
  

void CrcEncoder::divideStep(char* & divideHead)
{
  // if divideHead points to '0', do nothing and move it to the 
  // next char
  // if points to '1', do XOR between GENERATOR and the part of
  // padded data between divideHead and (divideHead + R_LENGTH)
  // this part of data is directly replaced by remainder of this
  // division
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
  // create a new c string that's big enough to include data
  // and padded zeros
  char* paddedData = new char[strlen(data) + R_LENGTH + 1];
  // copy data over to the new c string
  strcpy(paddedData, data);
  // pads data with zeros of length R_LENGTH
  memset(paddedData + strlen(data), '0', R_LENGTH);
  *(paddedData + strlen(data) + R_LENGTH) = '\0';
  return paddedData;
}
