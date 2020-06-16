/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== rfEasyLinkTx_nortos.c ========
 */
/* Application header files */
#include "smartrf_settings/smartrf_settings.h"

/* Board Header files */
#include "Board.h"

/* Standard C Libraries */
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>


/* TI Drivers */
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/rf/RF.h>
#include <ti/devices/DeviceFamily.h>

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* LDPC Include */
#include "addParity.h"


/* Undefine to not use async mode */
#define RFEASYLINKTX_ASYNC

#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      30

#define FINALLENGTH                     512

/* Pin driver handle */
static PIN_Handle pinHandle;
static PIN_State pinState;

/*Codes */

int generator[8][16]=
    {{1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
     {1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
     {0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
     {0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0},
     {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
     {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
     {1, 1, 0, 1, 0, 1, 1, 1, 0, 0 ,0, 0, 0, 0, 0, 1}};

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






/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config pinTable[] = {
    Board_PIN_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_PIN_LED2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint16_t seqNumber;

#ifdef RFEASYLINKTX_ASYNC
static volatile bool txDoneFlag;
static volatile uint8_t txSleepPeriodsElapsed;
#endif //RFEASYLINKTX_ASYNC

#ifdef RFEASYLINKTX_ASYNC
void txDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle LED1 to indicate TX */
        PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
    }
    else if(status == EasyLink_Status_Aborted)
    {
        /* Toggle LED2 to indicate command aborted */
        PIN_setOutputValue(pinHandle, Board_PIN_LED2,!PIN_getOutputValue(Board_PIN_LED2));
    }
    else
    {
        /* Toggle LED1 and LED2 to indicate error */
        PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
        PIN_setOutputValue(pinHandle, Board_PIN_LED2,!PIN_getOutputValue(Board_PIN_LED2));
    }
    txDoneFlag = true;
    txSleepPeriodsElapsed = 0;
}
#endif //RFEASYLINKTX_ASYNC

void *mainThread(void *arg0)
{
    uint32_t absTime;

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    if (pinHandle == NULL)
    {
        while(1);
    }

    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, Board_PIN_LED1, 0);
    PIN_setOutputValue(pinHandle, Board_PIN_LED2, 0);

    static uint8_t txBurstSize = 0;

#ifdef RFEASYLINKTX_ASYNC
    /* Reset the sleep period counter */
    txSleepPeriodsElapsed = 0;
    /* Set the transmission flag to its default state */
    txDoneFlag = false;
#endif //RFEASYLINKTX_ASYNC

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    /*
     * Initialize EasyLink with the settings found in easylink_config.h
     * Modify EASYLINK_PARAM_CONFIG in easylink_config.h to change the default
     * PHY
     */
    if (EasyLink_init(&easyLink_params) != EasyLink_Status_Success){
        while(1);
    }


    /*
     * If you wish to use a frequency other than the default, use
     * the following API:
     * EasyLink_setFrequency(868000000);
     */



    while(1) {
        EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };

        /* Create packet with incrementing sequence number and random payload */
        txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
        txPacket.payload[1] = (uint8_t)(seqNumber++);


        /* Encode Message */
        int k;
        int j; //calculates index based on  seqNumber
        //uint8_t i;
       char input[1]={"H"};
       static int inputLength=sizeof(input)/sizeof(char);
     //   static int l1=16*sizeof(input)/sizeof(char);
       int signalOutput[16];
      // int signalOutput[l1];
       int f;
        for (f=0; f < inputLength; f++){ //for each character
           char currentChar=input[f];
            int tempArray[8];
            for (j=0; j<8;j++){
                tempArray[7-j]=( (currentChar & (1 << j)) ? 1 : 0 ); //convert to binary, tempArray is a single byte
            }


//**************add parity**************/

           int parityByte[16];
                  // parityByte=addParity(tempArray,generator); //add parity bits

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
                    }





        }
        //**************matched filter**************/

     //       int finalLength=16*512*1;
           // int output[16*512*sizeof(input)/sizeof(char)];
     //       int output[FINALLENGTH];

          //  int z;
           // int y;
           // int index;
           // for(z=0;z<FINALLENGTH;z++){ //for each individual bit
               // if (signalOutput[z]==0){
                   // for(y=0; y<512; y++){
                      //  index=512*z+y;
                       //output[index]=code0[y];
                   // }
            //    }

             //   if (signalOutput[z]==1){

                 //   for(y=0;y<512;y++){
                       // output[512*z+y]=code1[y];
                   // }
           //     }

         //   }


      //  int test[FINALLENGTH];
       // for(k=0;k<FINALLENGTH;k++){
       //     test[k]=k;
      //  }



       // uint8_t tmax=sizeof(test)/sizeof(uint8_t);
        int load;
        int idx;
        int r;
        int m;
        for (k = 2; k < RFEASYLINKTXPAYLOAD_LENGTH; k++)
        {
            load=(seqNumber-1)*(RFEASYLINKTXPAYLOAD_LENGTH-2)+k-2;
            idx= load/512;
            r= load % 512;

            if (idx<16){
                m=signalOutput[idx];
                if (m==0){
                    txPacket.payload[k] = code0[r];
                }
                else{
                    txPacket.payload[k] = code1[r];
                }

            }
            else{

             txPacket.payload[k] = 0;
            }
        }

        txPacket.len = RFEASYLINKTXPAYLOAD_LENGTH;

        /*
         * Address filtering is enabled by default on the Rx device with the
         * an address of 0xAA. This device must set the dstAddr accordingly.
         */
        txPacket.dstAddr[0] = 0xaa;

        /* Add a Tx delay for > 500ms, so that the abort kicks in and brakes the burst */
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {
            // Problem getting absolute time
        }
        if(txBurstSize++ >= RFEASYLINKTX_BURST_SIZE)
        {
            /* Set Tx absolute time to current time + 1s */

            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(1000);
            txBurstSize = 0;
        }
        /* Else set the next packet in burst to Tx in 100ms */
        else
        {
            /* Set Tx absolute time to current time + 100ms */
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);
        }

#ifdef RFEASYLINKTX_ASYNC
        /*
         * Set the Transmit done flag to false, callback will set it to true
         * Also set the sleep counter to 0
         */
        txDoneFlag = false;
        txSleepPeriodsElapsed = 0;

        /* Transmit the packet */
        EasyLink_transmitAsync(&txPacket, txDoneCb);

        while(!txDoneFlag){
            /*
             * Set the device to sleep for 108ms. The packet transmission is
             * set 100 ms in the future but takes about 7ms to complete and
             * for the execution to hit the callback. A 1ms buffer is added to
             * the sleep time to ensure the callback always execute prior to
             * the end of usleep().
             */
            usleep(108000);

            /* check to see if the transmit flag was set during sleep */
            if(!txDoneFlag){
                txSleepPeriodsElapsed++;
                if(txSleepPeriodsElapsed == 3){
                    /* 324 ms have passed. We need to abort the transmission */
                    if(EasyLink_abort() == EasyLink_Status_Success)
                    {
                        /*
                         * Abort will cause the txDoneCb to be called and the
                         * txDoneFlag to be set
                         */
                        while(!txDoneFlag){};
                    }
                    break;
                }
            }
        }
#else
        EasyLink_Status result = EasyLink_transmit(&txPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle LED1 to indicate TX */
            PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
        }
        else
        {
            /* Toggle LED1 and LED2 to indicate error */
            PIN_setOutputValue(pinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
            PIN_setOutputValue(pinHandle, Board_PIN_LED2,!PIN_getOutputValue(Board_PIN_LED2));
        }
#endif //RFEASYLINKTX_ASYNC
    }
}
