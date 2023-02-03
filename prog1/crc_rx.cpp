#include <iostream>
#include <fstream>
#include <string>
#include "crc_encoder.h"

int main()
{
  // read the data file
  std::string filename = "dataRx.txt";
  std::ifstream inFile(filename.c_str());
  // create a new crc encoder
  CrcEncoder* encoder = new CrcEncoder();
  // store file content on each line in this variable
  std::string line;
  // read file line by line
  while (std::getline(inFile, line)) {
    const char* codeword = line.c_str();
    // output verification result
    // encoder->verifyCodeword() returns a boolean
    std::cout << (encoder->verifyCodeword(codeword) ? "Pass" : "Not Pass") << std::endl;
  }
}
