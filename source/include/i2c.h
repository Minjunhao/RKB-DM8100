//------------------------------------------------------------
//typedef unsigned char 	byte;
//typedef unsigned short 	word;
//typedef unsigned char 	BYTE;
//typedef unsigned short 	WORD;
/*
typedef union {
	unsigned char Byte;
	struct {
		unsigned char bit0:1;
		unsigned char bit1:1;
		unsigned char bit2:1;
		unsigned char bit3:1;
		unsigned char bit4:1;
		unsigned char bit5:1;
		unsigned char bit6:1;
		unsigned char bit7:1;
	}byte_bit;
} byte_flags;
*/
typedef union W_TEMP1{
	BYTE	valB[2];	//B[]={0xaa,0xbb}
	WORD	valW;		//W=0xaabb
}W_TEMP1;

#define I2C_24CXX		0xA0   //24C04/08/16
#define I2C_EEP_ADDR		I2C_24CXX
//------------------------------------------------------------
#define WriteData	IICSW_Write
#define ReadData	IICSW_Read

#define DebugInfo   WriteData2

extern void IICSW_Write(BYTE,BYTE,BYTE *,WORD);
extern void IICSW_Read(BYTE,BYTE,BYTE *,WORD);
extern void EEPROM_Write(WORD,BYTE *,WORD);
extern void EEPROM_Read(WORD,BYTE *,WORD);
extern   void Initial_I2C(void);
void Delay_10u(int count);
void Delay_2u(int count);

