#include "helperFunctions.h"


void reverseArray(unsigned char* arr, int start, int end) {
	while (start < end) {
		unsigned char temp = arr[start];
		arr[start] = arr[end];
		arr[end] = temp;

		start++;
		end--;
	}
}

void leftShift(unsigned char* arr, int positionToShift, int lenght) {
	reverseArray(arr, 0, positionToShift);
	reverseArray(arr, positionToShift, lenght - 1);
	reverseArray(arr, 0, lenght - 1);
}

void rightShift(unsigned char* arr, int positionToShift, int lenght) {
	leftShift(arr, lenght - positionToShift, lenght);
}

void memCopy(unsigned char from[BLOCK_SIDE][BLOCK_SIDE], unsigned char to[BLOCK_SIDE][BLOCK_SIDE]) {
	for (size_t row = 0; row < BLOCK_SIDE; row++)
	{
		for (size_t col = 0; col < BLOCK_SIDE; col++)
		{
			to[row][col] = from[row][col];
		}
	}
}