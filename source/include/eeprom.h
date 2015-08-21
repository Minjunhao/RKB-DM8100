void nvr_Init(void);
void iic_Init(void);
void iic_Delay(void);
void iic_Start(void);
void nvr_Stop(void);
void iic_Stop(void);
void nvr_StopWrite(void);
void iic_StopWrite(void);
unsigned char iic_getch(void);
unsigned char iic_getAck(void);
void iic_putch(unsigned char data);
void iic_putAck(void);
void iic_putNoAck(void);
void nvr_SetWrite(unsigned char addr);
void nvr_SetRead(unsigned char addr);
unsigned char nvr_GetData(void);
unsigned char nvr_GetData_NoAck(void);
void nvr_PutData(unsigned char data);
unsigned char nvr_xGetData(unsigned char addr);
unsigned char nvr_xGetDataPage1(unsigned char addr);
void nvr_xPutData(unsigned char addr, unsigned char data);
void nvr_xPutDataPage1(unsigned char addr, unsigned char data);
unsigned char nvr_GetChksum(unsigned char start_addr, unsigned char end_addr);
unsigned char nvr_GetChksumPage1(unsigned char start_addr, unsigned char end_addr);
unsigned char nvr_CheckChksum(void);
unsigned char nvr_CheckChksumPage1(void);
void nvr_AdjChksum(void);
void nvr_AdjChksumPage1(void);
void Set_Delay(int count);

void nvr_PutDataPage0(void);	/* 256 byte */
void nvr_PutDataPage1(void);	/* 256 byte */
void nvr_PutDataPage2(void);	/* 256 byte */
void nvr_PutDataPage3(void);	/* 256 byte */
void nvr_PutDataPage4(void);	/* 256 byte */	//20060201
void nvr_PutDataPage5(void);	/* 256 byte */
void nvr_PutDataPage6(void);	/* 256 byte */
void nvr_PutDataPage7(void);	/* 256 byte */
void nvr_GetDataPage0(void);	/* 256 byte */
void nvr_GetDataPage1(void);	/* 256 byte */
void nvr_GetDataPage2(void);	/* 256 byte */
void nvr_GetDataPage3(void);	/* 256 byte */
void nvr_GetDataPage4(void);	/* 256 byte */	//20060201
void nvr_GetDataPage5(void);	/* 256 byte */
void nvr_GetDataPage6(void);	/* 256 byte */
void nvr_GetDataPage7(void);	/* 256 byte */

//CMedia 20121101
void CMiic_Init(void);
void CM_xGetData(unsigned char addr, unsigned char *data1, unsigned char *data2);
void CM_xPutData(unsigned char addr, unsigned char data1, unsigned char data2);
