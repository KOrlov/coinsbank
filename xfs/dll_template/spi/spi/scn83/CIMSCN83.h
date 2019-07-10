#pragma once
#include "GenericDevice.h"
#include "XFSCIM.H"
#include "mpostLite.h"
using namespace Mpost;

enum AcceptorState
{
	stateOk = WFS_CIM_ACCOK,
	statePartialyOk = WFS_CIM_ACCCUSTATE,
	stateStop = WFS_CIM_ACCCUSTOP,
	stateUnknown = WFS_CIM_ACCCUUNKNOWN
};


class CIMSCN83 :
	public GenericDevice
{

protected:
	void getInfoProc(REQUESTID reqId) override;
	void executeProc(REQUESTID reqId) override;
	void copyLpInput(LPVOID lpData, REQUESTID reqId)override;

private:
	std::string port;
	std::string configName;
	int baudRate;
	MpostLite *mpost;

	//Structures & related XFS data

	WFSCIMSTATUS status;
	WFSCIMCAPS caps;
	WFSCIMINPOS position;
	WFSCIMCASHINSTATUS cashInStatus;
	std::vector<WFSCIMNOTETYPE> noteTypes;
	std::vector<WFSCIMCURRENCYEXP> currencyExp;
	
	//logical cashin unit
	std::vector<WFSCIMCASHIN> cashin;
	//physical cashin unit
	std::vector<WFSCIMPHCU> phcu;

	unsigned short notein;
	std::map<USHORT, ULONG> notesin;
	std::map<USHORT, ULONG> notesrej;
	std::vector<int> notesConfigured;
	bool exchangeActive = false;
	HRESULT hResult;

	//Supplementary
	
	void initCashInInfo();
	void changed();
	void noteInEscrow();
	void getStoredData();	
	void setStoredData();
	int totalCount();
	void renewStatus();
	void initCapabilities();
	void initNoteTypes();
	void initExponent();
	void initCashInStatus();
	bool canAccept();
	AcceptorState getAcceptorState();


	LPWFSCIMCASHIN constructLPWFSCIMCASHIN(LPVOID pResult);
	


	//EVENTS
	void sendWFS_SRVE_CIM_SAFEDOOROPEN(HSERVICE hService);
	void sendWFS_SRVE_CIM_SAFEDOORCLOSED(HSERVICE hService) ;
	void sendWFS_USRE_CIM_CASHUNITTHRESHOLD(HSERVICE hService) ;
	void sendWFS_SRVE_CIM_CASHUNITINFOCHANGED(HSERVICE hService);
	void sendWFS_SRVE_CIM_TELLERINFOCHANGED(HSERVICE hService) ;
	void sendWFS_EXEE_CIM_CASHUNITERROR(HSERVICE hService, WORD wFailure);
	void sendWFS_SRVE_CIM_ITEMSTAKEN(HSERVICE hService, WORD wPosition, WORD wAdditionalBunches, USHORT usBunchesRemaining);
	void sendWFS_SRVE_CIM_COUNTS_CHANGED(HSERVICE hService, USHORT usCount, LPUSHORT lpusCUNumList);
	void sendWFS_EXEE_CIM_INPUTREFUSE(HSERVICE hService,  USHORT lpusReason) ;
	void sendWFS_SRVE_CIM_ITEMSPRESENTED(HSERVICE hService, WORD wPosition, WORD wAdditionalBunches, USHORT usBunchesRemaining);
	void sendWFS_SRVE_CIM_ITEMSINSERTED(HSERVICE hService, WORD wPosition, WORD wAdditionalBunches, USHORT usBunchesRemaining);
	void sendWFS_EXEE_CIM_NOTEERROR(HSERVICE hService, USHORT usReason);
	void sendWFS_EXEE_CIM_SUBCASHIN(HSERVICE hService, LPWFSCIMNOTENUMBERLIST lpNoteNumberLists);
	void sendWFS_SRVE_CIM_MEDIADETECTED(HSERVICE hService, LPWFSCIMITEMPOSITION lpResetIns);
	void sendWFS_EXEE_CIM_INPUT_P6(HSERVICE hService) ;
	void sendWFS_EXEE_CIM_INFO_AVAILABLE(HSERVICE hService, LPWFSCIMITEMINFOSUMMARY *lppItemInfoSummary, int c);
	void sendWFS_EXEE_CIM_INSERTITEMS(HSERVICE hService) ;
	void sendWFS_SRVE_CIM_DEVICEPOSITION(HSERVICE hService, WORD wPosition);
	void sendWFS_SRVE_CIM_POWER_SAVE_CHANGE(HSERVICE hService, USHORT usPowerSaveRecoveryTime);
	void sendWFS_EXEE_CIM_INCOMPLETEREPLENISH(HSERVICE hService, LPWFSCIMINCOMPLETEREPLENISH lpIncompleteRepleniss);
	void sendWFS_EXEE_CIM_INCOMPLETEDEPLETE(HSERVICE hService, LPWFSCIMINCOMPLETEDEPLETE lpIncompleteDepletes) ;
	void sendWFS_SRVE_CIM_SHUTTERSTATUSCHANGED(HSERVICE hService, WORD fwPosition, WORD fwShutter) ;
	
	//GetInfo
	void getWFS_INF_CIM_STATUS(REQUESTID reqId);
	void getWFS_INF_CIM_CAPABILITIES(REQUESTID reqId);
	void getWFS_INF_CIM_CASH_UNIT_INFO(REQUESTID reqId);
	void getWFS_INF_CIM_TELLER_INFO(REQUESTID reqId);
	void getWFS_INF_CIM_CURRENCY_EXP(REQUESTID reqId);
	void getWFS_INF_CIM_BANKNOTE_TYPES(REQUESTID reqId);
	void getWFS_INF_CIM_CASH_IN_STATUS(REQUESTID reqId);
	void getWFS_INF_CIM_GET_P6_INFO(REQUESTID reqId);
	void getWFS_INF_CIM_GET_P6_SIGNATURE(REQUESTID reqId);
	void getWFS_INF_CIM_GET_ITEM_INFO(REQUESTID reqId);
	void getWFS_INF_CIM_POSITION_CAPABILITIES(REQUESTID reqId);
	void getWFS_INF_CIM_REPLENISH_TARGET(REQUESTID reqId);
	void getWFS_INF_CIM_DEVICELOCK_STATUS(REQUESTID reqId);
	void getWFS_INF_CIM_CASH_UNIT_CAPABILITIES(REQUESTID reqId);
	void getWFS_INF_CIM_DEPLETE_SOURCE(REQUESTID reqId);
	void getWFS_INF_CIM_GET_ALL_ITEMS_INFO(REQUESTID reqId);
	void getWFS_INF_CIM_GET_BLACKLIST(REQUESTID reqId);

	//Execute
	void execWFS_CMD_CIM_CASH_IN_START(REQUESTID reqId);
	void execWFS_CMD_CIM_CASH_IN(REQUESTID reqId);
	void execWFS_CMD_CIM_CASH_IN_END(REQUESTID reqId);
	void execWFS_CMD_CIM_CASH_IN_ROLLBACK(REQUESTID reqId);
	void execWFS_CMD_CIM_RETRACT(REQUESTID reqId);
	void execWFS_CMD_CIM_OPEN_SHUTTER(REQUESTID reqId);
	void execWFS_CMD_CIM_CLOSE_SHUTTER(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_TELLER_INFO(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_CASH_UNIT_INFO(REQUESTID reqId);
	void execWFS_CMD_CIM_START_EXCHANGE(REQUESTID reqId);
	void execWFS_CMD_CIM_END_EXCHANGE(REQUESTID reqId);
	void execWFS_CMD_CIM_OPEN_SAFE_DOOR(REQUESTID reqId);
	void execWFS_CMD_CIM_RESET(REQUESTID reqId);
	void execWFS_CMD_CIM_CONFIGURE_CASH_IN_UNITS(REQUESTID reqId);
	void execWFS_CMD_CIM_CONFIGURE_NOTETYPES(REQUESTID reqId);
	void execWFS_CMD_CIM_CREATE_P6_SIGNATURE(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_GUIDANCE_LIGHT(REQUESTID reqId);
	void execWFS_CMD_CIM_CONFIGURE_NOTE_READER(REQUESTID reqId);
	void execWFS_CMD_CIM_COMPARE_P6_SIGNATURE(REQUESTID reqId);
	void execWFS_CMD_CIM_POWER_SAVE_CONTROL(REQUESTID reqId);
	void execWFS_CMD_CIM_REPLENISH(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_CASH_IN_LIMIT(REQUESTID reqId);
	void execWFS_CMD_CIM_CASH_UNIT_COUNT(REQUESTID reqId);
	void execWFS_CMD_CIM_DEVICE_LOCK_CONTROL(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_MODE(REQUESTID reqId);
	void execWFS_CMD_CIM_PRESENT_MEDIA(REQUESTID reqId);
	void execWFS_CMD_CIM_DEPLETE(REQUESTID reqId);
	void execWFS_CMD_CIM_SET_BLACKLIST(REQUESTID reqId);
	void execWFS_CMD_CIM_SYNCHRONIZE_COMMAND(REQUESTID reqId);

public:
	CIMSCN83(volatile SharedData* s, unsigned short version, std::string, int, std::string);
	~CIMSCN83();
};

