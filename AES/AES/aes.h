#define BLOCK_LEN 16
#define BLOCK_SIDE 4
#define AES_128 128
#define AES_256 256

unsigned char MultiplyBytes(unsigned char g1, unsigned char g2);

void addRoundKey(unsigned char state[BLOCK_SIDE][BLOCK_SIDE], unsigned char subkey[BLOCK_SIDE][BLOCK_SIDE]);
void byteSubstitute(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void shiftRows(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void mixCols(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);