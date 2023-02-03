#include <iostream>
#include <fstream>
#include <string>
#include "crc_encoder.h"

int main()
{
  std::string filename = "dataTx.txt";
  std::ifstream inFile(filename.c_str());
  CrcEncoder* encoder = new CrcEncoder();
  std::string line;
  while (std::getline(inFile, line)) {
    const char* data = line.c_str();
    const char* remainder = encoder->getRemainder(data);
    const char* codeword = encoder->getCodeword(data, remainder);
    std::cout << "Codeword is:\n" << codeword << "\nCRC is\n" << remainder << "\n" << std::endl;
    //delete data;
    //delete remainder;
    //delete codeword;
    //std::cout << "end of round" << std::endl;
  }
}
