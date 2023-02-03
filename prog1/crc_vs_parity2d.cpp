#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include "crc_encoder.h"
#include "parity2d_encoder.h"


void addError(char* codeword, const char* errorBits)
{
  while (*codeword != '\0' && *errorBits != '\0') {
    *codeword = (*codeword == *errorBits) ? '0' : '1';
    codeword ++;
    errorBits ++;
  }
}

void parityCheck(Parity2DEncoder* parity2DEncoder, const char* data, const char* errorBits, int & outLen)
{
  const unsigned int segNum = strlen(data) / Parity2DEncoder::SEG_LENGTH;
  char** partData = parity2DEncoder->partitionData(data, segNum);
  const char* rowParities = parity2DEncoder->insertRowParities(partData, segNum);
  const char* columnParities = parity2DEncoder->insertColumnParities(partData, segNum);
  char* codeword = parity2DEncoder->reducePartDataToStr(partData, segNum);

  std::stringstream ss;
  ss << "2D Parity: Col: " << columnParities << "; Row: " << rowParities << ";    ";
  std::string output = ss.str();
  outLen = output.length();

  addError(codeword, errorBits);
  bool result = parity2DEncoder->verify(codeword);
  std::cout << output << "Result: " << (result ? "Pass" : "Not Pass") << std::endl;

  delete[] partData;
  delete[] rowParities;
  delete[] columnParities;
  delete[] codeword;
}

void crcCheck(CrcEncoder* crcEncoder, const char* data, const char* errorBits, const int & outLen)
{
  const char* remainder = crcEncoder->getRemainder(data);
  char* codeword = crcEncoder->getCodeword(data, remainder);

  std::stringstream ss;
  ss << "CRC: " << remainder << ";" << std::string(outLen - 6 - strlen(remainder), ' ');
  std::string output = ss.str();

  addError(codeword, errorBits);
  bool result = crcEncoder->verifyCodeword(codeword);
  std::cout << output << "Result: " << (result ? "Pass" : "Not Pass") << std::endl;

  delete[] remainder;
  delete[] codeword;
}

int main()
{
  std::string filename = "dataVs.txt";
  std::ifstream inFile(filename.c_str());
  CrcEncoder* crcEncoder = new CrcEncoder();
  Parity2DEncoder* parity2DEncoder = new Parity2DEncoder();
  int outLen;
  std::string line;
  std::istringstream iss;
  std::string d, err;
  while (!inFile.eof()) {
    std::getline(inFile, line);
    iss.str(line);
    iss >> d >> err;
    iss.clear();
    const char* data = d.c_str();
    const char* errorBits = err.c_str();

    parityCheck(parity2DEncoder, data, errorBits, outLen);
    crcCheck(crcEncoder, data, errorBits, outLen);
    std::cout << std::string(26, '=') << std::endl;
  }
}
