#include<cstdio>
#include<cstring>

using namespace std;

typedef unsigned short int uint16;

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1<<(b)))) // '!!' to make sure this returns 0 or 1

#define MEMORY_SIZE 0x22 // Memory used (in bytes) in the whole process - highest access on v[0x20] uint16
#define PASSWORD_SIZE 0x16 // Total number of characters in the final password string
#define PLAYER_DATA_SIZE 13 // Number of player related inputs
#define GENERAL_DATA_SIZE 3 // Number of non-player related inputs

void uintSet(unsigned char *v, uint16 x);
uint16 uintFrom(unsigned char *v);

void encodeCurrentPassword(unsigned char *v);

void initializeAValues(uint16 *playerDataA, uint16 *generalDataA);
void initializeInputValues(uint16 *player1Data, uint16 *player2Data, uint16 *generalData);
void initializeMemory(unsigned char *v,
    uint16 *playerDataA, uint16 *generalDataA,
    uint16 *player1Data, uint16 *player2Data, uint16 *generalData);
void initializeVariables(unsigned char *v, uint16 a, uint16 x);
void uintadd(uint16 *a, unsigned char *x, char *carry);
void rotateRight(unsigned char *v, char *carry, int ignoreCarry);
void rotateLeft(unsigned char *v, char *carry, int ignoreCarry);
bool isPlus(uint16 x);
uint16 checksum(uint16 *player1Data, uint16 *player2Data, uint16 *generalData);



int main(){
    unsigned char v[MEMORY_SIZE]; // Memory used for the process
    uint16 playerDataA[PLAYER_DATA_SIZE]; // Constants related to each input
    uint16 generalDataA[GENERAL_DATA_SIZE]; // Constants related to each input
    uint16 playerData[2][PLAYER_DATA_SIZE]; // Variable input
    uint16 generalData[GENERAL_DATA_SIZE]; // Variable input

    initializeAValues(playerDataA, generalDataA);

    initializeInputValues(playerData[0], playerData[1], generalData);
    initializeMemory(v,playerDataA,generalDataA,playerData[0],playerData[1],generalData);
    encodeCurrentPassword(v);
}

// These are the constants used for each input respective A value
// They are put into the A register before each call to $9fd819 subroutine
// The first set is made at $9fd350 and all others happen in the same page
void initializeAValues(uint16 *playerDataA, uint16 *generalDataA){
    memset(playerDataA,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(generalDataA,0,GENERAL_DATA_SIZE*sizeof(uint16));

    playerDataA[0]=0x0004;
    playerDataA[1]=0x0004;
    playerDataA[2]=0x0004;
    playerDataA[3]=0x0004;
    playerDataA[4]=0x0004;
    playerDataA[5]=0x0004;
    playerDataA[6]=0x0005;
    playerDataA[7]=0x0005;
    playerDataA[8]=0x0005;
    playerDataA[9]=0x0008;
    playerDataA[10]=0x0002;
    playerDataA[11]=0x000a;
    playerDataA[12]=0x0500;

    generalDataA[0]=0x0003;
    generalDataA[1]=0x0041;
    generalDataA[2]=0x0002;
}

void initializeInputValues(uint16 *player1Data, uint16 *player2Data, uint16 *generalData){
    memset(player1Data,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(player2Data,0,PLAYER_DATA_SIZE*sizeof(uint16));
    memset(generalData,0,GENERAL_DATA_SIZE*sizeof(uint16));

    // This section has potential for exploration.
    // As the memory usually holds a lot more space than the usual values, weird things may happen if the values are chosen correctly
    // For instance, choosing everything 0, but the region number 16 will be valid, but freeze the game.

    // Player equipment values are all in the range 0-3
    player1Data[0]=0; // $1d03 - P1 engineLevel
    player2Data[0]=0; // $1d2c - P2 engineLevel

    player1Data[1]=0; // $1d05 - P1 wetTireLevel
    player2Data[1]=0; // $1d2e - P2 wetTireLevel

    player1Data[2]=0; // $1d07 - P1 dryTireLevel
    player2Data[2]=0; // $1d30 - P2 dryTireLevel

    player1Data[3]=0; // $1d09 - P1 gearboxLevel
    player2Data[3]=0; // $1d32 - P2 gearboxLevel

    player1Data[4]=0; // $1d0b - P1 nitroLevel
    player2Data[4]=0; // $1d34 - P2 nitroLevel

    // Thanks rimsky82 for the discovery
    player1Data[5]=0; // $1d0d - P1 Gas Tank
    player2Data[5]=0; // $1d36 - P2 Gas Tank

    player1Data[6]=0; // $1d0f - P1 sideArmorLevel
    player2Data[6]=0; // $1d38 - P2 sideArmorLevel

    player1Data[7]=0; // $1d11 - P1 rearArmorLevel
    player2Data[7]=0; // $1d3a - P2 rearArmorLevel

    player1Data[8]=0; // $1d13 - P1 frontArmorLevel
    player2Data[8]=0; // $1d3c - P2 frontArmorLevel

    // Values: 0-7
    // Red, Blue, White, Yellow, Green, Black, Gray, Purple
    player1Data[9]=0x0000; // $1d15 - P1 color
    player2Data[9]=0x0001; // $1d3e - P2 color

    // Values: 0-1
    // MPH, KPH
    player1Data[10]=0; // $1cff - P1 speedType (MPH/KPH)
    player2Data[10]=0; // $1d28 - P2 speedType (MPH/KPH)

    // Values:
    // Left byte goes from 0-4 on control types
    // Right byte goes:
    // 0x00: Manual
    // 0x80: Automatic
    // Example, automatic with control type 2 would be:
    // 0x8001
    player1Data[11]=0x8000; // $1d01 - P1 gear type and control type
    player2Data[11]=0x8000; // $1d2a - P2 gear type and control type

    // Values: 0-255 (There are 2 bytes here, in theory, we can go up to 0xffff)
    // Anyway, 255 is enough to buy everything
    player1Data[12]=0; // $1d19 - P1 Money (Thousands)
    player2Data[12]=0; // $1d42 - P2 Money (Thousands)

    // Values: 0-2
    // Amateur, Professional, Championship
    generalData[0]=0; // $1cdd - Difficulty

    // Values: 0-15
    // Australasia, Britain, ...
    generalData[1]=0; // $1ce1 - Region Number

    // Values: 0-1
    // 1 player, 2 players
    generalData[2]=0; // $1cdb - Number of players (1 or 2)
}

void initializeMemory(unsigned char *v, 
    uint16 *playerDataA, uint16 *generalDataA,
    uint16 *player1Data, uint16 *player2Data, uint16 *generalData){
    uint16 a, x;

    // Re-setting happens from 9fd33f
    // The only reset 2-bytes are from 0x06 to 0x12
    // Here we reset everything as we won't use any prior or posterior side-effects.
    memset(v,0,MEMORY_SIZE*sizeof(unsigned char));

    // From 9fd34d
    for(int j=0;j<2;j++){
        for(int i=0; i<=10; i++){
            x = j==0 ? player1Data[i]: player2Data[i];
            a = playerDataA[i];
            initializeVariables(v,a,x);
        }

        a = (j==0 ? player1Data[11] : player2Data[11]);
        if(!isPlus(a)){
            a = (a & 0x7fff) + 0x0005;
        }
        x = a;
        a = 0x000a;
        initializeVariables(v,a,x);

        x = (j==0 ? player1Data[12]: player2Data[12]);
        a = playerDataA[12];
        initializeVariables(v,a,x);
    }

    // General data part

    // Difficulty
    x = generalData[0];
    a = generalDataA[0];
    initializeVariables(v,a,x);

    // Region number
    a = generalData[1];
    a = a << 2;
    x = a;
    a = generalDataA[1];
    initializeVariables(v,a,x);

    // Number of players
    x = generalData[2];
    a = generalDataA[2];
    initializeVariables(v,a,x);

    // 9fd46a - Calculate checksum
    a = checksum(player1Data,player2Data, generalData);
    x = a;
    a = 0x006d;
    initializeVariables(v,a,x);

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
    // int cryptoTableSize = 22;
    //                               13,   5,  26,  20,  26,  14,  23,   7,  22,  13,  12,  22,   6,  19,  13,  11,  13,  11,  13,  31,  18,   0
    unsigned char cryptoTable[] = {0x0d,0x05,0x1a,0x14,0x1a,0x0e,0x17,0x07,0x16,0x0d,0x0c,0x16,0x06,0x13,0x0d,0x1b,0x1d,0x1b,0x1d,0x1f,0x12,0x00};
    //   $9fd7d9 == passwordDictionaryArray
    // int passwordDictionaryArraySize = 32;
    // Dictionary of letters, it goes "BCDF"... as it is shown in the password menu
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
    // printf("Final password: %s\n",bufferPassword);
    printf("%.4s %.4s %.4s\n%.5s %.5s\n\n",
    &bufferPassword[0],
    &bufferPassword[4],
    &bufferPassword[8],
    &bufferPassword[12],
    &bufferPassword[17]
    );

}

// At $9fd74f
uint16 checksum(uint16 *player1Data, uint16 *player2Data, uint16 *generalData){
    uint16 response = 0x0000;
    for(int i=0; i<PLAYER_DATA_SIZE; i++){
        response += player1Data[i]; // Always with no carry
        response += player2Data[i]; // Always with no carry
    }
    for(int i=0; i<GENERAL_DATA_SIZE; i++){
        response += generalData[i]; // Always with no carry
    }

    return response % 0x6d; // 0x6d is a prime: 109
}

// CPU functions

// Sets v[0] and v[1] with the value from x using little endian
// Equivalent to STA v in 16-bit mode
void uintSet(unsigned char *v, uint16 a){
    unsigned char lessSignificantByte = a;
    unsigned char mostSignificantByte = a>>8;
    v[0] = lessSignificantByte;
    v[1] = mostSignificantByte;
}

// Gets a uint16 from the values of v[0] and v[1] using little endian
// Equivalent to LDA v in 16-bit mode
uint16 uintFrom(unsigned char *v){
    uint16 resp = v[1] << 8;
    resp |= v[0];
    return resp;
}

// CPU instructions. It comes in 2 formats:
// LSR (Logical Shift Right) will ignore (use zero as) carry input and set carry output
// ROR (Rotate Right) will use the carry input and set carry output
// Here, the option is made by the "ignoreCarry" parameter
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

// CPU instructions. It comes in 2 formats:
// ASL (Accumulator Shift Left) will ignore (use zero as) carry input and set carry output
// ROL (Rotate Right) will use the carry input and set carry output
// Here, the option is made by the "ignoreCarry" parameter
void rotateLeft(unsigned char *v, char *carry, int ignoreCarry){
    unsigned char byte1 = v[0];
    unsigned char byte2 = v[1];
    int byte1Carry = ignoreCarry? 0 : *carry;
    int byte2Carry = byte1>>7 & 1;
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
}

// Equivalent to checking the negative flag, i.e. checking if the MSB is 1
bool isPlus(uint16 a){
    return a>>15 == 0;
}

