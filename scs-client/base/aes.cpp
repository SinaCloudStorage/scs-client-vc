// AES.cpp: implementation of the CAES class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AES.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAES::CAES()
{
}

CAES::~CAES()
{
}

CAES::CAES(int keySize, unsigned char* keyBytes)
{
	SetNbNkNr(keySize);//初始化
	memcpy(key,keyBytes,keySize);
	KeyExpansion();
	// expand the seed key into a key schedule and store in w
}  // Aes constructor

void CAES::Cipher(unsigned char* input, unsigned char* output)  // encipher 16-bit input
{
	// state = input
	memset(&State[0][0],0,16);
	int i;
	for (i = 0; i < (4 * Nb); i++)//
	{
        State[i % 4][ i / 4] = input[i];
	}
	
	AddRoundKey(0);
	
	for (int round = 1; round <= (Nr - 1); round++)  // main round loop
	{
        SubBytes(); 
        ShiftRows();  
        MixColumns(); 
        AddRoundKey(round);
	}  // main round loop
	
	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);
	
	// output = state
	for (i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}
	
}  // Cipher()

void CAES::InvCipher(unsigned char * input, unsigned char * output)  // decipher 16-bit input
{
	// state = input
	int i; 
	memset(&State[0][0],0,16);
	for (i = 0; i < (4 * Nb); i++)
	{
		State[i % 4][ i / 4] = input[i];
	}
	
	AddRoundKey(Nr);
	
	for (int round = Nr-1; round >= 1; round--)  // main round loop
	{
        InvShiftRows();
        InvSubBytes();
        AddRoundKey(round);
        InvMixColumns();
	}  // end main round loop for InvCipher
	
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);
	
	// output = state
	for (i = 0; i < (4 * Nb); i++)
	{
        output[i] =  State[i % 4][ i / 4];
	}
	
}  // InvCipher()

void CAES::SetNbNkNr(int keyS)
{
	Nb = 4;     // block size always = 4 words = 16 bytes = 128 bits for AES
	Nk = 4;
	if (keyS == Bits128)
	{
		Nk = 4;   // key size = 4 words = 16 bytes = 128 bits
		Nr = 10;  // rounds for algorithm = 10
	}
	else if (keyS == Bits192)
	{
		Nk = 6;   // 6 words = 24 bytes = 192 bits
		Nr = 12;
	}
	else if (keyS == Bits256)
	{
		Nk = 8;   // 8 words = 32 bytes = 256 bits
		Nr = 14;
	}
}  // SetNbNkNr()



void CAES::AddRoundKey(int round)
{
	int r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {//w:    4*x+y
			State[r][c]=(unsigned char)((int)State[r][c]^(int)w[4*((round*4)+c)+r]);
        }
	}
}  // AddRoundKey()

void CAES::SubBytes()
{
	int r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] =  Sbox[ 16*(State[r][c] >> 4)+ ( State[r][c] & 0x0f) ];
        }
	}
}  // SubBytes

void CAES::InvSubBytes()
{
	int r,c;
	for (r = 0; r < 4; r++)
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] =  iSbox[ 16*( State[r][c] >> 4)+( State[r][c] & 0x0f) ];
        }
	}
}  // InvSubBytes

void CAES::ShiftRows()
{
	unsigned char temp[4*4];
	int r,c;
	for (r = 0; r < 4; r++)  // copy State into temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	//??
	for (r = 1; r < 4; r++)  // shift temp into State
	{
        for (c = 0; c < 4; c++)
        {
			State[r][c] = temp[ 4*r+ (c + r) % Nb ];
        }
	}
}  // ShiftRows()

void CAES::InvShiftRows()
{
	unsigned char temp[4*4];
	int r,c;
	for (r = 0; r < 4; r++)  // copy State into temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	for (r = 1; r < 4; r++)  // shift temp into State
	{
        for (c = 0; c < 4; c++)
        {
			State[r][ (c + r) % Nb ] = temp[4*r+c];
        }
	}
}  // InvShiftRows()

void CAES::MixColumns()
{
	unsigned char temp[4*4];
	int r,c;
	for (r = 0; r < 4; r++)  // copy State into temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	
	for (c = 0; c < 4; c++)
	{
		State[0][c] = (unsigned char) ( (int)gfmultby02(temp[0+c]) ^ (int)gfmultby03(temp[4*1+c]) ^
			(int)gfmultby01(temp[4*2+c]) ^ (int)gfmultby01(temp[4*3+c]) );
		State[1][c] = (unsigned char) ( (int)gfmultby01(temp[0+c]) ^ (int)gfmultby02(temp[4*1+c]) ^
			(int)gfmultby03(temp[4*2+c]) ^ (int)gfmultby01(temp[4*3+c]) );
		State[2][c] = (unsigned char) ( (int)gfmultby01(temp[0+c]) ^ (int)gfmultby01(temp[4*1+c]) ^
			(int)gfmultby02(temp[4*2+c]) ^ (int)gfmultby03(temp[4*3+c]) );
		State[3][c] = (unsigned char) ( (int)gfmultby03(temp[0+c]) ^ (int)gfmultby01(temp[4*1+c]) ^
			(int)gfmultby01(temp[4*2+c]) ^ (int)gfmultby02(temp[4*3+c]) );
	}
}  // MixColumns

void CAES::InvMixColumns()
{
	unsigned char temp[4*4];
	int r,c;
	for (r = 0; r < 4; r++)  // copy State into temp[]
	{
        for (c = 0; c < 4; c++)
        {
			temp[4*r+c] =  State[r][c];
        }
	}
	
	for (c = 0; c < 4; c++)
	{
		State[0][c] = (unsigned char) ( (int)gfmultby0e(temp[c]) ^ (int)gfmultby0b(temp[4+c]) ^
			(int)gfmultby0d(temp[4*2+c]) ^ (int)gfmultby09(temp[4*3+c]) );
		State[1][c] = (unsigned char) ( (int)gfmultby09(temp[c]) ^ (int)gfmultby0e(temp[4+c]) ^
			(int)gfmultby0b(temp[4*2+c]) ^ (int)gfmultby0d(temp[4*3+c]) );
		State[2][c] = (unsigned char) ( (int)gfmultby0d(temp[c]) ^ (int)gfmultby09(temp[4+c]) ^
			(int)gfmultby0e(temp[4*2+c]) ^ (int)gfmultby0b(temp[4*3+c]) );
		State[3][c] = (unsigned char) ( (int)gfmultby0b(temp[c]) ^ (int)gfmultby0d(temp[4+c]) ^
			(int)gfmultby09(temp[4*2+c]) ^ (int)gfmultby0e(temp[4*3+c]) );
	}
}  // InvMixColumns

unsigned char CAES::gfmultby01(unsigned char b)
{
	return b;
}
/*

unsigned char CAES::gfmultby02(unsigned char b)
{
	if (b < 0x80)
        return (unsigned char)(int)(b <<1);
	else
        return (unsigned char)( (int)(b << 1) ^ (int)(0x1b) );
}*/


unsigned char CAES::gfmultby03(unsigned char b)
{
	return (unsigned char) ( (int)gfmultby02(b) ^ (int)b );
}

unsigned char CAES::gfmultby09(unsigned char b)
{
	return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)b );
}

unsigned char CAES::gfmultby0b(unsigned char b)
{
	return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(b) ^
		(int)b );
}

unsigned char CAES::gfmultby0d(unsigned char b)
{
	return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(gfmultby02(b)) ^
		(int)(b) );
}

unsigned char CAES::gfmultby0e(unsigned char b)
{
	return (unsigned char)( (int)gfmultby02(gfmultby02(gfmultby02(b))) ^
		(int)gfmultby02(gfmultby02(b)) ^
		(int)gfmultby02(b) );
}

void CAES::KeyExpansion()
{
	unsigned char result[4],result2[4];
    memset(w,0,16*15);
	int row;
	for (row = 0; row < Nk; row++)//Nk=4,6,8
	{
		w[4*row+0] =  key[4*row];
		w[4*row+1] =  key[4*row+1];
		w[4*row+2] =  key[4*row+2];
		w[4*row+3] =  key[4*row+3];
	}
	
	unsigned char temp[4];
	
	for (row = Nk; row < Nb * (Nr+1); row++)
	{
        temp[0] =  w[4*(row-1)+0]; 
		temp[1] =  w[4*(row-1)+1];
        temp[2] =  w[4*(row-1)+2]; 
		temp[3] =  w[4*(row-1)+3];
		
        if (row % Nk == 0)  
        {
			RotWord(temp,result);
			SubWord(result,result2);
			memcpy(temp,result2,4);//
			
			temp[0] = (BYTE)( (int)temp[0] ^ (int) Rcon[4*(row/Nk)+0] );
			temp[1] = (BYTE)( (int)temp[1] ^ (int) Rcon[4*(row/Nk)+1] );
			temp[2] = (BYTE)( (int)temp[2] ^ (int) Rcon[4*(row/Nk)+2] );
			temp[3] = (BYTE)( (int)temp[3] ^ (int) Rcon[4*(row/Nk)+3] );
        }
        else if ( Nk > 6 && (row % Nk == 4) )  
        {
			SubWord(temp,result);
			memcpy(temp,result,4);
        }
        // w[row] = w[row-Nk] xor temp
		w[4*row+0] = (BYTE) ( (int) w[4*(row-Nk)+0] ^ (int)temp[0] );
		w[4*row+1] = (BYTE) ( (int) w[4*(row-Nk)+1] ^ (int)temp[1] );
		w[4*row+2] = (BYTE) ( (int) w[4*(row-Nk)+2] ^ (int)temp[2] );
		w[4*row+3] = (BYTE) ( (int) w[4*(row-Nk)+3] ^ (int)temp[3] );
	}  // for loop
}  // KeyExpansion()

void CAES::SubWord(unsigned char * word,unsigned char* result)
{//不要这样返回！
	result[0] =  Sbox[ 16*(word[0] >> 4)+ (word[0] & 0x0f) ];
	result[1] =  Sbox[ 16*(word[1] >> 4)+ (word[1] & 0x0f) ];
	result[2] =  Sbox[ 16*(word[2] >> 4)+ (word[2] & 0x0f) ];
	result[3] =  Sbox[ 16*(word[3] >> 4)+ (word[3] & 0x0f) ];
}

void CAES::RotWord(unsigned char * word,unsigned char *result)
{//不要这样返回
	result[0] = word[1];
	result[1] = word[2];
	result[2] = word[3];
	result[3] = word[0];
}

void CAES::Dump()
{
	//Console.WriteLine("Nb = " + Nb + " Nk = " + Nk + " Nr = " + Nr);
	//Console.WriteLine("\nThe key is \n" + DumpKey() );
	//Console.WriteLine("\nThe Sbox is \n" + DumpTwoByTwo(Sbox));
	//Console.WriteLine("\nThe w array is \n" + DumpTwoByTwo(w));
	//Console.WriteLine("\nThe State array is \n" + DumpTwoByTwo(State));
}

unsigned char * CAES::DumpKey()
{
	//	string s = "";
	//	for (int i = 0; i < key.Length; i++)
	//      s += key[i].ToString("x2") + " ";
	//return s;
	return NULL;
}

unsigned char * CAES::DumpTwoByTwo(unsigned char *  a)
{/*
	string s ="";
	for (int r = 0; r < a.GetLength(0); r++)
	{
	s += "["+r+"]" + " ";
	for (int c = 0; c < a.GetLength(1); c++)
	{
	s += a[r,c].ToString("x2") + " " ;
	}
	s += "\n";
	}
	return s;*/
	return NULL;
}
