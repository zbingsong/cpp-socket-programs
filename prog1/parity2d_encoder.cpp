#include <cstring>
#include "parity2d_encoder.h"


const unsigned int Parity2DEncoder::SEG_LENGTH = 8;

char** Parity2DEncoder::partitionData(const char* data, const unsigned int segNum)
{
  char** partData = new char*[segNum+1];
  const char* copyHead = data;
  for (unsigned int i = 0; i < segNum; i ++) {
    char* head = new char[SEG_LENGTH+2];
    strncpy(head, copyHead, SEG_LENGTH);
    *(head + SEG_LENGTH) = '\0';
    *(partData + i) = head;
    copyHead += SEG_LENGTH;
  }
  return partData;
}

char** Parity2DEncoder::partitionCodeword(const char* codeword, const unsigned int segNum)
{
  char** partCodeword = new char*[segNum+1];
  for (unsigned int i = 0; i <= segNum; i ++) {
    char* head = new char[SEG_LENGTH+2];
    strncpy(head, codeword, SEG_LENGTH+1);
    *(head + SEG_LENGTH + 1) = '\0';
    *(partCodeword + i) = head;
    codeword += SEG_LENGTH+1;
  }
  return partCodeword;
}

char Parity2DEncoder::computeRowParity(const char* rowData)
{
  char parity = SEG_LENGTH % 2 == 0 ? 0 : '0';
  for (unsigned int j = 0; j < SEG_LENGTH; j ++) {
    parity ^= *(rowData + j);
  }
  return parity == 0 ? '0' : '1';
}

char Parity2DEncoder::computeColumnParity(
  const char* const* partData, const unsigned int colNum, const unsigned int segNum
)
{
  char parity = segNum % 2 == 0 ? 0 : '0';
  for (unsigned int i = 0; i < segNum; i ++) {
    parity ^= *(*(partData + i) + colNum);
  }
  return parity == 0 ? '0' : '1';
}

char* Parity2DEncoder::insertRowParities(char** partData, const unsigned int segNum)
{
  char* rowParities = new char[segNum+2];
  for (unsigned int i = 0; i < segNum; i ++) {
    char* head = *(partData + i);
    char parity = this->computeRowParity(head);
    *(head + SEG_LENGTH) = parity;
    *(head + SEG_LENGTH + 1) = '\0';
    *(rowParities + i) = parity;
  }
  *(rowParities + SEG_LENGTH) = this->computeColumnParity(partData, SEG_LENGTH, segNum);
  *(rowParities + SEG_LENGTH + 1) = '\0';
  return rowParities;
}

char* Parity2DEncoder::insertColumnParities(char** partData, const unsigned int segNum)
{
  char* columnParities = new char[SEG_LENGTH+2];
  for (unsigned int j = 0; j < SEG_LENGTH; j ++) {
    char parity = this->computeColumnParity(partData, j, segNum);
    *(columnParities + j) = parity;
  }
  *(columnParities + SEG_LENGTH) = this->computeRowParity(columnParities);
  *(columnParities + SEG_LENGTH + 1) = '\0';
  *(partData + segNum) = columnParities;
  return columnParities;
}

bool Parity2DEncoder::verifyRowParities(const char* const* partCodeword, const unsigned int segNum)
{
  for (unsigned int i = 0; i <= segNum; i ++) {
    const char* head = *(partCodeword + i);
    char parity = this->computeRowParity(head);
    if (parity != *(head + SEG_LENGTH)) {
      return false;
    }
  }
  return true;
}

bool Parity2DEncoder::verifyColumnParities(const char* const* partCodeword, const unsigned int segNum)
{
  const char* columnParities = *(partCodeword + segNum);
  for (unsigned int j = 0; j < SEG_LENGTH; j ++) {
    char parity = this->computeColumnParity(partCodeword, j, segNum);
    if (parity != *(columnParities + j)) {
      return false;
    }
  }
  return true;
}

char* Parity2DEncoder::reducePartDataToStr(const char* const* partData, const unsigned int segNum)
{
  char* strData = new char[(segNum+1) * (SEG_LENGTH+1) + 1];
  char* copyHead = strData;
  for (unsigned int i = 0; i <= segNum; i ++) {
    strcpy(copyHead, *(partData + i));
    copyHead += SEG_LENGTH + 1;
  }
  return strData;
}

// char* Parity2DEncoder::encode(const char* data)
// {
//   const unsigned int segNum = strlen(data) / SEG_LENGTH;
  
//   char** partData = this->partitionData(data, segNum);
//   this->insertRowParities(partData, segNum);
//   this->insertColumnParities(partData, segNum);    
//   char* codeword = this->reducePartDataToStr(partData, segNum);
  
//   delete[] partData;
//   partData = NULL;
//   return codeword;
// }

bool Parity2DEncoder::verify(const char* codeword)
{
  const unsigned int segNum = strlen(codeword) / (SEG_LENGTH+1) - 1;
  const char* const* partCodeword = this->partitionCodeword(codeword, segNum);
  bool result = this->verifyRowParities(partCodeword, segNum) 
    && this->verifyColumnParities(partCodeword, segNum);
  delete[] partCodeword;
  partCodeword = NULL;
  return result;
}
