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