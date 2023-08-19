#pragma region Macros

#define BLOCK_LEN 16
#define BLOCK_SIDE 4
#define AES_128 128
#define AES_128_NR 10
#define AES_256 256
#define AES_256_NR 14
#define AES_128_ROUNDS 11
#pragma endregion

unsigned char MultiplyBytes(unsigned char g1, unsigned char g2);

#pragma region EncryptionLayers

void addRoundKey(unsigned char state[BLOCK_SIDE][BLOCK_SIDE], unsigned char subkey[BLOCK_SIDE][BLOCK_SIDE]);
void byteSubstitute(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void shiftRows(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void mixCols(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]);
void aes_encrypt_block(unsigned char* text, int n, unsigned char subkeys[][BLOCK_SIDE][BLOCK_SIDE], int nr, unsigned char out[BLOCK_LEN]);
void aes_encrypt(unsigned char* input, int n, unsigned char* key, int keylen, unsigned char** out);
#pragma endregion

void generateKeySchedule(unsigned char* key, int keylen, unsigned char subkey[][BLOCK_SIDE][BLOCK_SIDE] );
void generateKeySchedule128(unsigned char* key, unsigned char subkey[AES_128_ROUNDS][BLOCK_SIDE][BLOCK_SIDE]);