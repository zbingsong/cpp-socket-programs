/**
 * A class for encoding a binary string with 2D even parity
 * 
 * PRECONDITION:
 * 1. length of binary string is a multiple of SEG_LENGTH
*/
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
     * Partition binary string into an array of small binary c strings
     * of length SEG_LENGTH
     * 
     * @param data  the binary string to be partitioned
     * @param segNum  the number of segments (small strings) to be 
     * partitioned into
     * @return  a pointer to the array of segments
    */
    char** partitionData(const char* data, const unsigned int segNum);

    /**
     * Insert row parity bits into the partitioned data, return the parity bits
     * inserted as c string
     * Computes, but does not insert, the whole parity bit
     * 
     * @param partData  the binary string partitioned into segments
     * @param segNum  number of segments in the partitioned data
    */
    char* insertRowParities(char** partData, const unsigned int segNum);

    /**
     * Insert column parity bits into the partitioned data, return the parity bits
     * inserted as c string
     * Computes and inserts the whole parity bit
     * 
     * @param partData  the binary string partitioned into segments
     * @param segNum  number of segments in the partitioned data
    */
    char* insertColumnParities(char** partData, const unsigned int segNum);

    /**
     * Concatenate the partitioned, encoded data back into a c string
     * 
     * @param partData  the binary string partitioned into segments, already encoded
     * @param segNum  number of segments in the partitioned data
    */
    char* reducePartDataToStr(const char* const* partData, const unsigned int segNum);

  private:
    /**
     * Partition encoded binary string into an array of small binary c strings
     * of length (SEG_LENGTH + 1)
     * 
     * @param codeword  the encoded binary string to be partitioned
     * @param segNum  number of segments (small strings) to be partitioned into
     * @return  a pointer to the array of segments
    */
    char** partitionCodeword(const char* codeword, const unsigned int segNum);

    /**
     * Compute the parity bit of a row of data
     * 
     * @param rowData the row of data as c string, whose parity bit will be computed
     * @return  the parity bit as char
    */
    char computeRowParity(const char* rowData);

    /**
     * Compute the parity bit of a column of data
     * 
     * @param partData  a pointer to the array of data segments
     * @param colNum  the index of the column whose parity bit will be computed
     * @param segNum  number of segments in the partitioned data
     * @return  the parity bit as char
    */
    char computeColumnParity(const char* const* partData, const unsigned int colNum, const unsigned int segNum);

    /**
     * Verify that all row parity bits (including the row of column parities) match
     * the encoded data
     * 
     * @param partCodeword  a pointer to the array of segments from partitioned codeword
     * @param segNum  number of segments in the partitioned codeword
     * @return  a boolean indicating if all row parities match their rows in data
    */
    bool verifyRowParities(const char* const* partCodeword, const unsigned int segNum);

    /**
     * Verify that all column parity bits (excluding the column of row parities) match
     * the encoded data
     * 
     * @param partCodeword  a pointer to the array of segments from partitioned codeword
     * @param segNum  number of segments in the partitioned codeword
     * @return  a boolean indicating if all column parities match their rows in data
    */
    bool verifyColumnParities(const char* const* partCodeword, const unsigned int segNum);
};
