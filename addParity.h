/*
 * addParity.h
 * adds parity bits to a given byte
 *  Created on: Dec 7, 2019
 *      Author: alexandermayo-smith
 */

#ifndef ADDPARITY_H_
#define ADDPARITY_H_
#include <stdio.h>


int  * addParity(int input[8], int generator[8][16]){
    int i;
	int static parityOutput[16];
	for (i=0;i<16;i++){ //matrix multiplication
		int tempSum=0;
		int j;
		for (j = 0; j<8;j++){
			tempSum += input[j]*generator[j][i];
		}
		int modSum=tempSum % 2; //To format results in binary

		parityOutput[i]=modSum;

	}

	return parityOutput;

}


#endif /* ADDPARITY_H_ */
