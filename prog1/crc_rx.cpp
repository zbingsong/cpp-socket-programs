#include <iostream>
#include <fstream>
#include <string>
#include "crc_encoder.h"

int main()
{
  std::string filename = "dataRx.txt";
  std::ifstream inFile(filename.c_str());
  CrcEncoder* encoder = new CrcEncoder();
  std::string line;
  std::string result;
  while (std::getline(inFile, line)) {
    const char* codeword = line.c_str();
    result = encoder->verifyCodeword(codeword) ? "Pass" : "Not Pass";
    std::cout << result << std::endl;
    //delete[] codeword;
  }
}
