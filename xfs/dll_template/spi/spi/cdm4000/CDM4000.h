#pragma once
#include "GenericDevice.h"
#include "XFSCDM.H"
#include "MFSCommCDM.h"
class CDM4000 :
	public GenericDevice
{


private :
	std::string configName;
	//suplementary
	void initCaps();
	void initStatus();
	void initCashUnits();
	void getStoredData();	
	void setStoredData();
	std::vector<string> split(std::string);
	LPWFSCDMCASHUNIT  constructLPWFSCDMCASHUNIT(LPVOID pResult,  int unit);
	bool exchangeActive = false;

	//CDM-related
	HWND cdmHWND;
	CDMCFGSTATUS cdmCfgstatus;
	int port;
	bool cdmOpened = false;
	bool openCdm();
	bool closeCdm();
	void checkStatus();
	void updateCashUnitStatusFromCounters(int n);
	void updateCashUnitStatusFromCountersAndRaiseEvent(HSERVICE hService,int n);

	WFSCDMSTATUS status;
	WFSCDMCAPS caps;
	std::vector<WFSCDMCASHUNIT> cashUnits;
	std::vector<WFSCDMPHCU> phcu;
	std::vector<std::string> cuNames;
	std::vector<std::string> phcuPositions;
	int maxDispense;
	bool nearEndHardware[5];
	std::string errLine = "";

	void execWFS_CMD_CDM_DENOMINATE(REQUESTID reqId);
	void execWFS_CMD_CDM_DISPENSE(REQUESTID reqId);
	void execWFS_CMD_CDM_COUNT(REQUESTID reqId);
	void execWFS_CMD_CDM_PRESENT(REQUESTID reqId);
	void execWFS_CMD_CDM_REJECT(REQUESTID reqId);
	void execWFS_CMD_CDM_RETRACT(REQUESTID reqId);
	void execWFS_CMD_CDM_OPEN_SHUTTER(REQUESTID reqId);
	void execWFS_CMD_CDM_CLOSE_SHUTTER(REQUESTID reqId);
	void execWFS_CMD_CDM_SET_TELLER_INFO(REQUESTID reqId);
	void execWFS_CMD_CDM_SET_CASH_UNIT_INFO(REQUESTID reqId);
	void execWFS_CMD_CDM_START_EXCHANGE(REQUESTID reqId);
	void execWFS_CMD_CDM_END_EXCHANGE(REQUESTID reqId);
	void execWFS_CMD_CDM_OPEN_SAFE_DOOR(REQUESTID reqId);
	void execWFS_CMD_CDM_CALIBRATE_CASH_UNIT(REQUESTID reqId);
	void execWFS_CMD_CDM_SET_MIX_TABLE(REQUESTID reqId);
	void execWFS_CMD_CDM_RESET(REQUESTID reqId);
	void execWFS_CMD_CDM_TEST_CASH_UNITS(REQUESTID reqId);
	void execWFS_CMD_CDM_SET_GUIDANCE_LIGHT(REQUESTID reqId);
	void execWFS_CMD_CDM_POWER_SAVE_CONTROL(REQUESTID reqId);
	void execWFS_CMD_CDM_PREPARE_DISPENSE(REQUESTID reqId);
	void execWFS_CMD_CDM_SET_BLACKLIST(REQUESTID reqId);
	void execWFS_CMD_CDM_SYNCHRONIZE_COMMAND(REQUESTID reqId);


	void getWFS_INF_CDM_STATUS(REQUESTID reqId);
	void getWFS_INF_CDM_CAPABILITIES(REQUESTID reqId);
	void getWFS_INF_CDM_CASH_UNIT_INFO(REQUESTID reqId);
	void getWFS_INF_CDM_TELLER_INFO(REQUESTID reqId);
	void getWFS_INF_CDM_CURRENCY_EXP(REQUESTID reqId);
	void getWFS_INF_CDM_MIX_TYPES(REQUESTID reqId);
	void getWFS_INF_CDM_MIX_TABLE(REQUESTID reqId);
	void getWFS_INF_CDM_PRESENT_STATUS(REQUESTID reqId);
	void getWFS_INF_CDM_GET_ITEM_INFO(REQUESTID reqId);
	void getWFS_INF_CDM_GET_BLACKLIST(REQUESTID reqId);
	void getWFS_INF_CDM_GET_ALL_ITEMS_INFO(REQUESTID reqId);


	void sendWFS_SRVE_CDM_SAFEDOOROPEN(HSERVICE hService);
	void sendWFS_SRVE_CDM_SAFEDOORCLOSED(HSERVICE hService);
	void sendWFS_USRE_CDM_CASHUNITTHRESHOLD(HSERVICE hService, int cu);
	void sendWFS_SRVE_CDM_CASHUNITINFOCHANGED(HSERVICE hService, int cu);
	void sendWFS_SRVE_CDM_TELLERINFOCHANGED(HSERVICE hService);
	void sendWFS_EXEE_CDM_DELAYEDDISPENSE(HSERVICE hService);
	void sendWFS_EXEE_CDM_STARTDISPENSE(HSERVICE hService);
	void sendWFS_EXEE_CDM_CASHUNITERROR(HSERVICE hService, int id, WORD wFailure);
	void sendWFS_SRVE_CDM_ITEMSTAKEN(HSERVICE hService);
	void sendWFS_SRVE_CDM_COUNTS_CHANGED(HSERVICE hService);
	void sendWFS_EXEE_CDM_PARTIALDISPENSE(HSERVICE hService);
	void sendWFS_EXEE_CDM_SUBDISPENSEOK(HSERVICE hService);
	void sendWFS_EXEE_CDM_INCOMPLETEDISPENSE(HSERVICE hService, int amount, int cashBox, std::vector<long> values);
	void sendWFS_EXEE_CDM_NOTEERROR(HSERVICE hService, short r);
	void sendWFS_SRVE_CDM_ITEMSPRESENTED(HSERVICE hService);
	void sendWFS_SRVE_CDM_MEDIADETECTED(HSERVICE hService);
	void sendWFS_EXEE_CDM_INPUT_P6(HSERVICE hService);
	void sendWFS_SRVE_CDM_DEVICEPOSITION(HSERVICE hService);
	void sendWFS_SRVE_CDM_POWER_SAVE_CHANGE(HSERVICE hService);
	void sendWFS_EXEE_CDM_INFO_AVAILABLE(HSERVICE hService);
	void sendWFS_EXEE_CDM_INCOMPLETERETRACT(HSERVICE hService);
	void sendWFS_SRVE_CDM_SHUTTERSTATUSCHANGED(HSERVICE hService);

protected:
	void getInfoProc(REQUESTID reqId) override;
	void executeProc(REQUESTID reqId) override;
	void copyLpInput(LPVOID lpData, REQUESTID reqId)override;



public:
	CDM4000(volatile SharedData* s, unsigned short version, std::string port, int baudRate, std::string lname);
	~CDM4000();
};

