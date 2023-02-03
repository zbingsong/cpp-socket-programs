class Parity2DEncoder {
  public:
    /// @brief length of each segment of partitioned data (8 bits)
    static const unsigned int SEG_LENGTH;
    /**
     * Verify that codeword is intact by 2D parity
     * 
     * @param codeword  the encoded binary string to be verified
     * @return  a boolean indicating if the codeword has any error
    */
    bool verify(const char* codeword);
    /**
     * Partition binary string into an array of small binary strings
     * of length SEG_LENGTH
     * 
     * @param data  the binary string to be partitioned
     * @param segNum  the number of segments (small strings) to be 
     * partitioned into
     * @return  a pointer to the array of segments
    */
    char** partitionData(const char* data, const unsigned int segNum);
    char* insertRowParities(char** partData, const unsigned int segNum);
    char* insertColumnParities(char** partData, const unsigned int segNum);
    char* reducePartDataToStr(const char* const* partData, const unsigned int segNum);
  private:
    char** partitionCodeword(const char* codeword, const unsigned int segNum);
    char computeRowParity(const char* rowData);
    char computeColumnParity(const char* const* partData, const unsigned int colNum, const unsigned int segNum);
    bool verifyRowParities(const char* const* partCodeword, const unsigned int segNum);
    bool verifyColumnParities(const char* const* partCodeword, const unsigned int segNum);
};
