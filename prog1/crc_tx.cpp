#include <iostream>
#include <fstream>
#include <string>
#include "crc_encoder.h"

int main()
{
  // read the file
  std::string filename = "dataTx.txt";
  std::ifstream inFile(filename.c_str());
  // create a new crc encoder
  CrcEncoder* encoder = new CrcEncoder();
  // read file line by line and store content in this string variable
  std::string line;
  while (std::getline(inFile, line)) {
    const char* data = line.c_str();
    // get remainder and encoded string
    const char* remainder = encoder->getRemainder(data);
    const char* codeword = encoder->getCodeword(data, remainder);
    // output result
    std::cout << "Codeword is:\n" << codeword << "\nCRC is\n" << remainder << "\n" << std::endl;
  }
}
