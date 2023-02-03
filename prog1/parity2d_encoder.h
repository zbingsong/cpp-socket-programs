class Parity2DEncoder {
  public:
    static const unsigned int SEG_LENGTH;
    // char* encode(const char* data);
    bool verify(const char* codeword);
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
