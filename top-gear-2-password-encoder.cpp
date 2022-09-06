// TODO
// FIX bool isPlus(uint16 x);

#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<algorithm>
#include<string>
#include<cstring>

using namespace std;

typedef unsigned short int uint16;

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

#define BITMASK_SET(x, mask) ((x) |= (mask))
#define BITMASK_CLEAR(x, mask) ((x) &= (~(mask)))
#define BITMASK_FLIP(x, mask) ((x) ^= (mask))
#define BITMASK_CHECK_ALL(x, mask) (!(~(x) & (mask)))
#define BITMASK_CHECK_ANY(x, mask) ((x) & (mask))

#define MEMORY_SIZE 32
#define PASSWORD_SIZE 22
#define PLAYER_DATA_SIZE 13
#define GENERAL_DATA_SIZE 3

string charToBinaryString(unsigned char x);
string printuint16(uint16 x);
void printCharArray(unsigned char *v, int size);
void printCharArrayBinary(unsigned char *v, int size);
void uintSet(unsigned char *v, uint16 x);
uint16 uintFrom(unsigned char *v);

void encodeCurrentPassword(unsigned char *v);

void initializeMemory(unsigned char *v, uint16 *a, uint16 *x, uint16 *y);
void initializeVariables(unsigned char *v, uint16 a, uint16 x);
void uintadd(uint16 *a, unsigned char *x, char *carry);
void rotateRight(unsigned char *v, char *carry, int ignoreCarry);
void rotateLeft(unsigned char *v, char *carry, int ignoreCarry);
bool isPlus(uint16 x);
uint16 checksum(uint16 *player1Data, uint16 *player2Data, uint16 *generalData);

// Definitions:
// Remember the 65816 is little-endian (the least significant byte shows up first in memory)

// LSR($x)
// Logical Shift Right
// CARRY is not used on input, ie. the leftmost output bit is ALWAYS 0.
// The rightmost input bit is carried into CARRY
// So (in 16-bit mode):
//
// $x   = 76543210 <---- These are indexes
// $x+1 = fedcba98 <---- These are indexes
//   lsr($x)
// = lsr(fedcba9876543210)
//   v--- this is a literal 0 value.
// = 0fedcba987654321 (with bit with index 0 going into carry)
//
// An example:
// $14 = 11111111 ($ff)
// $15 = 11111111 ($ff)
// 
// lsr($14)
// 
// $14 = 11111111 ($ff)
// $15 = 01111111 ($7f)
// CARRY = 1

// ROR($x)
// Rotate Right
// Equivalent to LSR, except the carry flag
// is shifted into the most significant bit rather than a zero.
// So (in 16-bit mode):
//
// $x   = 76543210 <---- These are indexes
// $x+1 = fedcba98 <---- These are indexes
//   ror($x)
// = ror(fedcba9876543210)
//   v--- this is the value of the CARRY flag.
// = Cfedcba987654321 (with bit with index 0 going into carry)
//
// An example:
// $14 = 01010101 ($55)
// $15 = 01010101 ($55)
// 
// ror($14) with carry 1
// 
// $14 = 10101010 ($aa)
// $15 = 10101010 ($aa) (if carry was 1)
// or
// $15 = 00101010 ($2a) (if carry was 0)


string charToBinaryString(unsigned char x){
    string resp;
    for(int i=0; i<8; i++){
        resp+= x & 1 ? '1' : '0';
        x >>=1;
    }
    std::reverse(begin(resp), end(resp));
    return resp;
}

string printuint16(uint16 x){
    unsigned char lessSignificantByte = x;
    unsigned char mostSignificantByte = x>>8;
    printf("%02X %02X\n",mostSignificantByte,lessSignificantByte);
    string resp;
    string byte2 = charToBinaryString(mostSignificantByte);
    string byte1 = charToBinaryString(lessSignificantByte);
    resp = byte2 + " " + byte1;
    return resp;
}

void printCharArray(unsigned char *v, int size){
    for(int i=0; i<size; i++){
        printf("%02X ",v[i]);
    }
    printf("\n");
}

void printCharArrayBinary(unsigned char *v, int size){
    for(int i=0; i<size; i++){
        cout << charToBinaryString(v[i]) << " ";
    }
    printf("\n");
}

void uintSet(unsigned char *v, uint16 x){
    unsigned char lessSignificantByte = x;
    unsigned char mostSignificanByte = x>>8;
    v[0] = lessSignificantByte;
    v[1] = mostSignificanByte;
}

uint16 uintFrom(unsigned char *v){
    uint16 resp = v[1] << 8;
    resp |= v[0];
    return resp;
}


int main(){
    unsigned char v[MEMORY_SIZE]; memset(v,0,MEMORY_SIZE*sizeof(unsigned char));
    uint16 a,x,y;

    initializeMemory(v,&a,&x,&y);
    encodeCurrentPassword(v);
}

void initializeMemory(unsigned char *v, uint16 *a, uint16 *x, uint16 *y){

    uint16 playerData[2][PLAYER_DATA_SIZE];
    uint16 playerDataA[PLAYER_DATA_SIZE];
    uint16 generalData[GENERAL_DATA_SIZE];
    uint16 generalDataA[GENERAL_DATA_SIZE];
    memset(playerData,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(playerDataA,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(generalData,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(generalDataA,0,PLAYER_DATA_SIZE*sizeof(uint16));

    playerData[0][0]=0; // $1d03 - P1 engineLevel*
    playerData[1][0]=0; // $1d2c - P2 engineLevel*
    playerDataA[0]=0x0004;

    playerData[0][1]=0; // $1d05 - P1 wetTireLevel*
    playerData[1][1]=0; // $1d2e - P2 wetTireLevel*
    playerDataA[1]=0x0004;

    playerData[0][2]=0; // $1d07 - P1 dryTireLevel*
    playerData[1][2]=0; // $1d30 - P2 dryTireLevel*
    playerDataA[2]=0x0004;

    playerData[0][3]=0; // $1d09 - P1 gearboxLevel*
    playerData[1][3]=0; // $1d32 - P2 gearboxLevel*
    playerDataA[3]=0x0004;

    playerData[0][4]=0; // $1d0b - P1 nitroLevel*
    playerData[1][4]=0; // $1d34 - P2 nitroLevel*
    playerDataA[4]=0x0004;

    playerData[0][5]=0; // $1d0d - ???*
    playerData[1][5]=0; // $1d36 - ???*
    playerDataA[5]=0x0004;

    playerData[0][6]=0; // $1d0f - P1 sideArmorLevel*
    playerData[1][6]=0; // $1d38 - P2 sideArmorLevel*
    playerDataA[6]=0x0005;

    playerData[0][7]=0; // $1d11 - P1 rearArmorLevel*
    playerData[1][7]=0; // $1d3a - P2 rearArmorLevel*
    playerDataA[7]=0x0005;

    playerData[0][8]=0; // $1d13 - P1 frontArmorLevel*
    playerData[1][8]=0; // $1d3c - P2 frontArmorLevel*
    playerDataA[8]=0x0005;

    playerData[0][9]=0x0000; // $1d15 - P1 color*
    playerData[1][9]=0x0001; // $1d3e - P2 color*
    playerDataA[9]=0x0008;

    playerData[0][10]=0; // $1cff - P1 speedType (MPH/KPH)*
    playerData[1][10]=0; // $1d28 - P2 speedType (MPH/KPH)*
    playerDataA[10]=0x0002;

    playerData[0][11]=0x8000; // $1d01 - P1 gear type and control type*
    playerData[1][11]=0x8000; // $1d2a - P2 gear type and control type*
    // This one has some extra steps down below
    playerDataA[11]=0x000a;

    playerData[0][12]=0; // $1d19 - P1 Money (Thousands)*
    playerData[1][12]=0; // $1d42 - P2 Money (Thousands)*
    playerDataA[12]=0x0500;

    generalData[0]=0; // $1cdd - Difficulty*
    generalDataA[0]=0x0003;

    generalData[1]=16; // $1ce1 - Region Number*
    generalDataA[1]=0x0041;

    generalData[2]=0; // $1cdb - Number of players (1 or 2)*
    generalDataA[2]=0x0002;


    // From 9fd33f
    uintSet(&v[0x06],0x0000);
    uintSet(&v[0x08],0x0000);
    uintSet(&v[0x0a],0x0000);
    uintSet(&v[0x0c],0x0000);
    uintSet(&v[0x0e],0x0000);
    uintSet(&v[0x10],0x0000);
    uintSet(&v[0x12],0x0000);

    // From 9fd34d
    for(int j=0;j<2;j++){
        for(int i=0; i<=10; i++){
            *x = playerData[j][i];
            *a = playerDataA[i];
            initializeVariables(v,*a,*x);
        }

        if(!isPlus(playerData[j][11])){
            uint16 aux = 0x7fff;
            *x = (playerData[j][11] & aux) + 0x0005;
            *a = playerDataA[11];
        }
        initializeVariables(v,*a,*x);

        *x = playerData[j][12];
        *a = playerDataA[12];
        initializeVariables(v,*a,*x);
    }

    // General data part

    // Difficulty
    *x = generalData[0];
    *a = generalDataA[0];
    initializeVariables(v,*a,*x);

    // Region number
    *a = generalData[1];
    *a = *a << 2;
    *x = *a;
    *a = generalDataA[1];
    initializeVariables(v,*a,*x);

    // Number of players
    *x = generalData[2];
    *a = generalDataA[2];
    initializeVariables(v,*a,*x);

    // 9fd46a - Calculate checksum
    *a = checksum(playerData[0],playerData[1], generalData);
    printf("Checksum: "); printuint16(*a);
    *x = *a;
    *a = 0x006d;
    initializeVariables(v,*a,*x);

    // Power-on password
    // uintSet(&v[0x06],0xd601);
    // uintSet(&v[0x08],0x5114);
    // uintSet(&v[0x0a],0xbcec);
    // uintSet(&v[0x0c],0x8be7);
    // uintSet(&v[0x0e],0x0001);
    // uintSet(&v[0x10],0x0000);
    // uintSet(&v[0x12],0x0000);
    // Should be RCYT8<G<M8TVR>RLG>(?TT

    // Power-on + Britain password
    // uintSet(&v[0x06],0xd96a);
    // uintSet(&v[0x08],0x5114);
    // uintSet(&v[0x0a],0xbcec);
    // uintSet(&v[0x0c],0x8be7);
    // uintSet(&v[0x0e],0x0001);
    // uintSet(&v[0x10],0x0000);
    // uintSet(&v[0x12],0x0000);
    // Should be L5C<NW5W9N<>)Y)85YTR<<

}

void initializeVariables(unsigned char *v, uint16 a, uint16 x){
    uintSet(&v[0x04],a);
    uintSet(&v[0x14],x);

    uintSet(&v[0x16],0x0000);
    uintSet(&v[0x18],0x0000);
    uintSet(&v[0x1a],0x0000);
    uintSet(&v[0x1c],0x0000);
    uintSet(&v[0x1e],0x0000);
    uintSet(&v[0x20],0x0000);

    char carry = 0;
    do
    {
        // 9fd827
        rotateRight(&v[0x04], &carry, 1);
        if(carry!=0){
            a = uintFrom(&v[0x06]);
            carry = 0;
            uintadd(&a,&v[0x14],&carry);
            
            a = uintFrom(&v[0x08]);
            uintadd(&a,&v[0x16],&carry);

            a = uintFrom(&v[0x0a]);
            uintadd(&a,&v[0x18],&carry);

            a = uintFrom(&v[0x0c]);
            uintadd(&a,&v[0x1a],&carry);

            a = uintFrom(&v[0x0e]);
            uintadd(&a,&v[0x1c],&carry);

            a = uintFrom(&v[0x10]);
            uintadd(&a,&v[0x1e],&carry);

            a = uintFrom(&v[0x12]);
            uintadd(&a,&v[0x20],&carry);

        }
        //9fd858
        rotateLeft(&v[0x06], &carry, 1);
        rotateLeft(&v[0x08], &carry, 0);
        rotateLeft(&v[0x0a], &carry, 0);
        rotateLeft(&v[0x0c], &carry, 0);
        rotateLeft(&v[0x0e], &carry, 0);
        rotateLeft(&v[0x10], &carry, 0);
        rotateLeft(&v[0x12], &carry, 0);
        a = uintFrom(&v[0x04]);
    } while (a!=0);

    // 9fd86a
    uintSet(&v[0x06],uintFrom(&v[0x14]));
    uintSet(&v[0x08],uintFrom(&v[0x16]));
    uintSet(&v[0x0a],uintFrom(&v[0x18]));
    uintSet(&v[0x0c],uintFrom(&v[0x1a]));
    uintSet(&v[0x0e],uintFrom(&v[0x1c]));
    uintSet(&v[0x10],uintFrom(&v[0x1e]));
    uintSet(&v[0x12],uintFrom(&v[0x20]));
}

// Creates an uint from the 2 bytes defined by x
// and add a to it
// a is also changed
void uintadd(uint16 *a, unsigned char *x, char *carry){
    uint16 aux = uintFrom(x);
    unsigned int result = aux + *a;
    if(*carry != 0){
        result ++;
    }
    if(result>=(1<<16)){
        *carry = 1;
    }else{
        *carry = 0;
    }
    *a = result;
    uintSet(x,*a);
}

// OUTPUTS
//   $9f1780 == bufferPassword

// Rough transcoding from subroutine in 0x9fd474

// v needs to be initialized from 0x06 to 0x13 prior to calling this function
// v[0x00] to c[0x03] will be overwritten
void encodeCurrentPassword(unsigned char *v)
{
    // CONSTANTS
    //   $9fd7f9 == cryptoTable
    int cryptoTableSize = 22;
    unsigned char cryptoTable[] = {0x0d,0x05,0x1a,0x14,0x1a,0x0e,0x17,0x07,0x16,0x0d,0x0c,0x16,0x06,0x13,0x0d,0x1b,0x1d,0x1b,0x1d,0x1f,0x12,0x00};
    //   $9fd7d9 == passwordDictionaryArray
    int passwordDictionaryArraySize = 32;
    unsigned char passwordDictionaryArray[] = { 0x42, 0x43, 0x44, 0x46, 0x47, 0x48, 0x4a, 0x4c, 0x4d, 0x4e, 0x50, 0x51, 0x52, 0x54, 0x56, 0x57, 0x59, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x23, 0x3f, 0x28, 0x29, 0x3c, 0x3e };

    // Variables
    uint16 a,x,y;
    uint16 aux;
    char CARRY;
    // v[0x00] is counter
    // v[0x02] is aux
    // v[0x04] is unused
    // v[0x06] to v[0x12] is the data

    // Output
    unsigned char bufferPassword[PASSWORD_SIZE]; memset(bufferPassword,0,PASSWORD_SIZE*sizeof(unsigned char));

    // Make A 16 bits
    a = 0x0016; // should be a=0x16
    uintSet(&v[0x00],a);
    y = 0x0000;
    uintSet(&v[0x02],0x0000);

    do
    {
        // printf("Current password: %s\n",bufferPassword);
        // printf("Current memory: \n"); printCharArray(v,16); printCharArray(&v[16],4);
        a = uintFrom(&v[0x06]);
        x = y; // necessary for the 65816. Could've used y in the next line instead.
        aux = uintFrom(&cryptoTable[x]); a ^= aux;
        CARRY = 0;
        aux = uintFrom(&v[0x02]); a += aux;
        a &= 0x001f; // Equivalent to a%=32
        uintSet(&v[0x02],a);
        // everything up to here is equivalent to: 
        // v[$02] = ((v[$06] ^ cryptoTable[x]) + v[$02]) mod 32
        x = a;
        // Make A 8 bits
        a = passwordDictionaryArray[x];
        bufferPassword[y] = a;
        // Make A 16 bits
        // This makes the ror 17-bit rotation
        x = 5;
        do {
            rotateRight(&v[0x12], &CARRY, 1);
            rotateRight(&v[0x10], &CARRY, 0);
            rotateRight(&v[0x0e], &CARRY, 0);
            rotateRight(&v[0x0c], &CARRY, 0);
            rotateRight(&v[0x0a], &CARRY, 0);
            rotateRight(&v[0x08], &CARRY, 0);
            rotateRight(&v[0x06], &CARRY, 0);
            x--;
        } while (x>0);
        y++;
        v[0x00]--;
    } while (v[0x00]>0);
    printf("Final password: %s\n",bufferPassword);
    printf("%.4s %.4s %.4s\n%.5s %.5s",
    &bufferPassword[0],
    &bufferPassword[4],
    &bufferPassword[8],
    &bufferPassword[12],
    &bufferPassword[17]
    );

}


// Rotate right:
//
// Inputs
// Byte 1: qwertyui
// Byte 2: asdfghjk
// Carry: c
//
// Outputs:
// Byte 1: kqwertyu
// Byte 2: casdfghj
// Carry: i

void rotateRight(unsigned char *v, char *carry, int ignoreCarry){
    unsigned char byte1 = v[0];
    unsigned char byte2 = v[1];
    int byte1Carry = byte2 & 1;
    int byte2Carry = ignoreCarry? 0 : *carry;
    *carry = byte1 & 1;


    // Output first byte:
    byte1 = byte1 >> 1;
    // Set first bit accordingly
    if(byte1Carry == 0){
        BIT_CLEAR(byte1,7);
    } else {
        BIT_SET(byte1,7);
    }
    v[0]=byte1;

    // Output second byte:
    byte2 = byte2 >> 1;
    // Set first bit accordingly
    if(byte2Carry == 0){
        BIT_CLEAR(byte2,7);
    } else {
        BIT_SET(byte2,7);
    }
    v[1]=byte2;

}

// Rotate left:
//
// Inputs
// Byte 1: qwertyui
// Byte 2: asdfghjk
// Carry: c
//
// Outputs:
// Byte 1: wertyuic
// Byte 2: sdfghjkq
// Carry: a

void rotateLeft(unsigned char *v, char *carry, int ignoreCarry){
    unsigned char byte1 = v[0];
    unsigned char byte2 = v[1];
    int byte1Carry = ignoreCarry? 0 : *carry;
    int byte2Carry = byte1>>7 & 1;
    //printf("Entrou (%02X,%02X). Carry: %d\n",byte1,byte2,*carry);
    *carry = byte2>>7 & 1;


    // Output first byte:
    byte1 = byte1 << 1;
    // Set first bit accordingly
    if(byte1Carry == 0){
        BIT_CLEAR(byte1,0);
    } else {
        BIT_SET(byte1,0);
    }
    v[0]=byte1;

    // Output second byte:
    byte2 = byte2 << 1;
    // Set first bit accordingly
    if(byte2Carry == 0){
        BIT_CLEAR(byte2,0);
    } else {
        BIT_SET(byte2,0);
    }
    v[1]=byte2;

    //printf("Saiu (%02X,%02X). Carry: %d\n",byte1,byte2,*carry);
    
}

bool isPlus(uint16 x){
    return false;
}

uint16 checksum(uint16 *player1Data, uint16 *player2Data, uint16 *generalData){
    uint16 response = 0x0000;
    for(int i=0; i<PLAYER_DATA_SIZE; i++){
        response += player1Data[i]; // Always with no carry
        response += player2Data[i]; // Always with no carry
    }
    for(int i=0; i<GENERAL_DATA_SIZE; i++){
        response += generalData[i]; // Always with no carry
    }
    // Divide using SNES.WRDIVL (4204), SNES.WRDIVB(4206) and SNES.RDMPYL (4216)
    // Puts response into 4204
    // Puts 0x6d into 4206
    // Gets output from 4216 and stores into A
    // Output represents response % 0x6d 

    return response % 0x6d; // 0x6d is a prime: 109
}