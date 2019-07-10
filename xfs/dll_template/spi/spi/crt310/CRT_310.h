#pragma comment(lib, "CRT_310.lib" )

#define ERR		-1
#define OK		0

#define ERRCARD	2
#define ERRRDCARD	3
#define ERRWRCARD	4
#define ERRCARDSEC	5
#define ERRCARDKEY	6
#define ERRCARDLOCKED 7
#define ERRMSG 8
#define ERRRFCARD	9
#define ERRFORMAT	10
#define ERROVERFLOW	11

#define NOCARD	1
#define UNKNOWCARD	12
#define ERRCARDPOSITION	14


#define PAC_ADDRESS	1021

#define ENQ  0x05//请求连接通信线路(询问).
#define ACK  0x06//确认(握手).
#define NAK  0x15//通信忙.
#define EOT  0x04//通信结束(传送结束).
#define CAN  0x18//解除通信(取消).
#define STX  0x02//数据包起始符(起始字符).
#define ETX  0x03//数据包结束符(终结符).
#define US   0x1F//数据分隔符.

int APIENTRY GetSysVerion(HANDLE ComHandle, char *strVerion);
int APIENTRY GetErrCode(int *Errorcode);


HANDLE APIENTRY CommOpen(char *Port);   //Open Comm. port with 9600 bps

//_BaudOption=
	//	1=>   Baut Rate is 1200 bps  
	//	2=>   Baut Rate is 2400 bps  
	//	3=>   Baut Rate is 4800 bps  
	//	4=>   Baut Rate is 9600 bps  
	//	5=>   Baut Rate is 19200 bps  
	//	6=>   Baut Rate is 38400 bps  
	//	7=>   Baut Rate is 57600 bps  
HANDLE APIENTRY CommOpenWithBaut(char *Port, BYTE _BaudOption); 

int APIENTRY CommClose(HANDLE ComHandle);

int APIENTRY CommSetting(HANDLE ComHandle,char *ComSeting);

//_BaudOption=
	//	1=>   Baut Rate is 1200 bps  
	//	2=>   Baut Rate is 2400 bps  
	//	3=>   Baut Rate is 4800 bps  
	//	4=>   Baut Rate is 9600 bps  
	//	5=>   Baut Rate is 19200 bps  
	//	6=>   Baut Rate is 38400 bps  
	//	7=>   Baut Rate is 57600 bps  
int APIENTRY CRT_R_SetComm(HANDLE ComHandle, BYTE _BaudOption);
////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY CRT310_Reset(HANDLE ComHandle, BYTE _Eject);
int APIENTRY CRT310_CardSetting(HANDLE ComHandle,BYTE _CardIn, BYTE _EnableBackIn);
int APIENTRY CRT310_CardPosition(HANDLE ComHandle, BYTE _Position);
int APIENTRY CRT310_GetStatus(HANDLE ComHandle, BYTE *_atPosition, BYTE *_frontSetting, BYTE *_rearSetting);
int APIENTRY CRT310_SensorStatus(HANDLE ComHandle, BYTE *_PSS0,BYTE *_PSS1, BYTE *_PSS2, BYTE *_PSS3, BYTE *_PSS4, BYTE *_PSS5, BYTE *_CTSW, BYTE *_KSW);
int APIENTRY CRT310_MovePosition(HANDLE ComHandle, BYTE _Position);
int APIENTRY CRT310_LEDSet(HANDLE ComHandle, BYTE _ONOFF);
int APIENTRY CRT310_LEDTime(HANDLE ComHandle,BYTE _T1,BYTE _T2);

int APIENTRY MC_ReadTrack(HANDLE ComHandle, BYTE _mode, BYTE _track, int *_BlockDataLen,BYTE _BlockData[]);

int APIENTRY CRT310_ReadSnr(HANDLE ComHandle, BYTE _SNData[], BYTE *_dataLen);
int APIENTRY CRT310_WriteSnr(HANDLE ComHandle, BYTE _SNData[], BYTE _dataLen);
////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY CRT_IC_CardOpen(HANDLE ComHandle);
int APIENTRY CRT_IC_CardClose(HANDLE ComHandle);
int APIENTRY CRT_R_DetectCard(HANDLE ComHandle,BYTE *_CardType,BYTE *_CardInfor);

int APIENTRY RF_DetectCard(HANDLE ComHandle);
int APIENTRY RF_GetCardID(HANDLE ComHandle, BYTE *_CardIDLen, BYTE _CardID[]);
int APIENTRY RF_ChangeSecKey(HANDLE ComHandle, BYTE _Sec, BYTE _KEYLen, BYTE _KEY[]);
int APIENTRY RF_LoadSecKey(HANDLE ComHandle, BYTE _Sec, BYTE _KEYType, BYTE _KEYLen, BYTE _KEY[]);
int APIENTRY RF_ReadBlock(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE *_BlockDatalen, BYTE _BlockData[]);
int APIENTRY RF_ReadMoreBlock(HANDLE ComHandle, BYTE _Sec, BYTE _StartBlock, BYTE _BlockNumber, BYTE *_BlockDatalen, BYTE _BlockData[]);
int APIENTRY RF_WriteBlock(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE _BlockDataLen, BYTE _BlockData[]);
int APIENTRY RF_WriteMoreBlock(HANDLE ComHandle, BYTE _Sec, BYTE _StartBlock, BYTE _BlockNumber, BYTE _BlockDatalen, BYTE _BlockData[]);

int APIENTRY RF_InitValue(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE _DataLen, BYTE _Data[]);
int APIENTRY RF_ReadValue(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE * _DataLen, BYTE _Data[]);
int APIENTRY RF_Increment(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE  _DataLen, BYTE _Data[]);
int APIENTRY RF_Decrement(HANDLE ComHandle, BYTE _Sec, BYTE _Block, BYTE  _DataLen, BYTE _Data[]);

int APIENTRY CPU_SelRfCard(HANDLE ComHandle,BYTE _CPUType,BYTE _exData[], int *_exdataLen);
int APIENTRY CPU_GetRfCardID(HANDLE ComHandle, BYTE *_CardIDLen, BYTE _CardID[]);
int APIENTRY CPU_SendRfAPDU(HANDLE ComHandle, int _dataLen, BYTE _APDUData[], BYTE _exData[], int *_exdataLen);
int APIENTRY CPU_DESelRfCard(HANDLE ComHandle);

int APIENTRY IC24CXX_DetectCard(HANDLE ComHandle,BYTE *_CardType);//旧,为了兼容，故保留
int APIENTRY IC24CXX_SetCard(HANDLE ComHandle,BYTE _CardType); //新  
int APIENTRY IC24CXX_ReadBlock(HANDLE ComHandle, BYTE _CardType, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY IC24CXX_WriteBlock(HANDLE ComHandle, BYTE _CardType, int _Address, BYTE _dataLen, BYTE _BlockData[]);

int APIENTRY CPU_ColdReset(HANDLE ComHandle, BYTE _CPUMode, BYTE *_CPUType,BYTE _exData[], int *_exdataLen);
int APIENTRY CPU_WarmReset(HANDLE ComHandle, BYTE *_CPUType,BYTE _exData[], int *_exdataLen);
int APIENTRY CPU_T0_C_APDU(HANDLE ComHandle, int _dataLen, BYTE _APDUData[], BYTE _exData[], int *_exdataLen);
int APIENTRY CPU_T1_C_APDU(HANDLE ComHandle, int _dataLen, BYTE _APDUData[], BYTE _exData[], int *_exdataLen);

int APIENTRY SIM_Reset(HANDLE ComHandle,BYTE _VOLTAGE,BYTE _SIMNo, BYTE *_SIMTYPE,BYTE _exData[], int *_exdataLen);
int APIENTRY SIM_T0_C_APDU(HANDLE ComHandle, BYTE SIMNo, int _dataLen, BYTE _APDUData[], BYTE _exData[], int *_exdataLen);
int APIENTRY SIM_T1_C_APDU(HANDLE ComHandle, BYTE SIMNo, int _dataLen, BYTE _APDUData[], BYTE _exData[], int *_exdataLen);
int APIENTRY SIM_CardClose(HANDLE ComHandle);

int APIENTRY SLE4442_Reset(HANDLE ComHandle);
int APIENTRY SLE4442_Read(HANDLE ComHandle, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4442_ReadP(HANDLE ComHandle, BYTE _BlockDataLen, BYTE _BlockData[]);
int APIENTRY SLE4442_ReadS(HANDLE ComHandle, BYTE _BlockDataLen, BYTE _BlockData[]);
int APIENTRY SLE4442_VerifyPWD(HANDLE ComHandle, BYTE _PWDataLen, BYTE _PWData[]);
int APIENTRY SLE4442_Write(HANDLE ComHandle, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4442_WriteP(HANDLE ComHandle, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4442_WritePWD(HANDLE ComHandle, BYTE _PWDatalen, BYTE _PWData[]);

int APIENTRY SLE4428_Reset(HANDLE ComHandle);
int APIENTRY SLE4428_Read(HANDLE ComHandle, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4428_ReadP(HANDLE ComHandle, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4428_VerifyPWD(HANDLE ComHandle, BYTE _PWDataLen, BYTE _PWData[]);
int APIENTRY SLE4428_WritePWD(HANDLE ComHandle,BYTE _PWDataLen, BYTE _PWDataOld[],BYTE _PWDataNew[]);
int APIENTRY SLE4428_Write(HANDLE ComHandle, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY SLE4428_WriteP(HANDLE ComHandle, int _Address, BYTE _dataLen, BYTE _BlockData[]);

int APIENTRY AT88SC102_Reset(HANDLE ComHandle);
int APIENTRY AT88SC102_Security1Clear(HANDLE ComHandle, BYTE _Index, BYTE _Address, BYTE _dataLen);
int APIENTRY AT88SC102_Security2ClearApp1(HANDLE ComHandle,BYTE _Security2App1PWLen,BYTE _Security2App1PW[]);
int APIENTRY AT88SC102_Security2ClearApp2(HANDLE ComHandle,BYTE _EC2, BYTE _Security2App2PWLen, BYTE _Security2App2PW[]);
int APIENTRY AT88SC102_Read(HANDLE ComHandle, BYTE _Index, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC102_VerifyPWD(HANDLE ComHandle, BYTE _PWDataLen, BYTE _PWData[]);
int APIENTRY AT88SC102_Write(HANDLE ComHandle, BYTE _Index, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC102_WritePWD(HANDLE ComHandle, BYTE _PWIndex, BYTE _PWDataLen,BYTE _PWData[]);
int APIENTRY AT88SC102_InitSecurity2(HANDLE ComHandle, BYTE _CtrlMode);
int APIENTRY AT88SC102_DisableEC2(HANDLE ComHandle);

int APIENTRY AT88SC1604_Clear(HANDLE ComHandle, BYTE _Index, int _Address, BYTE _dataLen);
int APIENTRY AT88SC1604_Personalization(HANDLE ComHandle,BYTE _data);
int APIENTRY AT88SC1604_Read(HANDLE ComHandle, BYTE _Index, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC1604_Reset(HANDLE ComHandle);
int APIENTRY AT88SC1604_VerifyPWD(HANDLE ComHandle, BYTE _PWIndex,BYTE _PWDataLen, BYTE _PWData[]);
int APIENTRY AT88SC1604_Write(HANDLE ComHandle, BYTE _Index, int _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC1604_WritePWD(HANDLE ComHandle, BYTE _PWIndex,BYTE _PWDataLen,BYTE _PWData[]);

int APIENTRY AT88SC1608_Reset(HANDLE ComHandle);
int APIENTRY AT88SC1604_VerifyPWD(HANDLE ComHandle, BYTE _PWIndex,BYTE _PWDataLen, BYTE _PWData[]);
int APIENTRY AT88SC1608_Read(HANDLE ComHandle, BYTE _Index, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC1608_Write(HANDLE ComHandle, BYTE _Index, BYTE _Address, BYTE _dataLen, BYTE _BlockData[]);
int APIENTRY AT88SC1608_ReadFUSE(HANDLE ComHandle, BYTE * _PER, BYTE * _CMA, BYTE * _FAB);
int APIENTRY AT88SC1608_WriteFUSE(HANDLE ComHandle);
int APIENTRY AT88SC1608_InitAuth(HANDLE ComHandle, BYTE _QLen, BYTE _Q[]);
int APIENTRY AT88SC1608_VerifyAuth(HANDLE ComHandle, BYTE _QLen, BYTE _Q[]);

int APIENTRY AT45D041_Reset(HANDLE ComHandle);
int APIENTRY AT45D041_Read(HANDLE ComHandle, int  _Address, int *_BlockDataLen, BYTE _BlockData[]);
int APIENTRY AT45D041_Write(HANDLE ComHandle, int  _Address, int _BlockDataLen, BYTE _BlockData[]);
