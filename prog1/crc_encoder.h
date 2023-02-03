/**
 * A class for encoding a binary string with CRC and verifying the 
 * integrity of encoding of a coded binary string
 * 
 * PRECONDITION:
 * 1. a binary string to be encoded is at least 16 bits long
 * 2. a binary string to be verified is at least 32 bits long
*/
class CrcEncoder {
  public:
    /// @brief the CRC generator to be used (17 bits)
    static const char* GENERATOR;
    /// @brief the length of remainder R (16 bits), one less than generator length
    static const unsigned int R_LENGTH;
    /**
     * Generate the remainder of a binary string; does not modify the string
     * 
     * PRECONDITION:
     * 1. the binary string is at least 16 chars long
     * 
     * @param data  the binary string whose remainder will be taken
     * @return  a pointer to the c string containing the remainder, 16 chars 
     * not including the terminating null character
    */
    char* getRemainder(const char* data);
    /**
     * Encode a binary string with CRC given its remainder; does not modify the
     * original string
     * 
     * @param data  the binary string to be encoded
     * @param remainder the remainder of the binary string
     * @return  a pointer to the c string containing the encoded binary string,
     * at least 32 chars not including the terminating null character
    */
    char* getCodeword(const char* data, const char* remainder);
    bool verifyCodeword(const char* codeword);
  
  private:
    void divideStep(char* & divideHead);
    char* copyAndPadData(const char* data);
};
