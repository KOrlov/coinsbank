#pragma once
#include "../GenericDevice.h"
#include "CRT_310.h"
#include "XFSIDC.H"


class IDCCRT310 :
	public GenericDevice
{
private:


	std::string port;
	std::string configName;
	int baudRate;	
	WFSIDCSTATUS status;
	WFSIDCCAPS caps;
	HRESULT hResult;
	int retainBinTreshold = 0;
	BYTE cpuType; //current  insert protocol T0/R=T1
	//events
	void sendWFS_EXEE_IDC_INVALIDTRACKDATA(HSERVICE hService, WORD fwStatus, std::string track, std::string data);
	void sendWFS_EXEE_IDC_MEDIAINSERTED(HSERVICE hService);
	void sendWFS_SRVE_IDC_MEDIAREMOVED(HSERVICE hService);
	void sendWFS_EXEE_IDC_MEDIARETAINED(HSERVICE hService);
	void sendWFS_EXEE_IDC_INVALIDMEDIA(HSERVICE hService);
	void sendWFS_SRVE_IDC_CARDACTION(HSERVICE hService,WORD wAction, WORD wPosition);
	void sendWFS_USRE_IDC_RETAINBINTHRESHOLD(HSERVICE hService, WORD fwRetainBin);
	void sendWFS_SRVE_IDC_MEDIADETECTED(HSERVICE hService, WORD wResetOut);
	void sendWFS_SRVE_IDC_RETAINBINREMOVED(HSERVICE hService);
	void sendWFS_SRVE_IDC_RETAINBININSERTED(HSERVICE hService);
	void sendWFS_EXEE_IDC_INSERTCARD(HSERVICE hService);
	void sendWFS_SRVE_IDC_DEVICEPOSITION(HSERVICE hService, WORD wPosition);
	void sendWFS_SRVE_IDC_POWER_SAVE_CHANGE(HSERVICE hService, USHORT usPowerSaveRecoveryTime);
	void sendWFS_EXEE_IDC_TRACKDETECTED(HSERVICE hService, WORD fwTracks);
	void sendWFS_EXEE_IDC_EMVCLESSREADSTATUS();

	//exec
	void execWFS_CMD_IDC_READ_RAW_DATA(REQUESTID reqId);
	void execWFS_CMD_IDC_RETAIN_CARD(REQUESTID reqId);
	void execWFS_CMD_IDC_EJECT_CARD(REQUESTID reqId);
	void execWFS_CMD_IDC_RESET_COUNT(REQUESTID reqId);
	void execWFS_CMD_IDC_RESET(REQUESTID reqId);
	void execWFS_CMD_IDC_SET_GUIDANCE_LIGHT(REQUESTID reqId);
	void execWFS_CMD_IDC_CHIP_IO(REQUESTID reqId);
	void execWFS_CMD_IDC_CHIP_POWER(REQUESTID reqId);

	//supplementary
	void updateCardStatus(HANDLE port);
	void loadRetainedCount();
	void loadCapabilities();
	void saveRetainedCount();
	void checkEc(int ec);
public:


	IDCCRT310(volatile SharedData* s, unsigned short version, std::string, int, std::string);
	~IDCCRT310();


	void getInfoProc(REQUESTID reqId) override;
	void executeProc(REQUESTID reqId) override;
	void copyLpInput(LPVOID lpData, REQUESTID reqId)override;

};

