#include <cstring>
#include "parity2d_encoder.h"


const unsigned int Parity2DEncoder::SEG_LENGTH = 8;


char** Parity2DEncoder::partitionData(const char* data, const unsigned int segNum)
{
  // create an array of char* pointers of length (segNum+1)
  // this '+1' is to accommodate column parities added later
  char** partData = new char*[segNum+1];
  // copy each SEG_LENGTH chars in data into a new c string
  // associate that c string with a pointer in partData
  for (unsigned int i = 0; i < segNum; i ++) {
    // '+2' b/c need one extra space for row parity bit
    // another one extra space for null character
    char* head = new char[SEG_LENGTH+2];
    strncpy(head, data, SEG_LENGTH);
    // for now, set '\0' to second last space b/c don't have
    // row parity yet
    *(head + SEG_LENGTH) = '\0';
    *(partData + i) = head;
    // increment copy head pointer
    data += SEG_LENGTH;
  }
  // dimension of partData should now be (segNum+1) * (SEG_LENGTH+2)
  // last row has no c string associated with it yet
  // second last column is all '\0'
  // last column is all empty (unassigned)
  return partData;
}

// similar to above method, except all rows and columns are filled
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
  // if there are even number of bits in row, set parity to 00000000
  // otherwise, set it to '0' (00110010)
  // equivalent to appending an extra '0' if odd number of bits
  // if don't do this, XORing even and odd num of bits give different
  // binary result (binary value of 'parity'), difficult to keep track of
  char parity = SEG_LENGTH % 2 == 0 ? 0 : '0';
  // XOR all chars in the row
  for (unsigned int j = 0; j < SEG_LENGTH; j ++) {
    parity ^= *(rowData + j);
  }
  // if even num of '1', should get 00000000 for 'parity'
  return parity == 0 ? '0' : '1';
}

// similar to above method
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
  // record all row parity bits in a c string
  char* rowParities = new char[segNum+2];
  // compute and append row parity bit to all rows, change position of
  // null character
  for (unsigned int i = 0; i < segNum; i ++) {
    char* head = *(partData + i);
    char parity = this->computeRowParity(head);
    *(head + SEG_LENGTH) = parity;
    *(head + SEG_LENGTH + 1) = '\0';
    *(rowParities + i) = parity;
  }
  // compute but not append the whole parity bit
  *(rowParities + SEG_LENGTH) = this->computeColumnParity(partData, SEG_LENGTH, segNum);
  *(rowParities + SEG_LENGTH + 1) = '\0';
  return rowParities;
}

// similar to above method, except that column parities are store in c string
// and attached to partitioned data at the end
char* Parity2DEncoder::insertColumnParities(char** partData, const unsigned int segNum)
{
  char* columnParities = new char[SEG_LENGTH+2];
  for (unsigned int j = 0; j < SEG_LENGTH; j ++) {
    char parity = this->computeColumnParity(partData, j, segNum);
    *(columnParities + j) = parity;
  }
  // compute and append the whole parity bit
  *(columnParities + SEG_LENGTH) = this->computeRowParity(columnParities);
  *(columnParities + SEG_LENGTH + 1) = '\0';
  *(partData + segNum) = columnParities;
  return columnParities;
}


bool Parity2DEncoder::verifyRowParities(const char* const* partCodeword, const unsigned int segNum)
{
  // for each row (including column parity row), compute its row parity
  // and compare against what's in codeword
  for (unsigned int i = 0; i <= segNum; i ++) {
    const char* head = *(partCodeword + i);
    char parity = this->computeRowParity(head);
    if (parity != *(head + SEG_LENGTH)) {
      return false;
    }
  }
  return true;
}

// similar to above method, except column parity of the column of row parities
// are not checked
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
  // allocate c string large enough to hold encoded data
  char* strData = new char[(segNum+1) * (SEG_LENGTH+1) + 1];
  // copy each row in partData into strData
  // null character is also copied, and gets overwritten by data for all
  // rows except last row
  char* copyHead = strData;
  for (unsigned int i = 0; i <= segNum; i ++) {
    strcpy(copyHead, *(partData + i));
    copyHead += SEG_LENGTH + 1;
  }
  return strData;
}


bool Parity2DEncoder::verify(const char* codeword)
{
  const unsigned int segNum = strlen(codeword) / (SEG_LENGTH+1) - 1;
  // partition codeword and check all row parity and column parity bits
  const char* const* partCodeword = this->partitionCodeword(codeword, segNum);
  bool result = this->verifyRowParities(partCodeword, segNum) 
    && this->verifyColumnParities(partCodeword, segNum);
  // clean up and return result
  delete[] partCodeword;
  partCodeword = NULL;
  return result;
}
