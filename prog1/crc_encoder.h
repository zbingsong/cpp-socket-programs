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
     * 1. the binary string is at least R_LENGTH chars long
     * 
     * @param data  the binary string as c string whose remainder will be taken
     * @return  a pointer to the c string containing the remainder, R_LENGTH chars 
     * not including the terminating null character
    */
    char* getRemainder(const char* data);
    /**
     * Encode a binary string with CRC given its remainder; does not modify the
     * original string
     * 
     * @param data  the binary string to be encoded as c string
     * @param remainder the remainder of the binary string as c string
     * @return  a pointer to the c string containing the encoded binary string,
     * at least 2 * R_LENGTH chars not including the terminating null character
    */
    char* getCodeword(const char* data, const char* remainder);
    /**
     * Verify the integrity of an encoded binary string
     * PRECONDITION:
     * 1. the encoded binary string is at least 2 * R_LENGTH chars long
     * 
     * @param codeword  the encoded binary string as c string
     * @return  a boolean indicating if the encoded binary string is intact
    */
    bool verifyCodeword(const char* codeword);
  
  private:
    /**
     * Performs one step of division, XOR the generator with a part of the 
     * padded binary string to be encoded
     * PRECONDITION:
     * 1. divideHead is at least R_LENGTH chars away from the null character
     * 
     * @param divideHead  a pointer on the binary string to be encoded, marking
     * the start of the part where XOR with generator will happen
    */
    void divideStep(char* & divideHead);
    /**
     * Copies and pads the binary string to be encoded with R_LENGTH trailing '0';
     * does not modify the original binary string
     * 
     * @param data  the binary string to be padded as c string
     * @return  a pointer to the c string containing the padded binary string
    */
    char* copyAndPadData(const char* data);
};
