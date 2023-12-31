#include "aes.h"
#include "helperFunctions.h"

//SOURCE: https://pastebin.com/y8377Ra7
#pragma region AESReferenceTables
unsigned char s_box[256] = {
    0x63,   0x7c,   0x77,   0x7b,   0xf2,   0x6b,   0x6f,   0xc5,   0x30,   0x01,   0x67,   0x2b,   0xfe,   0xd7,   0xab,   0x76,
    0xca,   0x82,   0xc9,   0x7d,   0xfa,   0x59,   0x47,   0xf0,   0xad,   0xd4,   0xa2,   0xaf,   0x9c,   0xa4,   0x72,   0xc0,
    0xb7,   0xfd,   0x93,   0x26,   0x36,   0x3f,   0xf7,   0xcc,   0x34,   0xa5,   0xe5,   0xf1,   0x71,   0xd8,   0x31,   0x15,
    0x04,   0xc7,   0x23,   0xc3,   0x18,   0x96,   0x05,   0x9a,   0x07,   0x12,   0x80,   0xe2,   0xeb,   0x27,   0xb2,   0x75,
    0x09,   0x83,   0x2c,   0x1a,   0x1b,   0x6e,   0x5a,   0xa0,   0x52,   0x3b,   0xd6,   0xb3,   0x29,   0xe3,   0x2f,   0x84,
    0x53,   0xd1,   0x00,   0xed,   0x20,   0xfc,   0xb1,   0x5b,   0x6a,   0xcb,   0xbe,   0x39,   0x4a,   0x4c,   0x58,   0xcf,
    0xd0,   0xef,   0xaa,   0xfb,   0x43,   0x4d,   0x33,   0x85,   0x45,   0xf9,   0x02,   0x7f,   0x50,   0x3c,   0x9f,   0xa8,
    0x51,   0xa3,   0x40,   0x8f,   0x92,   0x9d,   0x38,   0xf5,   0xbc,   0xb6,   0xda,   0x21,   0x10,   0xff,   0xf3,   0xd2,
    0xcd,   0x0c,   0x13,   0xec,   0x5f,   0x97,   0x44,   0x17,   0xc4,   0xa7,   0x7e,   0x3d,   0x64,   0x5d,   0x19,   0x73,
    0x60,   0x81,   0x4f,   0xdc,   0x22,   0x2a,   0x90,   0x88,   0x46,   0xee,   0xb8,   0x14,   0xde,   0x5e,   0x0b,   0xdb,
    0xe0,   0x32,   0x3a,   0x0a,   0x49,   0x06,   0x24,   0x5c,   0xc2,   0xd3,   0xac,   0x62,   0x91,   0x95,   0xe4,   0x79,
    0xe7,   0xc8,   0x37,   0x6d,   0x8d,   0xd5,   0x4e,   0xa9,   0x6c,   0x56,   0xf4,   0xea,   0x65,   0x7a,   0xae,   0x08,
    0xba,   0x78,   0x25,   0x2e,   0x1c,   0xa6,   0xb4,   0xc6,   0xe8,   0xdd,   0x74,   0x1f,   0x4b,   0xbd,   0x8b,   0x8a,
    0x70,   0x3e,   0xb5,   0x66,   0x48,   0x03,   0xf6,   0x0e,   0x61,   0x35,   0x57,   0xb9,   0x86,   0xc1,   0x1d,   0x9e,
    0xe1,   0xf8,   0x98,   0x11,   0x69,   0xd9,   0x8e,   0x94,   0x9b,   0x1e,   0x87,   0xe9,   0xce,   0x55,   0x28,   0xdf,
    0x8c,   0xa1,   0x89,   0x0d,   0xbf,   0xe6,   0x42,   0x68,   0x41,   0x99,   0x2d,   0x0f,   0xb0,   0x54,   0xbb,   0x16
};

//constant matrix for mixing columns
unsigned char mixColMatrix[BLOCK_SIDE][BLOCK_SIDE] = {
    { 0x02, 0x03, 0x01, 0x01 },
    { 0x01, 0x02, 0x03, 0x01 },
    { 0x01, 0x01, 0x02, 0x03 },
    { 0x03, 0x01, 0x01, 0x02 }
};
#pragma endregion

//Reference: https://gist.github.com/meagtan/dc1adff8d84bb895891d8fd027ec9d8c 
//Galois Field multiplication 
unsigned char MultiplyBytes(unsigned char g1, unsigned char g2) {
    const int numberBytes = 8;
    unsigned char result = 0;

    for (size_t i = 0; i < numberBytes; i++)
    {
        //if least significant bit is active
        if (g2 & 0x01)
            result = result ^ g1; // equals to result += g1 in the extension field
        

        //if g1 is greater or equal to 128(0x80 in hex, 0100 0000 in bynary) 
        //we shall reduce it 
        bool hiBit = (g1 & 0x80);
        g1 <<= 1; //left shift
        if (hiBit)
            g1 ^= 0x1B; // substraction
        
        g2 >>= 1;
    }

    return result;
}

void addRoundKey(unsigned char state[BLOCK_SIDE][BLOCK_SIDE], unsigned char subkey[BLOCK_SIDE][BLOCK_SIDE]) {
    for (size_t row = 0; row < BLOCK_SIDE; row++)
    {
        for (size_t col = 0; col < BLOCK_SIDE; col++)
        {
            state[row][col] = state[row][col] ^ subkey[row][col];
        }
    }
}

/// <summary>
/// Substitutes each byte using the s-box, defined above
/// </summary>
/// <param name="state">
/// Current state of the matrix
/// </param>
void byteSubstitute(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]) {
    for (size_t row = 0; row < BLOCK_SIDE; row++)
    {
        for (size_t col = 0; col < BLOCK_SIDE; col++)
        {
            state[row][col] = s_box[state[row][col]];
        }
    }
}

/// <summary>
/// Rotating each row according to its position in the matrix(row:0 is shifted 0 times, row:1 will be shifted 1 time and so on)
/// </summary>
/// <param name="state">Current state of the matrix</param>
void shiftRows(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]) {
    for (int row = 0; row < BLOCK_SIDE; row++) {
        leftShift(state[row], row, BLOCK_SIDE);
    }
}

/// <summary>
/// Each cell of the matrix is transformed to the byte multiplication of mixColMatrix defined above and the current state.
/// In this way, we mix the columns.(Step 3 of the algorithm)
/// </summary>
/// <param name="state">
/// Current state of the matrix
/// </param>
void mixCols(unsigned char state[BLOCK_SIDE][BLOCK_SIDE]) {
    unsigned char out[BLOCK_SIDE][BLOCK_SIDE];

    //matrix multiplication is used when we mix cols
    for (size_t row = 0; row < BLOCK_SIDE; row++)
    {
        for (size_t col = 0; col < BLOCK_SIDE; col++)
        {
            out[row][col] = 0x00;
            for (size_t i = 0; i < BLOCK_SIDE; i++)
            {
                out[row][col] ^= MultiplyBytes(mixColMatrix[row][i], state[i][col]);
            }
        }
    }

    memCopy(out, state);
}

void generateKeySchedule(unsigned char* key, int keylen, unsigned char subkeysArr[][BLOCK_SIDE][BLOCK_SIDE]) {
    generateKeySchedule128(key, subkeysArr);
}

/// <summary>
/// Populates the subkeysArr which we use in the encryption.
/// </summary>
/// <param name="key">
/// The key from which we generate the subkey array.
/// </param>
/// <param name="subkeysArr">
/// Three dimension array for our parameter, in which we store our subkeys.
/// </param>
void generateKeySchedule128(unsigned char* key, unsigned char subkeysArr[AES_128_ROUNDS][BLOCK_SIDE][BLOCK_SIDE]) {
    int counter = 0;

    for (size_t col = 0; col < BLOCK_SIDE; col++)
    {
        for (size_t row = 0; row < BLOCK_SIDE; row++)
        {
            subkeysArr[0][row][col] = key[counter++];
        }
    }

    //Generate each round
    unsigned char roundCoefficient = 0x01;
    for (size_t i = 1; i < 10; i++)
    {
        //transforms key
        unsigned char g[4] = {
              s_box[subkeysArr[i - 1][1][3]] ^ roundCoefficient,
              s_box[subkeysArr[i - 1][2][3]],
              s_box[subkeysArr[i - 1][3][3]],
              s_box[subkeysArr[i - 1][0][3]]
        };

        for (size_t row = 0; row < BLOCK_SIDE; row++)
        {
            subkeysArr[i][row][0] = subkeysArr[i - 1][row][0] ^ g[row];
        }

        for (size_t col = 1; col < BLOCK_SIDE; col++)
        {
            for (size_t row = 0; row < BLOCK_SIDE; row++)
            {
                subkeysArr[i][row][col] = subkeysArr[i - 1][row][col] ^ subkeysArr[i][row][col - 1];
            }
        }

        //increment round coefficient
        roundCoefficient = MultiplyBytes(roundCoefficient, 0x02);
    }
}

void copyTwoDimensionsIntoOne(unsigned char from[BLOCK_SIDE][BLOCK_SIDE], unsigned char to[BLOCK_LEN]) {
    int i = 0;
    for (size_t cols = 0; cols < BLOCK_SIDE; cols++)
    {
        for (size_t row = 0; row < BLOCK_SIDE; row++)
        {
            to[i++] = from[row][cols];
        }
    }
}

void aes_encrypt_block(unsigned char* text, int n, unsigned char subkeys[][BLOCK_SIDE][BLOCK_SIDE], int nr, unsigned char out[BLOCK_LEN]) {
    unsigned char state[BLOCK_SIDE][BLOCK_SIDE];
    int i = 0;

    for (size_t col = 0; col < BLOCK_SIDE; col++)
    {
        for (size_t row = 0; row < BLOCK_SIDE; row++)
        {
            if (i < n)
                state[row][col] = text[i++];
            else
                state[row][col] = 0;
        }
    }

    int round = 0;
    //ROUND 0
    addRoundKey(state, subkeys[round++]);

    //ROUNDS 1 to NR-1
    while(round < nr)
    {
        byteSubstitute(state);
        shiftRows(state);
        mixCols(state);
        addRoundKey(state, subkeys[round++]);
    }

    //Last Round
    byteSubstitute(state);
    shiftRows(state);
    addRoundKey(state, subkeys[nr]);

    copyTwoDimensionsIntoOne(state, out);
}  

void aes_encrypt(unsigned char* input, int n, unsigned char* key, int keylen, unsigned char** out) {
    //determine number of rounds
    if (keylen == AES_256) {
        /*unsigned  char subkeys[AES_256_NR + 1][BLOCK_SIDE][BLOCK_SIDE];

        generateKeySchedule(key, keylen, subkeys);*/
    }
    else {
        //NR numbers of BLOCK_SIDE*BLOCK_SIDE keys
        unsigned  char subkeys[AES_128_NR + 1][BLOCK_SIDE][BLOCK_SIDE];
        generateKeySchedule(key, keylen, subkeys);
        
        //1:52:16 move this into a function
        int noBlocks = (n >> 4) + 1; // n / BLOCK_LENGTH
        *out = new unsigned char[noBlocks * BLOCK_LEN];

        for (size_t i = 0; i < noBlocks; i++)
        {
            aes_encrypt_block(input + (i << 4), BLOCK_LEN, subkeys, AES_128_NR + 1, *out + (i << 4));
        }
    }
}

void deleteThreeDemensionalArray(unsigned char*** arr, int length, int rows, int cols) {
    for (size_t i = 0; i < length; i++)
    {
        for (size_t j = 0; j < length; j++)
        {
            delete[] arr[i][j];
        }
        delete[] arr[i];
    }

    delete[] arr;
}