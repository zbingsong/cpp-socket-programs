#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include "crc_encoder.h"
#include "parity2d_encoder.h"


/**
 * Induce errors in codeword according to content of errorBits;
 * will modify codeword
 * 
 * @param codeword  the initially correct codeword, to which errors
 * will be induced on
 * @param errorBits the string of error bits to induce errors
*/
void addError(char* codeword, const char* errorBits)
{
  // while neither c string ends
  while (*codeword != '\0' && *errorBits != '\0') {
    // induce error, doing XOR between char of codeword and of errorBits
    *codeword = (*codeword == *errorBits) ? '0' : '1';
    // increment both pointers to their respective next char
    // both pointers are passed by value (copied) so we can directly
    // increment them without forgetting the initial positions
    codeword ++;
    errorBits ++;
  }
}

/**
 * Encode the data with 2D parity, print the parities, induce errors, 
 * and check if the encoder can detect the errors
 * 
 * @param parity2DEncoder the encoder for 2D parity
 * @param data  the binary string to be encoded
 * @param errorBits the error bits to induce errors
 * @param outLen  an integer that record the number of chars in printed
 * output, for aligning outputs from 2D parity and CRC
*/
void parityCheck(Parity2DEncoder* parity2DEncoder, const char* data, const char* errorBits, int & outLen)
{
  // get number of segments in data if data is partitioned into 8-bit segments
  const unsigned int segNum = strlen(data) / Parity2DEncoder::SEG_LENGTH;
  // partition the data into segments
  char** partData = parity2DEncoder->partitionData(data, segNum);
  // insert row and column parity bits into data, get these parity bits
  const char* rowParities = parity2DEncoder->insertRowParities(partData, segNum);
  const char* columnParities = parity2DEncoder->insertColumnParities(partData, segNum);
  // get the codeword from partitioned data (now containing parity bits)
  char* codeword = parity2DEncoder->reducePartDataToStr(partData, segNum);

  // use a stringstream to build printout of parities
  std::stringstream ss;
  ss << "2D Parity: Col: " << columnParities << "; Row: " << rowParities << ";    ";
  std::string output = ss.str();
  // count the length of the printout for alignment purposes
  outLen = output.length();

  // induce errors on codeword
  addError(codeword, errorBits);
  // verify the codeword with errors
  bool result = parity2DEncoder->verify(codeword);
  // print parities and verification result
  std::cout << output << "Result: " << (result ? "Pass" : "Not Pass") << std::endl;

  // clean up
  delete[] partData;
  delete[] rowParities;
  delete[] columnParities;
  delete[] codeword;
}

/**
 * Encode the data with CRC, print the parities, induce errors, 
 * and check if the encoder can detect the errors
 * 
 * @param crcEncoder the encoder for CRC
 * @param data  the binary string to be encoded
 * @param errorBits the error bits to induce errors
 * @param outLen  an integer that record the number of chars in printed
 * output, for aligning outputs from 2D parity and CRC
*/
void crcCheck(CrcEncoder* crcEncoder, const char* data, const char* errorBits, const int & outLen)
{
  // get remainder and encoded data
  const char* remainder = crcEncoder->getRemainder(data);
  char* codeword = crcEncoder->getCodeword(data, remainder);

  // build printout
  std::stringstream ss;
  // append (outLen - 6 - strlen(remainder)) spaces to make printout 
  // align with that of 2D parity
  ss << "CRC: " << remainder << ";" << std::string(outLen - 6 - strlen(remainder), ' ');
  std::string output = ss.str();

  // induce errors and verify the codeword with errors
  addError(codeword, errorBits);
  bool result = crcEncoder->verifyCodeword(codeword);
  // output results
  std::cout << output << "Result: " << (result ? "Pass" : "Not Pass") << std::endl;

  // clean up
  delete[] remainder;
  delete[] codeword;
}

/// @brief run the program, read file inputs and perform encoding and verifications
int main()
{
  // read data
  std::string filename = "dataVs.txt";
  std::ifstream inFile(filename.c_str());
  // create encoders for CRC and 2D parity
  CrcEncoder* crcEncoder = new CrcEncoder();
  Parity2DEncoder* parity2DEncoder = new Parity2DEncoder();
  // store length of printout
  int outLen;
  // store data line by line
  std::string line;
  // get data and error bits using istringstream and two strings
  std::istringstream iss;
  std::string d, err;

  while (!inFile.eof()) {
    // get a line of data file
    std::getline(inFile, line);
    // put data string to 'd', put error bits to 'err'
    iss.str(line);
    iss >> d >> err;
    iss.clear();
    // convert data and error bits to c string
    const char* data = d.c_str();
    const char* errorBits = err.c_str();

    // do encoding and verification for both 2D parity and CRC
    parityCheck(parity2DEncoder, data, errorBits, outLen);
    crcCheck(crcEncoder, data, errorBits, outLen);
    // separator for readability
    std::cout << std::string(26, '=') << std::endl;
  }
}
