/**********
Copyright (c) 2017, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

// This function represents an OpenCL kernel. The kernel will be call from
// host application using the xcl_run_kernels call. The pointers in kernel
// parameters with the global keyword represents cl_mem objects on the FPGA
// DDR memory.


// This Kernel has been adapted from Lorenzo Di Tucci and Marco Rabozzi to be compliant with CAOS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define DATA_SIZE 20480
#define INIT_VAL_1  10  
#define INIT_VAL_2  32

void vector_add(int c[DATA_SIZE], int a[DATA_SIZE], int b[DATA_SIZE]);



int main(int argc, char *argv[]){
    int * source_a = (int *)malloc(sizeof(int) * DATA_SIZE);
    int * source_b = (int *)malloc(sizeof(int) * DATA_SIZE);
    int * source_results = (int *)malloc(sizeof(int) * DATA_SIZE);

    for(int i = 0; i < DATA_SIZE; i++){
        source_a[i] = INIT_VAL_1;
        source_b[i] = INIT_VAL_2;
    }

    vector_add(source_results, source_a, source_b);
    
    for (int i = 0; i < DATA_SIZE; i++) {
        int host_result = source_a[i] + source_b[i];
        if(source_results[i] != host_result){
            printf("Error, Mismatch at index %d, Host %d Hardware %d \n", i, host_result, source_results[i]);
            printf("TEST FAILED \n");
            return -1;
        } else {
            printf("%d ", source_results[i]);
            if (((i + 1) % 16) == 0) printf("\n");
        }
    }

    printf("TEST PASSED \n");
    return 0;


}

void vector_add (int c[DATA_SIZE], int a[DATA_SIZE], int b[DATA_SIZE]){
    int arrayA[DATA_SIZE];
    int arrayB[DATA_SIZE];

    readA: for (int i = 0; i < DATA_SIZE; i++){
        arrayA[i] = a[i];
    }
    readB: for (int i = 0; i < DATA_SIZE; i++){
        arrayB[i] = b[i];
    }

    vadd_writeC: for(int  i = 0; i < DATA_SIZE; i++){
        c[i] = arrayA[i] + arrayB[i];
    }
}