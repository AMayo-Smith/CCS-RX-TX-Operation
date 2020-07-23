/*
 * encode.c
 *
 *  Created on: Dec 7, 2019
 *  Author: alexandermayo-smith
 *  amici.
 */
#include "addParity.h"

#include <stdio.h>


#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      30
#define INPUTLENGTH 5
#define TESTNUMBER 4000

/*****************/
int generator[8][16]=
	{{1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0 },
	 {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0},
	 {0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1 },
	 {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1},
	 {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1 },
	 {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0 },
	 {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	 {0, 0 ,0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1 }};

int code1[512]= {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0,
                  0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1,
                  1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1,
                  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1,
                  0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
                  0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
                  1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0};

int code0[512]={1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1,
          0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1,
          0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1,
          1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1,
          1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1,
          0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0,
          0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0,
          1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0};





int main(){
int seqNumber;
int Payload[RFEASYLINKTXPAYLOAD_LENGTH];
// For testing
int finalLength=RFEASYLINKTXPAYLOAD_LENGTH*INPUTLENGTH;
int testMessage=TESTNUMBER*RFEASYLINKTXPAYLOAD_LENGTH;
int output[testMessage];

printf("Start: \n");

for (seqNumber=0;seqNumber<TESTNUMBER;seqNumber++){

	Payload[0]=seqNumber+2;
	Payload[1]=9;
	/**************format input into Binary**************/
		char input[INPUTLENGTH]={"Hello"};
		int k;
		int j;
		int signalOutput[16*INPUTLENGTH];
		for (int f=0; f < INPUTLENGTH; f++){ //for each character
				char currentChar=input[f];
				int tempArray[8];
				for ( int j=0; j<8;j++)
					tempArray[7-j]=( (currentChar & (1 << j)) ? 1 : 0 ); //convert to binary, tempArray is a single byte
			/*	for ( int z=0; z<8;z++)
				printf("%d",tempArray[z]);
				printf(" ");  */

	/**************add parity**************/

				int parityByte[16];
				   int it;
				   for (it=0; it<16; it++){ //matrix multiplication
					   int tempSum;
					   tempSum=0;
					   for (j=0; j<8;j++){
						   tempSum += tempArray[j]*generator[j][it];
						   }
						   int modSum;
						  modSum=tempSum % 2; //To format results in binary

						   parityByte[it]=modSum;
				   }
						for (k=0;k<16;k++){//add to signalOutput matrix
							signalOutput[16*f+k]=parityByte[k];
							//printf("%d",signalOutput[16*f+k]);
							}
						//printf("-");

			}
		//printf("\n");
	//**************matched filter**************
	int load;
	int idx;
	int r;
	int m;
	for (k = 2; k < RFEASYLINKTXPAYLOAD_LENGTH; k++) //load each packet
		   {
			   load=(seqNumber-1)*(RFEASYLINKTXPAYLOAD_LENGTH-2)+k-2;
			   idx= load/512; //describes where in the message you are
			   r= load % 512; //describes where in the message you are

			   if (idx<16*INPUTLENGTH){
				   m=signalOutput[idx];
				   if (m==0){
					   Payload[k] = code0[r];
				   }
				   else{
					   Payload[k] = code1[r];
				   }

			   }
			   else{

				Payload[k] = 0;
			   }


		   }

	for(int testing=0;testing<RFEASYLINKTXPAYLOAD_LENGTH;testing++){
		output[testing+RFEASYLINKTXPAYLOAD_LENGTH*seqNumber]=Payload[testing];

	}
}

//**************write to file**************

	FILE *fP;
	fP=fopen("encodedData.txt","w");
	for(int i=0;i<testMessage;i++){
		if (output[i]!= 1 && output[i]!= 0)
			fprintf(fP,"|%d| ",output[i]);
	//	else if (output[i]<-1)
	//		fprintf(fP,"b@%d ",(i));
		else
			fprintf(fP,"%d ", output[i]);
	}

	fclose(fP);
printf("Done.");
	return 0;
}
