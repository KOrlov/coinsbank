#include "stdafx.h"
#include "CIMSCN83.h"
#include "mpostLite.h"
using  namespace Mpost;
using namespace boost::filesystem;
using namespace boost::property_tree;


bool CIMSCN83::canAccept()
{	
	if (this->exchangeActive)return false;

	Xfs::getInstance()->l.debug("cashInStatus");
	if (this->cashInStatus.wStatus != WFS_CIM_CIOK)return false;
	Xfs::getInstance()->l.debug("fwDevice");
	if (this->status.fwDevice != WFS_CIM_DEVONLINE)return false;
	Xfs::getInstance()->l.debug("acceptorState");
	if (this->status.fwAcceptor != AcceptorState::stateOk)return false;
	Xfs::getInstance()->l.debug("intermediate stacker");
	if (this->status.fwIntermediateStacker != WFS_CIM_ISEMPTY)return false;
	Xfs::getInstance()->l.debug("stacker items");
	if (this->status.fwStackerItems != WFS_CIM_NOITEMS)return false;
	if (this->status.fwBanknoteReader != WFS_CIM_BNROK)return false;

	return true;
}

void CIMSCN83::initCashInStatus()
{
	this->cashInStatus.wStatus = WFS_CIM_CIOK;
	this->cashInStatus.usNumOfRefused = 0;
	this->notein = 0;
}


AcceptorState CIMSCN83::getAcceptorState()
{
	Xfs::getInstance()->l.debug("getAcceptorState");

	if (this->status.fwDevice != WFS_CIM_DEVONLINE)return AcceptorState::stateUnknown;

	if (this->totalCount() >= this->cashin[0].ulMaximum)
	{		
		Xfs::getInstance()->l.debug("totalCount()");

		return AcceptorState::stateStop;
	}
	if (this->mpost->cheated)
	{		
		Xfs::getInstance()->l.debug("cheated");
		return AcceptorState::stateStop;
	}
	if (!this->mpost->cassetteAttached)
	{		
		Xfs::getInstance()->l.debug("cassete not attached");
		return AcceptorState::stateStop;
	}
	Xfs::getInstance()->l.debug("getAcceptorState ok");

	return AcceptorState::stateOk;
}

void CIMSCN83::renewStatus()
{

	if (this->mpost->connected())
	{
		this->status.fwDevice = WFS_CIM_DEVONLINE;
	}
	else
		this->status.fwDevice = WFS_CIM_DEVOFFLINE;

	//we don't support safedoor
	this->status.fwSafeDoor = WFS_CIM_DOORNOTSUPPORTED;

	this->status.fwAcceptor = getAcceptorState();
	
	this->status.fwIntermediateStacker = notein ? WFS_CIM_ISNOTEMPTY : WFS_CIM_ISEMPTY;
	
	this->status.fwStackerItems = notein ? WFS_CIM_CUSTOMERACCESS : WFS_CIM_NOITEMS;

	this->status.fwBanknoteReader = WFS_CIM_BNROK;

	this->status.bDropBox = false;
	this->status.lppPositions = 0;
	this->status.lpszExtra = 0;
	for (int i = 0; i < WFS_CIM_GUIDLIGHTS_SIZE; i++)
		this->status.dwGuidLights[i] = WFS_CIM_GUIDANCE_NOT_AVAILABLE;
	this->status.wDevicePosition = WFS_CIM_DEVICEPOSNOTSUPP;
	this->status.usPowerSaveRecoveryTime = 0;
	this->status.wMixedMode = WFS_CIM_MIXEDMEDIANOTACTIVE;
	this->status.wAntiFraudModule = WFS_CIM_AFMNOTSUPP;

}
void CIMSCN83::initCapabilities()
{
	this->caps.wClass = WFS_SERVICE_CLASS_CIM;
	this->caps.fwType = WFS_CIM_SELFSERVICEBILL;
	this->caps.wMaxCashInItems = 1;
	this->caps.bCompound = 0;
	this->caps.bShutter = 1;
	this->caps.bShutterControl = 0;
	this->caps.bSafeDoor = 0;
	this->caps.bCashBox = 0;
	this->caps.bRefill = 0;
	this->caps.fwIntermediateStacker = 0;
	this->caps.bItemsTakenSensor = 0;
	this->caps.bItemsInsertedSensor = 1;
	this->caps.fwPositions = WFS_CIM_POSINFRONT | WFS_CIM_POSOUTFRONT;
	this->caps.fwExchangeType = WFS_CIM_EXBYHAND;
	this->caps.fwRetractAreas = WFS_CIM_RA_RETRACT;
	this->caps.fwRetractTransportActions = WFS_CIM_REJECT;
	this->caps.fwRetractStackerActions = WFS_CIM_REJECT;
	this->caps.lpszExtra = 0;
	for (int i = 0; i < WFS_CIM_GUIDLIGHTS_MAX; i++)
		this->caps.dwGuidLights[i] = WFS_CIM_GUIDANCE_NOT_AVAILABLE;
	this->caps.dwItemInfoTypes = WFS_CIM_ITEM_SERIALNUMBER;
	this->caps.bCompareSignatures = 0;
	this->caps.bPowerSaveControl = 0;
	this->caps.bReplenish = 0;
	this->caps.fwCashInLimit = WFS_CIM_LIMITNOTSUPP;
	this->caps.fwCountActions = WFS_CIM_COUNTNOTSUPP;
	this->caps.bDeviceLockControl = 0;
	this->caps.wMixedMode = WFS_CIM_MIXEDMEDIANOTSUPP;
	this->caps.bMixedDepositAndRollback = 0;
	this->caps.bAntiFraudModule = 0;
	this->caps.bDeplete = 0;
	this->caps.bBlacklist = 0;
	this->caps.lpdwSynchronizableCommands = 0;
}
void CIMSCN83::initNoteTypes()
{
	this->mpost->queryValueTable();
	for (auto n : mpost->noteTypes)
	{
		WFSCIMNOTETYPE nt;
		nt.bConfigured = true;
		nt.usRelease = 0;
		nt.ulValues = n.base * pow(10, n.exp);
		nt.usNoteID = n.index;
		nt.cCurrencyID[0] = n.iso[0];
		nt.cCurrencyID[1] = n.iso[1];
		nt.cCurrencyID[2] = n.iso[2];
		this->noteTypes.push_back(nt);
	}
}
void CIMSCN83::initExponent()
{
	//Setting currency exponent for euro
	WFSCIMCURRENCYEXP ce;
	ce.cCurrencyID[0] = 'E';
	ce.cCurrencyID[1] = 'U';
	ce.cCurrencyID[2] = 'R';
	ce.sExponent = 0;
	currencyExp.push_back(ce);
}
int CIMSCN83::totalCount()
{
	int total = 0;
	for (auto note : this->notesin)
		total += note.second;
	return total;
}


LPWFSCIMCASHIN  CIMSCN83::constructLPWFSCIMCASHIN(LPVOID pResult)
{
	LPWFSCIMCASHIN cin;
	memAlloc->AllocateMore(sizeof(WFSCIMCASHIN), pResult, (void **)&cin);

	auto ciCassete = this->cashin[0];

	cin->usNumber= ciCassete.usNumber;
	cin->fwType = ciCassete.fwType;
	cin->fwItemType = ciCassete.fwItemType;

	cin->cUnitID[0] = ciCassete.cUnitID[0];
	cin->cUnitID[1] = ciCassete.cUnitID[1];
	cin->cUnitID[2] = ciCassete.cUnitID[2];
	cin->cUnitID[3] = ciCassete.cUnitID[3];
	cin->cUnitID[4] = ciCassete.cUnitID[4];
	cin->cCurrencyID[0] = ciCassete.cCurrencyID[0];
	cin->cCurrencyID[1] = ciCassete.cCurrencyID[1];
	cin->cCurrencyID[2] = ciCassete.cCurrencyID[2];
	cin->ulValues = ciCassete.ulValues;//we accept more than 1 denom.
	cin->ulCashInCount = this->totalCount();
	cin->ulCount =  this->totalCount();
	cin->ulMaximum = ciCassete.ulMaximum;
	cin->usStatus = ciCassete.usStatus;
	cin->bAppLock = ciCassete.bAppLock;
	//cin->lpNoteNumberList ciCassete.lpNoteNumberList;
	cin->usNumPhysicalCUs=ciCassete.usNumPhysicalCUs;
	///cin->lppPhysical= ciCassete.lppPhysical;
	cin->lpszExtra = 0;
	cin->lpusNoteIDs = 0;//WFS_CIM_CITYPALL type doesnt required note id's
	cin->usCDMType = ciCassete.usCDMType; // we dont have CDM attached
	cin->lpszCashUnitName = 0;
	cin->ulInitialCount= ciCassete.ulInitialCount;
	cin->ulDispensedCount = ciCassete.ulDispensedCount;
	cin->ulPresentedCount= ciCassete.ulPresentedCount;
	cin->ulRetractedCount= ciCassete.ulRetractedCount;
	cin->ulRejectCount= ciCassete.ulRejectCount;
	cin->ulMinimum= ciCassete.ulMinimum;

	//sub-properties
	//note number list
	memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBERLIST), pResult, (void **)&cin->lpNoteNumberList);
	cin->lpNoteNumberList->usNumOfNoteNumbers = notesin.size();
	memAlloc->AllocateMore(sizeof(LPWFSCIMNOTENUMBER)*cin->lpNoteNumberList->usNumOfNoteNumbers, pResult, (void **)&cin->lpNoteNumberList->lppNoteNumber);
	

	int i = 0;
	for (auto n : this->notesin)
	{
		memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBER)*cin->lpNoteNumberList->usNumOfNoteNumbers, pResult, (void **)&cin->lpNoteNumberList->lppNoteNumber[i]);
		cin->lpNoteNumberList->lppNoteNumber[i]->usNoteID = n.first;
		cin->lpNoteNumberList->lppNoteNumber[i]->ulCount = n.second;
		i++;
	}

	memAlloc->AllocateMore(sizeof(LPWFSCIMPHCU), pResult, (void **)&cin->lppPhysical);
	memAlloc->AllocateMore(sizeof(WFSCIMPHCU), pResult, (void **)&cin->lppPhysical[0]);
	
	WFSCIMPHCU pcu = this->phcu[0];
	cin->lppPhysical[0]->lpPhysicalPositionName = 0;
	cin->lppPhysical[0]->cUnitID[0] = pcu.cUnitID[0];
	cin->lppPhysical[0]->cUnitID[1] = pcu.cUnitID[1];
	cin->lppPhysical[0]->cUnitID[2] = pcu.cUnitID[2];
	cin->lppPhysical[0]->cUnitID[3] = pcu.cUnitID[3];
	cin->lppPhysical[0]->cUnitID[4] = pcu.cUnitID[4];
	cin->lppPhysical[0]->ulCashInCount = cin->ulCashInCount;
	cin->lppPhysical[0]->ulCount = cin->ulCount;
	cin->lppPhysical[0]->ulMaximum = cin->ulMaximum;
	cin->lppPhysical[0]->usPStatus = pcu.usPStatus;
	cin->lppPhysical[0]->bHardwareSensors = pcu.bHardwareSensors;
	cin->lppPhysical[0]->lpszExtra = 0;
	cin->lppPhysical[0]->ulInitialCount = pcu.ulInitialCount;
	cin->lppPhysical[0]->ulDispensedCount = pcu.ulDispensedCount;
	cin->lppPhysical[0]->ulPresentedCount = pcu.ulPresentedCount;
	cin->lppPhysical[0]->ulRetractedCount = pcu.ulRetractedCount;
	cin->lppPhysical[0]->ulRejectCount = pcu.ulRejectCount;
	return cin;
}



void CIMSCN83::initCashInInfo()
{	
	WFSCIMCASHIN ciCassete;
	//Cash cassete

	ciCassete.usNumber = 1;
	ciCassete.fwType = WFS_CIM_TYPECASHIN;
	ciCassete.fwItemType = WFS_CIM_CITYPALL;

	ciCassete.cUnitID[0] = '0';
	ciCassete.cUnitID[1] = '0';
	ciCassete.cUnitID[2] = '0';
	ciCassete.cUnitID[3] = '0';
	ciCassete.cUnitID[4] = '1';
	ciCassete.cCurrencyID[0] = 'E';
	ciCassete.cCurrencyID[1] = 'U';
	ciCassete.cCurrencyID[2] = 'R';
	ciCassete.ulValues = 0;//we accept more than 1 denom.
	ciCassete.ulCashInCount = 0;
	ciCassete.ulCount = 0;
	ciCassete.ulMaximum = 0;
	ciCassete.usStatus = WFS_CIM_STATCUOK;
	ciCassete.bAppLock = false;
	ciCassete.lpNoteNumberList;
	ciCassete.usNumPhysicalCUs = 1;
	ciCassete.lppPhysical = 0;
	ciCassete.lpszExtra = 0;
	ciCassete.lpusNoteIDs;//WFS_CIM_CITYPALL type doesnt required note id's
	ciCassete.usCDMType = 0; // we dont have CDM attached
	ciCassete.lpszCashUnitName = 0;
	ciCassete.ulInitialCount = 0;
	ciCassete.ulDispensedCount = 0;
	ciCassete.ulPresentedCount = 0;
	ciCassete.ulRetractedCount = 0;
	ciCassete.ulRejectCount = 0;
	ciCassete.ulMinimum = 0;	
	this->cashin.push_back(ciCassete);

	WFSCIMPHCU ph;
	ph.lpPhysicalPositionName = 0;
	ph.cUnitID[0] = '0';
	ph.cUnitID[1] = '0';
	ph.cUnitID[2] = '0';
	ph.cUnitID[3] = '0';
	ph.cUnitID[4] = '1';
	ph.ulCashInCount = 0;
	ph.ulCount = 0;
	ph.ulMaximum = 0;
	ph.usPStatus = WFS_CIM_STATCUOK;
	ph.bHardwareSensors = false;
	ph.lpszExtra = 0;
	ph.ulInitialCount = 0;
	ph.ulDispensedCount = 0;
	ph.ulPresentedCount = 0;
	ph.ulRetractedCount = 0;
	ph.ulRejectCount = 0;

	this->phcu.push_back(ph);
}



void CIMSCN83::sendWFS_SRVE_CIM_SAFEDOOROPEN(HSERVICE hService) 
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_SAFEDOOROPEN;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_SAFEDOORCLOSED(HSERVICE hService) 
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_SAFEDOORCLOSED;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_USRE_CIM_CASHUNITTHRESHOLD(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_USER_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_USRE_CIM_CASHUNITTHRESHOLD;

		
		
		
		pResult->lpBuffer = constructLPWFSCIMCASHIN(pResult);

		sendMessage(i.first, WFS_USER_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_CASHUNITINFOCHANGED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_CASHUNITINFOCHANGED;

		
		pResult->lpBuffer = constructLPWFSCIMCASHIN(pResult);

		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}


void CIMSCN83::sendWFS_SRVE_CIM_TELLERINFOCHANGED(HSERVICE hService) 
{
	throw "Device is not teller-type";
}


void CIMSCN83::sendWFS_EXEE_CIM_CASHUNITERROR(HSERVICE hService, WORD wFailure)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_CASHUNITERROR;

		LPWFSCIMCUERROR lpCashUnitError;
		memAlloc->AllocateMore(sizeof(WFSCIMCUERROR), pResult,(void **)&lpCashUnitError);
		lpCashUnitError->wFailure = wFailure;
		
		lpCashUnitError->lpCashUnit = constructLPWFSCIMCASHIN(pResult);
		

		pResult->lpBuffer = lpCashUnitError;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_ITEMSTAKEN(HSERVICE hService, WORD wPosition,WORD wAdditionalBunches,USHORT usBunchesRemaining)
{
	map<HWND, EventReg> events = getEventMap(0,WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_ITEMSTAKEN;

		LPWFSCIMPOSITIONINFO lpPositionInf;

		memAlloc->AllocateMore(sizeof(WFSCIMPOSITIONINFO), pResult, (void **)&lpPositionInf);
		
		lpPositionInf->usBunchesRemaining = usBunchesRemaining;
		lpPositionInf->wAdditionalBunches = wAdditionalBunches;
		lpPositionInf->wPosition = wPosition;

		pResult->lpBuffer = lpPositionInf;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}

}
void CIMSCN83::sendWFS_SRVE_CIM_COUNTS_CHANGED(HSERVICE hService, USHORT usCount,LPUSHORT lpusCUNumList)
{
	map<HWND, EventReg> events = getEventMap(0,WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_COUNTS_CHANGED;

		LPWFSCIMCOUNTSCHANGED lpCountsChanged;
		memAlloc->AllocateMore(sizeof(WFSCIMCOUNTSCHANGED), pResult, (void **)&lpCountsChanged);
		lpCountsChanged->usCount = usCount;
		memAlloc->AllocateMore(lpCountsChanged->usCount*2, pResult, (void **)&lpCountsChanged->lpusCUNumList);
		for (int c = 0; c < usCount; c++)
		{
			lpCountsChanged->lpusCUNumList[c] = lpusCUNumList[c];
		}		
		pResult->lpBuffer = lpCountsChanged;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_EXEE_CIM_INPUTREFUSE(HSERVICE hService, USHORT usReason)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_INPUTREFUSE;

		LPUSHORT lpusReason;
		memAlloc->AllocateMore(sizeof(USHORT), pResult, (void **)&lpusReason);
		*lpusReason = usReason;
		
		pResult->lpBuffer = lpusReason;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_ITEMSPRESENTED(HSERVICE hService, WORD wPosition, WORD wAdditionalBunches, USHORT usBunchesRemaining)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_ITEMSPRESENTED;

		LPWFSCIMPOSITIONINFO lpPositionInf;

		memAlloc->AllocateMore(sizeof(WFSCIMPOSITIONINFO), pResult, (void **)&lpPositionInf);

		lpPositionInf->usBunchesRemaining = usBunchesRemaining;
		lpPositionInf->wAdditionalBunches = wAdditionalBunches;
		lpPositionInf->wPosition = wPosition;

		pResult->lpBuffer = lpPositionInf;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}

}
void CIMSCN83::sendWFS_SRVE_CIM_ITEMSINSERTED(HSERVICE hService, WORD wPosition, WORD wAdditionalBunches, USHORT usBunchesRemaining)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_ITEMSINSERTED;

		LPWFSCIMPOSITIONINFO lpPositionInf;

		memAlloc->AllocateMore(sizeof(WFSCIMPOSITIONINFO), pResult, (void **)&lpPositionInf);

		lpPositionInf->usBunchesRemaining = usBunchesRemaining;
		lpPositionInf->wAdditionalBunches = wAdditionalBunches;
		lpPositionInf->wPosition = wPosition;

		pResult->lpBuffer = lpPositionInf;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}

}
void CIMSCN83::sendWFS_EXEE_CIM_NOTEERROR(HSERVICE hService, USHORT usReason)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_NOTEERROR;

		LPUSHORT lpusReason;
		memAlloc->AllocateMore(sizeof(USHORT), pResult, (void **)&lpusReason);
		*lpusReason = usReason;

		pResult->lpBuffer = lpusReason;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_EXEE_CIM_SUBCASHIN(HSERVICE hService, LPWFSCIMNOTENUMBERLIST lpNoteNumberLists)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_SUBCASHIN;

		LPWFSCIMNOTENUMBERLIST lpNoteNumberListd;
		memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBERLIST), pResult, (void **)&lpNoteNumberListd);
		lpNoteNumberListd->usNumOfNoteNumbers = lpNoteNumberLists->usNumOfNoteNumbers;
		memAlloc->AllocateMore(lpNoteNumberListd->usNumOfNoteNumbers*2, pResult, (void **)&lpNoteNumberListd->lppNoteNumber);

		for (int c = 0; c < lpNoteNumberListd->usNumOfNoteNumbers; c++)
		{
			lpNoteNumberListd->lppNoteNumber[c] = lpNoteNumberLists->lppNoteNumber[c];
		}

		pResult->lpBuffer = lpNoteNumberListd;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_MEDIADETECTED(HSERVICE hService, LPWFSCIMITEMPOSITION lpResetIns)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_MEDIADETECTED;

		LPWFSCIMITEMPOSITION lpResetInd;

		memAlloc->AllocateBuffer(sizeof(WFSCIMITEMPOSITION), (void **)&lpResetInd);
		
		lpResetInd->fwOutputPosition = lpResetIns->fwOutputPosition;
		lpResetInd->usNumber = lpResetIns->usNumber;
		if (lpResetIns->lpRetractArea)
		{
			memAlloc->AllocateMore(sizeof(WFSCIMITEMPOSITION), pResult, (void **)&lpResetInd->lpRetractArea);
			lpResetInd->lpRetractArea->fwOutputPosition = lpResetIns->lpRetractArea->fwOutputPosition;
			lpResetInd->lpRetractArea->usIndex = lpResetIns->lpRetractArea->usIndex;
			lpResetInd->lpRetractArea->usRetractArea = lpResetIns->lpRetractArea->usRetractArea;
		}
		pResult->lpBuffer = lpResetInd;

		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);

	}

	
}
void CIMSCN83::sendWFS_EXEE_CIM_INPUT_P6(HSERVICE hService)
{
	throw "P6 not implemented";
}

void CIMSCN83::sendWFS_EXEE_CIM_INFO_AVAILABLE(HSERVICE hService, LPWFSCIMITEMINFOSUMMARY *lppItemInfoSummarys, int c)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_INFO_AVAILABLE;

		LPWFSCIMITEMINFOSUMMARY *lppItemInfoSummaryd;
		memAlloc->AllocateMore(sizeof(LPWFSCIMITEMINFOSUMMARY)*(c+1), pResult, (void **)&lppItemInfoSummaryd);
		lppItemInfoSummaryd[c] = 0;
		for (int k = 0; k < c; k++)
		{
			memAlloc->AllocateMore(sizeof(WFSCIMITEMINFOSUMMARY), pResult, (void **)&lppItemInfoSummaryd[k]);
			lppItemInfoSummaryd[k]->usLevel = lppItemInfoSummarys[k]->usLevel;
			lppItemInfoSummaryd[k]->usNumOfItems = lppItemInfoSummarys[k]->usNumOfItems;
		}	

		pResult->lpBuffer = lppItemInfoSummaryd;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_EXEE_CIM_INSERTITEMS(HSERVICE hService) 
{

	Xfs::getInstance()->l.debug("sendWFS_EXEE_CIM_INSERTITEMS  EXECUTING");
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_INSERTITEMS;

		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_DEVICEPOSITION(HSERVICE hService, WORD wPosition)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_DEVICEPOSITION;

		LPWFSCIMDEVICEPOSITION lpDevicePosition;
		memAlloc->AllocateMore(sizeof(WFSCIMDEVICEPOSITION), pResult, (void **)&lpDevicePosition);
		lpDevicePosition->wPosition = wPosition;

		pResult->lpBuffer = lpDevicePosition;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_POWER_SAVE_CHANGE(HSERVICE hService, USHORT usPowerSaveRecoveryTime)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_POWER_SAVE_CHANGE;

		LPWFSCIMPOWERSAVECHANGE lpPowerSaveChange;
		memAlloc->AllocateMore(sizeof(WFSCIMPOWERSAVECHANGE), pResult, (void **)&lpPowerSaveChange);
		lpPowerSaveChange->usPowerSaveRecoveryTime = usPowerSaveRecoveryTime;

		pResult->lpBuffer = lpPowerSaveChange;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_EXEE_CIM_INCOMPLETEREPLENISH(HSERVICE hService, LPWFSCIMINCOMPLETEREPLENISH lpIncompleteReplenishs)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_INCOMPLETEREPLENISH;

		LPWFSCIMINCOMPLETEREPLENISH lpIncompleteReplenishd;
		
		memAlloc->AllocateMore(sizeof(WFSCIMINCOMPLETEREPLENISH), pResult, (void **)&lpIncompleteReplenishd);
		memAlloc->AllocateMore(sizeof(WFSCIMREPRES), pResult, (void **)&lpIncompleteReplenishd->lpReplenish);
		lpIncompleteReplenishd->lpReplenish->ulNumberOfItemsRemoved = lpIncompleteReplenishs->lpReplenish->ulNumberOfItemsRemoved;
		lpIncompleteReplenishd->lpReplenish->ulNumberOfItemsRejected = lpIncompleteReplenishs->lpReplenish->ulNumberOfItemsRejected;


		int c = 0;
		while (lpIncompleteReplenishs->lpReplenish->lppReplenishTargetResults[c])c++;
		memAlloc->AllocateMore(4*(c+1), pResult, (void **)&lpIncompleteReplenishd->lpReplenish->lppReplenishTargetResults);
		lpIncompleteReplenishd->lpReplenish->lppReplenishTargetResults[c] = 0;
		for (int k = 0; k < c; k++)
		{
			lpIncompleteReplenishd->lpReplenish->lppReplenishTargetResults[k]->ulNumberOfItemsReceived = lpIncompleteReplenishs->lpReplenish->lppReplenishTargetResults[k]->ulNumberOfItemsReceived;
			lpIncompleteReplenishd->lpReplenish->lppReplenishTargetResults[k]->usNoteID = lpIncompleteReplenishs->lpReplenish->lppReplenishTargetResults[k]->usNoteID;
			lpIncompleteReplenishd->lpReplenish->lppReplenishTargetResults[k]->usNumberTarget = lpIncompleteReplenishs->lpReplenish->lppReplenishTargetResults[k]->usNumberTarget;
		}
			   		 
		pResult->lpBuffer = lpIncompleteReplenishd;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_EXEE_CIM_INCOMPLETEDEPLETE(HSERVICE hService, LPWFSCIMINCOMPLETEDEPLETE lpIncompleteDepletes)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_CIM_INCOMPLETEDEPLETE;

		LPWFSCIMINCOMPLETEDEPLETE lpIncompleteDepleted;

		memAlloc->AllocateMore(sizeof(WFSCIMINCOMPLETEDEPLETE), pResult, (void **)&lpIncompleteDepleted);
		
		memAlloc->AllocateMore(sizeof(WFSCIMDEPRES), pResult, (void **)&lpIncompleteDepleted->lpDeplete);
		lpIncompleteDepleted->lpDeplete->ulNumberOfItemsReceived = lpIncompleteDepletes->lpDeplete->ulNumberOfItemsReceived;
		lpIncompleteDepleted->lpDeplete->ulNumberOfItemsRejected = lpIncompleteDepletes->lpDeplete->ulNumberOfItemsRejected;
		
		int c = 0;
		while (lpIncompleteDepletes->lpDeplete->lppDepleteSourceResults[c])c++;
		memAlloc->AllocateMore(4 * (c + 1), pResult, (void **)&lpIncompleteDepleted->lpDeplete->lppDepleteSourceResults);
		lpIncompleteDepleted->lpDeplete->lppDepleteSourceResults[c] = 0;
		for (int k = 0; k < c; k++)
		{
			lpIncompleteDepleted->lpDeplete->lppDepleteSourceResults[k]->ulNumberOfItemsRemoved = lpIncompleteDepletes->lpDeplete->lppDepleteSourceResults[k]->ulNumberOfItemsRemoved;
			lpIncompleteDepleted->lpDeplete->lppDepleteSourceResults[k]->usNoteID = lpIncompleteDepletes->lpDeplete->lppDepleteSourceResults[k]->usNoteID;
			lpIncompleteDepleted->lpDeplete->lppDepleteSourceResults[k]->usNumberSource= lpIncompleteDepletes->lpDeplete->lppDepleteSourceResults[k]->usNumberSource;
		}
		pResult->lpBuffer = lpIncompleteDepleted;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CIMSCN83::sendWFS_SRVE_CIM_SHUTTERSTATUSCHANGED(HSERVICE hService, WORD fwPosition,WORD fwShutter)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CIM_SHUTTERSTATUSCHANGED;

		LPWFSCIMSHUTTERSTATUSCHANGED lpShutterStatusChanged;
		memAlloc->AllocateMore(sizeof(WFSCIMSHUTTERSTATUSCHANGED), pResult, (void **)&lpShutterStatusChanged);
		lpShutterStatusChanged->fwPosition = fwPosition;
		lpShutterStatusChanged->fwShutter = fwShutter;
		
		pResult->lpBuffer = lpShutterStatusChanged;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void CIMSCN83::getWFS_INF_CIM_STATUS(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMSTATUS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMSTATUS s = (LPWFSCIMSTATUS)requests[reqId]->pResult->lpBuffer;

	s->fwDevice = status.fwDevice;
	s->fwSafeDoor = status.fwSafeDoor;
	s->fwAcceptor = status.fwAcceptor;
	s->fwIntermediateStacker = status.fwIntermediateStacker;
	s->fwStackerItems = status.fwStackerItems;
	s->fwBanknoteReader = status.fwBanknoteReader;
	s->bDropBox = status.bDropBox;
	int c = 0;
	while(s->lppPositions)c++;
	a = this->memAlloc->AllocateMore(4*(c+1), requests[reqId]->pResult, (void **)&s->lppPositions);
	s->lppPositions[c] = 0;
	for (int i = 0; i < c; i++)
	{
		a = this->memAlloc->AllocateMore(sizeof(WFSCIMINPOS), requests[reqId]->pResult, (void **)&s->lppPositions[i]);
		s->lppPositions[i]->fwJammedShutterPosition = status.lppPositions[i]->fwJammedShutterPosition;
		s->lppPositions[i]->fwPosition = status.lppPositions[i]->fwPosition;
		s->lppPositions[i]->fwPositionStatus = status.lppPositions[i]->fwPositionStatus;
		s->lppPositions[i]->fwShutter = status.lppPositions[i]->fwShutter;
		s->lppPositions[i]->fwTransport = status.lppPositions[i]->fwTransport;
		s->lppPositions[i]->fwTransportStatus = status.lppPositions[i]->fwTransportStatus;
	}




	s->lpszExtra = 0;
	for (int i = 0; i < WFS_CIM_GUIDLIGHTS_SIZE; i++)
	{
		s->dwGuidLights[i] = status.dwGuidLights[i];
	}
	
	s->wDevicePosition = status.wDevicePosition;
	s->usPowerSaveRecoveryTime = status.usPowerSaveRecoveryTime;
	s->wMixedMode = status.wMixedMode;
	s->wAntiFraudModule = status.wAntiFraudModule;



}
void CIMSCN83::getWFS_INF_CIM_CAPABILITIES(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMCAPS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMCAPS s = (LPWFSCIMCAPS)requests[reqId]->pResult->lpBuffer;
	s->wClass = caps.wClass;
	s->fwType = caps.fwType;
	s->wMaxCashInItems = caps.wMaxCashInItems;
	s->bCompound = caps.bCompound;
	s->bShutter = caps.bShutter;
	s->bShutterControl = caps.bShutterControl;
	s->bSafeDoor = caps.bSafeDoor;
	s->bCashBox = caps.bCashBox;
	s->bRefill = caps.bRefill;
	s->fwIntermediateStacker = caps.fwIntermediateStacker;
	s->bItemsTakenSensor = caps.bItemsTakenSensor;
	s->bItemsInsertedSensor = caps.bItemsInsertedSensor;
	s->fwPositions = caps.fwPositions;
	s->fwExchangeType = caps.fwExchangeType;
	s->fwRetractAreas = caps.fwRetractAreas;
	s->fwRetractTransportActions = caps.fwRetractTransportActions;
	s->fwRetractStackerActions = caps.fwRetractStackerActions;
	s->lpszExtra = 0;
	for(int i = 0;i<WFS_CIM_GUIDLIGHTS_SIZE;i++)
		s->dwGuidLights[i] = caps.dwGuidLights[i];
	s->dwItemInfoTypes = caps.dwItemInfoTypes;
	s->bCompareSignatures = caps.bCompareSignatures;
	s->bPowerSaveControl = caps.bPowerSaveControl;
	s->bReplenish = caps.bReplenish;
	s->fwCashInLimit = caps.fwCashInLimit;
	s->fwCountActions = caps.fwCountActions;
	s->bDeviceLockControl = caps.bDeviceLockControl;
	s->wMixedMode = caps.wMixedMode;
	s->bMixedDepositAndRollback = caps.bMixedDepositAndRollback;
	s->bAntiFraudModule = caps.bAntiFraudModule;
	s->bDeplete = caps.bDeplete;
	s->bBlacklist = caps.bBlacklist;
	s->lpdwSynchronizableCommands = 0;



}
void CIMSCN83::getWFS_INF_CIM_CASH_UNIT_INFO(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMCASHINFO), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMCASHINFO lpcii = (LPWFSCIMCASHINFO)requests[reqId]->pResult->lpBuffer;
	lpcii->usCount = 1;

	a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMCASHIN), requests[reqId]->pResult, (void **)&lpcii->lppCashIn);
	lpcii->lppCashIn[0] = constructLPWFSCIMCASHIN(requests[reqId]->pResult);	
}
void CIMSCN83::getWFS_INF_CIM_TELLER_INFO(REQUESTID reqId){}

void CIMSCN83::getWFS_INF_CIM_CURRENCY_EXP(REQUESTID reqId)
{
	
	auto a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMCURRENCYEXP)*(this->currencyExp.size() + 1), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	
	LPWFSCIMCURRENCYEXP *s = (LPWFSCIMCURRENCYEXP *)requests[reqId]->pResult->lpBuffer;
	s[currencyExp.size()] = 0;
	for (int i = 0; i < this->currencyExp.size(); i++)
	{
		this->memAlloc->AllocateMore(sizeof(WFSCIMCURRENCYEXP)*(this->currencyExp.size() + 1), requests[reqId]->pResult, (void **)&s[i]);
		s[i]->sExponent = currencyExp[i].sExponent;
		for(int c = 0;c<3;c++)
			s[i]->cCurrencyID[c] = currencyExp[i].cCurrencyID[c];
	}	
	
}
void CIMSCN83::getWFS_INF_CIM_BANKNOTE_TYPES(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMNOTETYPELIST), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMNOTETYPELIST s = (LPWFSCIMNOTETYPELIST)requests[reqId]->pResult->lpBuffer;
	s->usNumOfNoteTypes = noteTypes.size();
	a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMNOTETYPE)*s->usNumOfNoteTypes, requests[reqId]->pResult, (void **)&s->lppNoteTypes);
	for (int i = 0; i < s->usNumOfNoteTypes; i++)
	{
		a = this->memAlloc->AllocateMore(sizeof(WFSCIMNOTETYPE)*s->usNumOfNoteTypes, requests[reqId]->pResult, (void **)&s->lppNoteTypes[i]);

		for (int c = 0; c < 3; c++)
			s->lppNoteTypes[i]->cCurrencyID[c] = noteTypes[i].cCurrencyID[c];
		s->lppNoteTypes[i]->bConfigured = noteTypes[i].bConfigured;
		s->lppNoteTypes[i]->ulValues = noteTypes[i].ulValues;
		s->lppNoteTypes[i]->usNoteID = noteTypes[i].usNoteID;
		s->lppNoteTypes[i]->usRelease = noteTypes[i].usRelease;
	}


}


void CIMSCN83::getWFS_INF_CIM_CASH_IN_STATUS(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMCASHINSTATUS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);

	LPWFSCIMCASHINSTATUS lpCashInStatusd = (LPWFSCIMCASHINSTATUS)requests[reqId]->pResult->lpBuffer;
	lpCashInStatusd->lpszExtra = 0;
	lpCashInStatusd->usNumOfRefused = this->cashInStatus.usNumOfRefused;
	lpCashInStatusd->wStatus = this->cashInStatus.wStatus;

	this->memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBERLIST), requests[reqId]->pResult, (void **)&lpCashInStatusd->lpNoteNumberList);
	LPWFSCIMNOTENUMBERLIST nnl = (LPWFSCIMNOTENUMBERLIST)lpCashInStatusd->lpNoteNumberList;
	
	if (notein > 0)
		nnl->usNumOfNoteNumbers = 1;
	else
		nnl->usNumOfNoteNumbers = 0;
	if (!nnl->usNumOfNoteNumbers)
		return;

	this->memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBER), requests[reqId]->pResult, (void **)&nnl->lppNoteNumber);
	nnl->lppNoteNumber[0]->ulCount = 1;
	nnl->lppNoteNumber[0]->usNoteID = notein;
	
}

void CIMSCN83::getWFS_INF_CIM_GET_P6_INFO(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_GET_P6_SIGNATURE(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_GET_ITEM_INFO(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_POSITION_CAPABILITIES(REQUESTID reqId)
{
	this->memAlloc->AllocateMore(sizeof(WFSCIMPOSCAPABILITIES), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMPOSCAPABILITIES pcaps = (LPWFSCIMPOSCAPABILITIES)requests[reqId]->pResult->lpBuffer;

	this->memAlloc->AllocateMore(sizeof(LPWFSCIMPOSCAPS)*3, requests[reqId]->pResult, (void **)&pcaps->lppPosCapabilities);
	pcaps->lppPosCapabilities[2] = 0;
	this->memAlloc->AllocateMore(sizeof(WFSCIMPOSCAPS), requests[reqId]->pResult, (void **)&pcaps->lppPosCapabilities[0]);
	this->memAlloc->AllocateMore(sizeof(WFSCIMPOSCAPS), requests[reqId]->pResult, (void **)&pcaps->lppPosCapabilities[1]);

	pcaps->lppPosCapabilities[0]->fwPosition = WFS_CIM_POSINFRONT;
	pcaps->lppPosCapabilities[0]->fwUsage = WFS_CIM_POSIN;
	pcaps->lppPosCapabilities[0]->bShutterControl = false;
	pcaps->lppPosCapabilities[0]->bItemsTakenSensor = false;
	pcaps->lppPosCapabilities[0]->bItemsInsertedSensor = false;
	pcaps->lppPosCapabilities[0]->fwRetractAreas = WFS_CIM_RA_NOTSUPP;
	pcaps->lppPosCapabilities[0]->lpszExtra = 0;
	pcaps->lppPosCapabilities[0]->bPresentControl = true;


	pcaps->lppPosCapabilities[1]->fwPosition = WFS_CIM_POSOUTFRONT;
	pcaps->lppPosCapabilities[1]->fwUsage = WFS_CIM_POSREFUSE| WFS_CIM_POSROLLBACK;
	pcaps->lppPosCapabilities[1]->bShutterControl = false;
	pcaps->lppPosCapabilities[1]->bItemsTakenSensor = false;
	pcaps->lppPosCapabilities[1]->bItemsInsertedSensor = false;
	pcaps->lppPosCapabilities[1]->fwRetractAreas = WFS_CIM_RA_NOTSUPP;
	pcaps->lppPosCapabilities[1]->lpszExtra = 0;
	pcaps->lppPosCapabilities[1]->bPresentControl = true;
}
void CIMSCN83::getWFS_INF_CIM_REPLENISH_TARGET(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_DEVICELOCK_STATUS(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_CASH_UNIT_CAPABILITIES(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_DEPLETE_SOURCE(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_GET_ALL_ITEMS_INFO(REQUESTID reqId){}
void CIMSCN83::getWFS_INF_CIM_GET_BLACKLIST(REQUESTID reqId){}

void CIMSCN83::execWFS_CMD_CIM_CASH_IN_START(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug("Executing start cash in");
	
	if (!this->canAccept())
	{
		if(this->exchangeActive)
			requests[reqId]->pResult->hResult = WFS_ERR_CIM_EXCHANGEACTIVE;

		if(this->cashInStatus.wStatus == WFS_CIM_CIACTIVE)
			requests[reqId]->pResult->hResult = WFS_ERR_CIM_CASHINACTIVE;
		else
			requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}
	   	 
	notein = 0;
	this->cashInStatus.usNumOfRefused = 0;
	this->cashInStatus.wStatus = WFS_CIM_CIACTIVE;
	Xfs::getInstance()->l.debug("Starting cash in");	

}
void CIMSCN83::execWFS_CMD_CIM_CASH_IN(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug("execWFS_CMD_CIM_CASH_IN");

	if (this->exchangeActive)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_EXCHANGEACTIVE;
		return;		
	}

	if (this->cashInStatus.wStatus != WFS_CIM_CIACTIVE)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOCASHINACTIVE;
		return;
	}

	this->mpost->configureNoteTypes(this->notesConfigured);

	sendWFS_EXEE_CIM_INSERTITEMS(requests[reqId]->hservice);
	

	mpost->accept();
	bool isRejected = false;

	while (true)
	{
		if (mpost->bankNoteInEscrow.isPresent)break;
		if (requests[reqId]->isCancelled.load())break;
		if (mpost->rejected.load()) { 
			isRejected = true;
			break; 
		}
		Sleep(10);
	}
	mpost->disableAccept();

	if (isRejected)
	{
		sendWFS_EXEE_CIM_INPUTREFUSE(requests[reqId]->hservice, WFS_CIM_INVALIDBILL);
		Sleep(10);
		sendWFS_SRVE_CIM_ITEMSPRESENTED(requests[reqId]->hservice, WFS_CIM_POSOUTFRONT, 0, 0);

		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOITEMS;
		this->cashInStatus.usNumOfRefused++;
		return;
	}
	
	if (mpost->bankNoteInEscrow.isPresent)
	{
		this->status.fwIntermediateStacker = WFS_CIM_ISNOTEMPTY;
		this->status.fwStackerItems = WFS_CIM_CUSTOMERACCESS;

		sendWFS_SRVE_CIM_ITEMSINSERTED(requests[reqId]->hservice, WFS_CIM_POSINCENTER, 0, 0);
		this->cashInStatus.lpszExtra = 0;
		WFSCIMNOTENUMBER nn;
		nn.ulCount = 1;

		short nid = 0;
		int  val = mpost->bankNoteInEscrow.noteType.base * pow(10, mpost->bankNoteInEscrow.noteType.exp);

		Xfs::getInstance()->l.debug("mpost->bankNoteInEscrow.noteType.base=" + std::to_string(mpost->bankNoteInEscrow.noteType.base));
		Xfs::getInstance()->l.debug("mpost->bankNoteInEscrow.noteType.exp=" + std::to_string(mpost->bankNoteInEscrow.noteType.exp));
		Xfs::getInstance()->l.debug("val=" + std::to_string(val));
		Xfs::getInstance()->l.debug("notetypes count=" + std::to_string(noteTypes.size()));

		for (auto t : noteTypes)
		{
			if (t.ulValues == val)nid = t.usNoteID;
		}
		nn.usNoteID = nid;
		notesin[nid] += 1;
		notein = nid;
		auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBERLIST), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
		LPWFSCIMNOTENUMBERLIST lpNoteNumberList = (LPWFSCIMNOTENUMBERLIST)requests[reqId]->pResult->lpBuffer;
		lpNoteNumberList->usNumOfNoteNumbers = 1;

		a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMNOTENUMBER)*lpNoteNumberList->usNumOfNoteNumbers, requests[reqId]->pResult, (void **)&lpNoteNumberList->lppNoteNumber);
		a = this->memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBER), requests[reqId]->pResult, (void **)&lpNoteNumberList->lppNoteNumber[0]);

		lpNoteNumberList->lppNoteNumber[0]->ulCount = 1;
		lpNoteNumberList->lppNoteNumber[0]->usNoteID = nid;
		return;
	}

	if (requests[reqId]->isCancelled.load())
	{		
		requests[reqId]->pResult->hResult = WFS_ERR_CANCELED;
		
	}

}
void CIMSCN83::execWFS_CMD_CIM_CASH_IN_END(REQUESTID reqId)
{
	if (this->cashInStatus.wStatus != WFS_CIM_CIACTIVE)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOCASHINACTIVE;
		return;
	}


	Sleep(200);
	mpost->disableAccept();
	this->cashInStatus.wStatus = WFS_CIM_CIOK;
	
	
	if (!notein)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOITEMS;
		return;
	}
	


	setStoredData();
	
	mpost->stack();
	while (mpost->bankNoteInEscrow.isPresent)
	{
		Sleep(100);
	}
			   		 
	
	Sleep(200);
	mpost->disableAccept();
	notein = 0;
	this->cashInStatus.wStatus = WFS_CIM_CIOK;
	this->status.fwIntermediateStacker = WFS_CIM_ISEMPTY;
	this->status.fwStackerItems = WFS_CIM_NOITEMS;

	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMCASHINFO), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	Xfs::getInstance()->l.debug("constructLPWFSCIMCASHIN");

	LPWFSCIMCASHINFO lpcii = (LPWFSCIMCASHINFO)requests[reqId]->pResult->lpBuffer;
	lpcii->usCount = 1;

	a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMCASHIN), requests[reqId]->pResult, (void **)&lpcii->lppCashIn);

	lpcii->lppCashIn[0] = constructLPWFSCIMCASHIN(requests[reqId]->pResult);

	if (totalCount() >= (this->cashin[0].ulMaximum - 100))
	{
		sendWFS_USRE_CIM_CASHUNITTHRESHOLD(this->requests[reqId]->hservice);
	}


	
}
void CIMSCN83::execWFS_CMD_CIM_CASH_IN_ROLLBACK(REQUESTID reqId)
{
	if (this->cashInStatus.wStatus != WFS_CIM_CIACTIVE)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOCASHINACTIVE;
		return;
	}
	

	if (!notein)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOITEMS;
		return;
	}
	   	
	mpost->rollback();
	while (mpost->bankNoteInEscrow.isPresent)
	{
		Sleep(100);
	}
	
	
	mpost->disableAccept();
	this->cashInStatus.wStatus = WFS_CIM_CIOK;
	this->status.fwIntermediateStacker = WFS_CIM_ISEMPTY;
	this->status.fwStackerItems = WFS_CIM_NOITEMS;
}
void CIMSCN83::execWFS_CMD_CIM_RETRACT(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_OPEN_SHUTTER(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_CLOSE_SHUTTER(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_TELLER_INFO(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_CASH_UNIT_INFO(REQUESTID reqId)
{
	
	Xfs::getInstance()->l.debug("execWFS_CMD_CIM_SET_CASH_UNIT_INFO");
	LPWFSCIMCASHINFO cui = (LPWFSCIMCASHINFO)this->requests[reqId]->lpParam;
	
	
		
	this->cashin[0].ulCount = cui->lppCashIn[0]->ulCount;
	this->cashin[0].ulCashInCount = cui->lppCashIn[0]->ulCashInCount;
	this->cashin[0].ulMaximum = cui->lppCashIn[0]->ulMaximum;
	this->cashin[0].bAppLock = cui->lppCashIn[0]->bAppLock;
	this->cashin[0].ulInitialCount = cui->lppCashIn[0]->ulInitialCount;
	this->cashin[0].ulMinimum = cui->lppCashIn[0]->ulMinimum;
	
	if (cui->lppCashIn[0]->lpNoteNumberList)
		for (int i = 0; i < cui->lppCashIn[0]->lpNoteNumberList->usNumOfNoteNumbers; i++)
		{
			auto nid = cui->lppCashIn[0]->lpNoteNumberList->lppNoteNumber[i]->usNoteID;
			auto nic = cui->lppCashIn[0]->lpNoteNumberList->lppNoteNumber[i]->ulCount;
			this->notesin[nid] = nic;
		}
	else
		for (auto n : this->noteTypes)
			this->notesin[n.usNoteID] = 0;

	this->setStoredData();	
	sendWFS_SRVE_CIM_CASHUNITINFOCHANGED(this->requests[reqId]->hservice);
	renewStatus();
}
void CIMSCN83::execWFS_CMD_CIM_START_EXCHANGE(REQUESTID reqId)
{

	if (this->cashInStatus.wStatus == WFS_CIM_CIACTIVE)
	{
		this->requests[reqId]->pResult->hResult = WFS_ERR_CIM_CASHINACTIVE;
		return;
	}
	this->exchangeActive = true;
	

	auto a = this->memAlloc->AllocateMore(sizeof(WFSCIMCASHINFO), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCIMCASHINFO lpcii = (LPWFSCIMCASHINFO)requests[reqId]->pResult->lpBuffer;
	lpcii->usCount = 1;

	a = this->memAlloc->AllocateMore(sizeof(LPWFSCIMCASHIN), requests[reqId]->pResult, (void **)&lpcii->lppCashIn);

	lpcii->lppCashIn[0] = constructLPWFSCIMCASHIN(requests[reqId]->pResult);

	
}
void CIMSCN83::execWFS_CMD_CIM_END_EXCHANGE(REQUESTID reqId)
{
	if (!this->exchangeActive)
	{
		this->requests[reqId]->pResult->hResult = WFS_ERR_CIM_NOEXCHANGEACTIVE;
		return;
	}
	if (!this->mpost->cassetteAttached.load())
	{
		this->requests[reqId]->pResult->hResult = WFS_ERR_CIM_CASHUNITERROR;
		
		sendWFS_EXEE_CIM_CASHUNITERROR(this->requests[reqId]->hservice, WFS_CIM_CASHUNITERROR);
		
		return;
	}

	bool threshold = false;
	if (totalCount() > (this->cashin[0].ulMaximum - 100))
	{
		threshold = true;
	}


	this->exchangeActive = false;
	if (!requests[reqId]->lpParam)
	{
		return;
	}
	LPWFSCIMCASHINFO cui = (LPWFSCIMCASHINFO)this->requests[reqId]->lpParam;


	if (!cui->lppCashIn)return;
	if (!cui->lppCashIn[0])return;
	
		
	if (cui->lppCashIn[0]->lppPhysical&&cui->lppCashIn[0]->lppPhysical[0])
	{
		this->phcu[0].ulCashInCount = cui->lppCashIn[0]->lppPhysical[0]->ulCashInCount;
		this->phcu[0].ulCount = cui->lppCashIn[0]->lppPhysical[0]->ulCount;
		this->phcu[0].ulMaximum = cui->lppCashIn[0]->lppPhysical[0]->ulMaximum;
		this->phcu[0].ulInitialCount = cui->lppCashIn[0]->lppPhysical[0]->ulInitialCount;		
	}
	
	

	this->cashin[0].ulCount = cui->lppCashIn[0]->ulCount;
	this->cashin[0].ulCashInCount = cui->lppCashIn[0]->ulCashInCount;
	this->cashin[0].ulMaximum = cui->lppCashIn[0]->ulMaximum;
	this->cashin[0].bAppLock = cui->lppCashIn[0]->bAppLock;
	this->cashin[0].ulInitialCount = cui->lppCashIn[0]->ulInitialCount;
	this->cashin[0].ulMinimum = cui->lppCashIn[0]->ulMinimum;

	if (cui->lppCashIn[0]->lpNoteNumberList)
		for (int i = 0; i < cui->lppCashIn[0]->lpNoteNumberList->usNumOfNoteNumbers; i++)
		{
			auto nid = cui->lppCashIn[0]->lpNoteNumberList->lppNoteNumber[i]->usNoteID;
			auto nic = cui->lppCashIn[0]->lpNoteNumberList->lppNoteNumber[i]->ulCount;
			this->notesin[nid] = nic;
		}
	else
		for (auto n : this->noteTypes)
			this->notesin[n.usNoteID] = 0;

	this->setStoredData();

	if (totalCount() < (this->cashin[0].ulMaximum - 100) && threshold)
	{			
		sendWFS_USRE_CIM_CASHUNITTHRESHOLD(this->requests[reqId]->hservice);			
	}
	renewStatus();

}
void CIMSCN83::execWFS_CMD_CIM_OPEN_SAFE_DOOR(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_RESET(REQUESTID reqId)
{
	mpost->reset();
}
void CIMSCN83::execWFS_CMD_CIM_CONFIGURE_CASH_IN_UNITS(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_CONFIGURE_NOTETYPES(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug("WFS_CMD_CIM_CONFIGURE_NOTETYPES");


	LPUSHORT lpusNoteIDs = (LPUSHORT)this->requests[reqId]->lpParam;
	 
	
	if (!lpusNoteIDs)return;
	std::string t = "";
	int c = 0;

	this->notesConfigured.clear();
	while (lpusNoteIDs[c])
	{
		this->notesConfigured.push_back(lpusNoteIDs[c]);
		c++;
	}
		
	setStoredData();
	//this->mpost->configureNoteTypes(this->notesConfigured);

}
void CIMSCN83::execWFS_CMD_CIM_CREATE_P6_SIGNATURE(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_GUIDANCE_LIGHT(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_CONFIGURE_NOTE_READER(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_COMPARE_P6_SIGNATURE(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_POWER_SAVE_CONTROL(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_REPLENISH(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_CASH_IN_LIMIT(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_CASH_UNIT_COUNT(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_DEVICE_LOCK_CONTROL(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_MODE(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_PRESENT_MEDIA(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_DEPLETE(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SET_BLACKLIST(REQUESTID reqId){}
void CIMSCN83::execWFS_CMD_CIM_SYNCHRONIZE_COMMAND(REQUESTID reqId){}





void CIMSCN83::getInfoProc(REQUESTID reqId) 
{
		if (requests[reqId]->cmd == WFS_INF_CIM_STATUS)
		{
			getWFS_INF_CIM_STATUS(reqId);
		}
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_CAPABILITIES)
	{
		getWFS_INF_CIM_CAPABILITIES(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_BANKNOTE_TYPES)
	{
		getWFS_INF_CIM_BANKNOTE_TYPES(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_CURRENCY_EXP)
	{
		getWFS_INF_CIM_CURRENCY_EXP(reqId);
	}		
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_CASH_UNIT_INFO)
	{
		getWFS_INF_CIM_CASH_UNIT_INFO(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_CASH_IN_STATUS)
	{
		getWFS_INF_CIM_CASH_IN_STATUS(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CIM_POSITION_CAPABILITIES)
	{
		getWFS_INF_CIM_POSITION_CAPABILITIES(reqId);
	}
	else
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}

}
void CIMSCN83::executeProc(REQUESTID reqId) 
{
	if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_IN_START)
	{
		execWFS_CMD_CIM_CASH_IN_START(reqId);		
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_IN)
	{
		execWFS_CMD_CIM_CASH_IN(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_IN_END)
	{
		execWFS_CMD_CIM_CASH_IN_END(reqId);
		
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_IN_ROLLBACK)
	{
		execWFS_CMD_CIM_CASH_IN_ROLLBACK(reqId);
		
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_RESET)
	{
		execWFS_CMD_CIM_RESET(reqId);

	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_SET_CASH_UNIT_INFO)
	{
		execWFS_CMD_CIM_SET_CASH_UNIT_INFO(reqId);

	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_START_EXCHANGE)
	{
		execWFS_CMD_CIM_START_EXCHANGE(reqId);

	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_END_EXCHANGE)
	{
		execWFS_CMD_CIM_END_EXCHANGE(reqId);

	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CIM_CONFIGURE_NOTETYPES)
	{
		execWFS_CMD_CIM_CONFIGURE_NOTETYPES(reqId);
	}
	else
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
}
void CIMSCN83::copyLpInput(LPVOID lpData, REQUESTID reqId)
{

	if (requests[reqId]->messageType == MSG_EXECUTE && lpData)
	{
		if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_IN_START) 		
		{
			LPWFSCIMCASHINSTART lpCashInStarts = (LPWFSCIMCASHINSTART)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMCASHINSTART), (void **)&requests[reqId]->lpParam);

			LPWFSCIMCASHINSTART lpCashInStartd = (LPWFSCIMCASHINSTART)requests[reqId]->lpParam;
			lpCashInStartd->usTellerID = lpCashInStarts->usTellerID;
			lpCashInStartd->bUseRecycleUnits = lpCashInStarts->bUseRecycleUnits;
			lpCashInStartd->fwOutputPosition = lpCashInStarts->fwOutputPosition;
			lpCashInStartd->fwInputPosition = lpCashInStarts->fwInputPosition;
		}
						
		if (requests[reqId]->cmd == WFS_CMD_CIM_RETRACT) 
		{
			LPWFSCIMRETRACT lpRetracts = (LPWFSCIMRETRACT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMRETRACT), (void **)&requests[reqId]->lpParam);

			LPWFSCIMRETRACT lpRetractd = (LPWFSCIMRETRACT)requests[reqId]->lpParam;
			lpRetractd->fwOutputPosition = lpRetracts->fwOutputPosition;
			lpRetractd->usIndex = lpRetracts->usIndex;
			lpRetractd->usRetractArea = lpRetractd->usRetractArea;

		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_OPEN_SHUTTER) 
		{
			LPWORD lpfwPositions = (LPWORD)lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpfwPositiond = (LPWORD)requests[reqId]->lpParam;
			*lpfwPositiond = *lpfwPositions;

		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_CLOSE_SHUTTER) 
		{
			LPWORD lpfwPositions = (LPWORD)lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpfwPositiond = (LPWORD)requests[reqId]->lpParam;
			*lpfwPositiond = *lpfwPositions;
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_TELLER_INFO) 
		{
			throw "Teller info is not supported";
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_CASH_UNIT_INFO || requests[reqId]->cmd == WFS_CMD_CIM_END_EXCHANGE)
		{
			Xfs::getInstance()->l.debug("WFS_CMD_CIM_SET_CASH_UNIT_INFO copy data");
			LPWFSCIMCASHINFO lpCUInfos = (LPWFSCIMCASHINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMCASHINFO), (void **)&requests[reqId]->lpParam);

			LPWFSCIMCASHINFO lpCUInfod = (LPWFSCIMCASHINFO)requests[reqId]->lpParam;			
			lpCUInfod->usCount = lpCUInfos->usCount;
			memAlloc->AllocateMore(lpCUInfos->usCount*sizeof(LPWFSCIMCASHIN), requests[reqId]->lpParam,(void **)&lpCUInfod->lppCashIn);
			
			for (int i = 0; i < lpCUInfod->usCount; i++)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMCASHIN), requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]);
				lpCUInfod->lppCashIn[i]->bAppLock = lpCUInfos->lppCashIn[i]->bAppLock;
				lpCUInfod->lppCashIn[i]->usNumber = lpCUInfos->lppCashIn[i]->usNumber;
				lpCUInfod->lppCashIn[i]->fwType = lpCUInfos->lppCashIn[i]->fwType;
				lpCUInfod->lppCashIn[i]->fwItemType = lpCUInfos->lppCashIn[i]->fwItemType;

				for(int j=0;j<5;j++)
					lpCUInfod->lppCashIn[i]->cUnitID[j] = lpCUInfos->lppCashIn[i]->cUnitID[j];				
				for (int j = 0; j < 3; j++)
					lpCUInfod->lppCashIn[i]->cCurrencyID[j] = lpCUInfos->lppCashIn[i]->cCurrencyID[j];

				lpCUInfod->lppCashIn[i]->ulValues = lpCUInfos->lppCashIn[i]->ulValues;
				lpCUInfod->lppCashIn[i]->ulCashInCount = lpCUInfos->lppCashIn[i]->ulCashInCount;
				lpCUInfod->lppCashIn[i]->ulCount = lpCUInfos->lppCashIn[i]->ulCashInCount;
				lpCUInfod->lppCashIn[i]->ulMaximum = lpCUInfos->lppCashIn[i]->ulMaximum;
				lpCUInfod->lppCashIn[i]->usStatus = lpCUInfos->lppCashIn[i]->usStatus;
				lpCUInfod->lppCashIn[i]->bAppLock = lpCUInfos->lppCashIn[i]->bAppLock;
				
				memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBERLIST), requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpNoteNumberList);				
				lpCUInfod->lppCashIn[i]->lpNoteNumberList->usNumOfNoteNumbers = lpCUInfos->lppCashIn[i]->lpNoteNumberList->usNumOfNoteNumbers;
				memAlloc->AllocateMore(sizeof(LPWFSCIMNOTENUMBER)*lpCUInfod->lppCashIn[i]->lpNoteNumberList->usNumOfNoteNumbers, requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpNoteNumberList->lppNoteNumber);
				
				for (int j = 0; j < lpCUInfod->lppCashIn[i]->lpNoteNumberList->usNumOfNoteNumbers; j++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMNOTENUMBER), requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpNoteNumberList->lppNoteNumber[j]);
					lpCUInfod->lppCashIn[i]->lpNoteNumberList->lppNoteNumber[j]->ulCount = lpCUInfos->lppCashIn[i]->lpNoteNumberList->lppNoteNumber[j]->ulCount;
					lpCUInfod->lppCashIn[i]->lpNoteNumberList->lppNoteNumber[j]->usNoteID = lpCUInfos->lppCashIn[i]->lpNoteNumberList->lppNoteNumber[j]->usNoteID;
				}

				lpCUInfod->lppCashIn[i]->usNumPhysicalCUs = lpCUInfos->lppCashIn[i]->usNumPhysicalCUs;
				
				memAlloc->AllocateMore(sizeof(LPWFSCIMPHCU)*lpCUInfod->lppCashIn[i]->usNumPhysicalCUs, requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lppPhysical);
				
				for (int j = 0; j < lpCUInfod->lppCashIn[i]->usNumPhysicalCUs; j++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMPHCU), requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lppPhysical[j]);				
					
					

					if (lpCUInfos->lppCashIn[i]->lppPhysical[j]->lpPhysicalPositionName)
					{
						memAlloc->AllocateMore(strnlen(lpCUInfos->lppCashIn[i]->lppPhysical[j]->lpPhysicalPositionName, 100) + 1
							, requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lppPhysical[j]->lpPhysicalPositionName);
						
						strcpy(lpCUInfod->lppCashIn[i]->lppPhysical[j]->lpPhysicalPositionName, lpCUInfos->lppCashIn[i]->lppPhysical[j]->lpPhysicalPositionName);
					}


					for(int k=0;k<5;k++)
						lpCUInfod->lppCashIn[i]->lppPhysical[j]->cUnitID[k] = lpCUInfos->lppCashIn[i]->lppPhysical[j]->cUnitID[k];
					
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulCashInCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulCashInCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulMaximum = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulMaximum;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->usPStatus = lpCUInfos->lppCashIn[i]->lppPhysical[j]->usPStatus;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->bHardwareSensors = lpCUInfos->lppCashIn[i]->lppPhysical[j]->bHardwareSensors;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->lpszExtra = 0;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulInitialCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulInitialCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulDispensedCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulDispensedCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulPresentedCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulPresentedCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulRetractedCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulRetractedCount;
					lpCUInfod->lppCashIn[i]->lppPhysical[j]->ulRejectCount = lpCUInfos->lppCashIn[i]->lppPhysical[j]->ulRejectCount;

				}
								
				lpCUInfod->lppCashIn[i]->lpszExtra = 0;///

				if (lpCUInfos->lppCashIn[i]->lpusNoteIDs)
				{
					int k = 0;
					while (lpCUInfos->lppCashIn[i]->lpusNoteIDs[k])k++;
					memAlloc->AllocateMore((k + 1) * 4, requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpusNoteIDs);

					

					lpCUInfod->lppCashIn[i]->lpusNoteIDs[k] = 0;
					while (k >= 0)
					{
						k--;
						memAlloc->AllocateMore(sizeof(short), requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpusNoteIDs[k]);
						lpCUInfod->lppCashIn[i]->lpusNoteIDs[k] = lpCUInfos->lppCashIn[i]->lpusNoteIDs[k];
					}

					
				}

				lpCUInfod->lppCashIn[i]->usCDMType = lpCUInfos->lppCashIn[i]->usCDMType;
				

				if (lpCUInfos->lppCashIn[i]->lpszCashUnitName)
				{
					memAlloc->AllocateMore(strnlen(lpCUInfos->lppCashIn[i]->lpszCashUnitName, 100) + 1, requests[reqId]->lpParam, (void **)&lpCUInfod->lppCashIn[i]->lpszCashUnitName);

					strcpy(lpCUInfod->lppCashIn[i]->lpszCashUnitName, lpCUInfos->lppCashIn[i]->lpszCashUnitName);
				}
				


				lpCUInfod->lppCashIn[i]->ulInitialCount = lpCUInfos->lppCashIn[i]->ulInitialCount;
				lpCUInfod->lppCashIn[i]->ulDispensedCount = lpCUInfos->lppCashIn[i]->ulDispensedCount;
				lpCUInfod->lppCashIn[i]->ulPresentedCount = lpCUInfos->lppCashIn[i]->ulPresentedCount;
				lpCUInfod->lppCashIn[i]->ulRetractedCount = lpCUInfos->lppCashIn[i]->ulRetractedCount;
				lpCUInfod->lppCashIn[i]->ulRejectCount = lpCUInfos->lppCashIn[i]->ulRejectCount;
				lpCUInfod->lppCashIn[i]->ulMinimum = lpCUInfos->lppCashIn[i]->ulMinimum;

			}					
			
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_START_EXCHANGE) 
		{
			LPWFSCIMSTARTEX lpStartExs = (LPWFSCIMSTARTEX)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMSTARTEX), (void **)&requests[reqId]->lpParam);
			LPWFSCIMSTARTEX lpStartExd = (LPWFSCIMSTARTEX)requests[reqId]->lpParam;
			lpStartExd->usCount = lpStartExs->usCount;
			lpStartExd->usTellerID = lpStartExs->usTellerID;
			lpStartExd->fwExchangeType = lpStartExs->fwExchangeType;
						
			memAlloc->AllocateMore(lpStartExd->usCount * sizeof(short), requests[reqId]->lpParam, (void **)&lpStartExd->lpusCUNumList);
			for (int i = 0; i < lpStartExd->usCount; i++)
				lpStartExd->lpusCUNumList[i] = lpStartExs->lpusCUNumList[i];
			if (lpStartExd->lpOutput)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMOUTPUT), requests[reqId]->lpParam, (void **)&lpStartExd->lpOutput);
				lpStartExd->lpOutput->fwPosition = lpStartExs->lpOutput->fwPosition;
				lpStartExd->lpOutput->usLogicalNumber = lpStartExs->lpOutput->usLogicalNumber;
				lpStartExd->lpOutput->usNumber = lpStartExs->lpOutput->usNumber;
			}					   			 		  
		}			

		
		if (requests[reqId]->cmd == WFS_CMD_CIM_RESET) 
		{			
			LPWFSCIMITEMPOSITION lpResetIns = (LPWFSCIMITEMPOSITION)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMITEMPOSITION), (void **)&requests[reqId]->lpParam);
			LPWFSCIMITEMPOSITION lpResetInd = (LPWFSCIMITEMPOSITION)requests[reqId]->lpParam;
			lpResetInd->fwOutputPosition = lpResetIns->fwOutputPosition;
			lpResetInd->usNumber = lpResetIns->usNumber;
			if (lpResetIns->lpRetractArea)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMITEMPOSITION), requests[reqId]->lpParam, (void **)&lpResetInd->lpRetractArea);
				lpResetInd->lpRetractArea->fwOutputPosition = lpResetIns->lpRetractArea->fwOutputPosition;
				lpResetInd->lpRetractArea->usIndex = lpResetIns->lpRetractArea->usIndex;
				lpResetInd->lpRetractArea->usRetractArea = lpResetIns->lpRetractArea->usRetractArea;
			}


		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_CONFIGURE_CASH_IN_UNITS) 
		{
			LPWFSCIMCASHINTYPE *lppCashInTypes = (LPWFSCIMCASHINTYPE*)lpData;
			int  c = 0;
			while (*lppCashInTypes)
			{
				c++;
				lppCashInTypes++;
			}
			lppCashInTypes = (LPWFSCIMCASHINTYPE*)lpData;
			memAlloc->AllocateBuffer(sizeof(LPWFSCIMCASHINTYPE)*(c+1), (void **)&requests[reqId]->lpParam);

			LPWFSCIMCASHINTYPE *lppCashInTyped = (LPWFSCIMCASHINTYPE*)requests[reqId]->lpParam;
			lppCashInTyped[c] = 0;
			for (int i = 0; i < c; i++)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMCASHINTYPE), requests[reqId]->lpParam, (void **)&lppCashInTyped[i]);

				lppCashInTyped[i]->dwType = lppCashInTypes[i]->dwType;
				lppCashInTyped[i]->usNumber = lppCashInTypes[i]->usNumber;
				
				int j = 0;
				while (lppCashInTypes[i]->lpusNoteIDs[j])j++;				
				memAlloc->AllocateMore(sizeof(short)*(j+1), requests[reqId]->lpParam, (void **)&lppCashInTyped[i]->lpusNoteIDs);
				lppCashInTyped[i]->lpusNoteIDs[j] = 0;
				for (int k = 0; k < j; k++)
					lppCashInTyped[i]->lpusNoteIDs[k] = lppCashInTypes[i]->lpusNoteIDs[k];

			}


		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_CONFIGURE_NOTETYPES) 
		{
			LPUSHORT lpusNoteIDss = (LPUSHORT)lpData;
			int c = 0;
			while (*lpusNoteIDss)
			{
				lpusNoteIDss++;
				c++;
			}
			lpusNoteIDss = (LPUSHORT)lpData;
			Xfs::getInstance()->l.debug("NUM OF NOTES="+std::to_string(c));
			memAlloc->AllocateBuffer(sizeof(USHORT)*(c + 1), (void **)&requests[reqId]->lpParam);

			LPUSHORT lpusNoteIDsd = (LPUSHORT)requests[reqId]->lpParam;
			lpusNoteIDsd[c] = 0;
			for (int i = 0; i < c; i++)
			{
				
				lpusNoteIDsd[i] = lpusNoteIDss[i];
			}
				

		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_GUIDANCE_LIGHT) 
		{
			LPWFSCIMSETGUIDLIGHT lpSetGuidLights = (LPWFSCIMSETGUIDLIGHT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMSETGUIDLIGHT), (void **)&requests[reqId]->lpParam);
			LPWFSCIMSETGUIDLIGHT lpSetGuidLightd = (LPWFSCIMSETGUIDLIGHT)requests[reqId]->lpParam;
			lpSetGuidLightd->dwCommand = lpSetGuidLights->dwCommand;
			lpSetGuidLightd->wGuidLight = lpSetGuidLightd->wGuidLight;
		}


		if (requests[reqId]->cmd == WFS_CMD_CIM_CONFIGURE_NOTE_READER) 
		{
			LPWFSCIMCONFIGURENOTEREADER lpConfigureNoteReaders = (LPWFSCIMCONFIGURENOTEREADER)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMCONFIGURENOTEREADER), (void **)&requests[reqId]->lpParam);
			LPWFSCIMCONFIGURENOTEREADER lpConfigureNoteReaderd = (LPWFSCIMCONFIGURENOTEREADER)requests[reqId]->lpParam;
			lpConfigureNoteReaderd->bLoadAlways = lpConfigureNoteReaders->bLoadAlways;
		}
		
		if (requests[reqId]->cmd == WFS_CMD_CIM_COMPARE_P6_SIGNATURE) 
		{
			LPWFSCIMP6COMPARESIGNATURE lpP6CompareSignatures = (LPWFSCIMP6COMPARESIGNATURE)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMP6COMPARESIGNATURE), (void **)&requests[reqId]->lpParam);
			LPWFSCIMP6COMPARESIGNATURE lpP6CompareSignatured = (LPWFSCIMP6COMPARESIGNATURE)requests[reqId]->lpParam;
			if (lpP6CompareSignatures->lppP6ReferenceSignatures)
			{
				int  c = 0;
				while (lpP6CompareSignatures->lppP6ReferenceSignatures[c])c++;
				memAlloc->AllocateMore(4*(c + 1), requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6ReferenceSignatures);
				lpP6CompareSignatured->lppP6ReferenceSignatures[c] = 0;
				for (int i = 0; i < c; i++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMP6SIGNATURE), requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6ReferenceSignatures[i]);

					lpP6CompareSignatured->lppP6ReferenceSignatures[i]->usNoteId = lpP6CompareSignatures->lppP6ReferenceSignatures[i]->usNoteId;
					lpP6CompareSignatured->lppP6ReferenceSignatures[i]->dwOrientation = lpP6CompareSignatures->lppP6ReferenceSignatures[i]->dwOrientation;
					lpP6CompareSignatured->lppP6ReferenceSignatures[i]->ulLength = lpP6CompareSignatures->lppP6ReferenceSignatures[i]->ulLength;
					
					memAlloc->AllocateMore(lpP6CompareSignatured->lppP6ReferenceSignatures[i]->ulLength, requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6ReferenceSignatures[i]->lpSignature);
					memcpy_s(lpP6CompareSignatured->lppP6ReferenceSignatures[i]->lpSignature, lpP6CompareSignatured->lppP6ReferenceSignatures[i]->ulLength, lpP6CompareSignatures->lppP6ReferenceSignatures[i]->lpSignature, lpP6CompareSignatures->lppP6ReferenceSignatures[i]->ulLength);					
				}
			}
			if (lpP6CompareSignatures->lppP6Signatures)
			{
				int  c = 0;
				while (lpP6CompareSignatures->lppP6Signatures[c])c++;
				memAlloc->AllocateMore(4 * (c + 1), requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6Signatures);
				lpP6CompareSignatured->lppP6Signatures[c] = 0;
				for (int i = 0; i < c; i++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMP6SIGNATURE), requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6Signatures[i]);

					lpP6CompareSignatured->lppP6Signatures[i]->usNoteId = lpP6CompareSignatures->lppP6Signatures[i]->usNoteId;
					lpP6CompareSignatured->lppP6Signatures[i]->dwOrientation = lpP6CompareSignatures->lppP6Signatures[i]->dwOrientation;
					lpP6CompareSignatured->lppP6Signatures[i]->ulLength = lpP6CompareSignatures->lppP6Signatures[i]->ulLength;

					memAlloc->AllocateMore(lpP6CompareSignatured->lppP6Signatures[i]->ulLength, requests[reqId]->lpParam, (void **)&lpP6CompareSignatured->lppP6Signatures[i]->lpSignature);
					memcpy_s(lpP6CompareSignatured->lppP6Signatures[i]->lpSignature, lpP6CompareSignatured->lppP6Signatures[i]->ulLength, lpP6CompareSignatures->lppP6Signatures[i]->lpSignature, lpP6CompareSignatures->lppP6Signatures[i]->ulLength);
				}
			}
		}


		if (requests[reqId]->cmd == WFS_CMD_CIM_POWER_SAVE_CONTROL) 
		{
			LPWFSCIMPOWERSAVECONTROL lpPowerSaveControls = (LPWFSCIMPOWERSAVECONTROL)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMPOWERSAVECONTROL), (void **)&requests[reqId]->lpParam);
			LPWFSCIMPOWERSAVECONTROL lpPowerSaveControld = (LPWFSCIMPOWERSAVECONTROL)requests[reqId]->lpParam;
			lpPowerSaveControld->usMaxPowerSaveRecoveryTime = lpPowerSaveControls->usMaxPowerSaveRecoveryTime;
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_REPLENISH) 
		{
			LPWFSCIMREP lpReplenishs = (LPWFSCIMREP)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMREP), (void **)&requests[reqId]->lpParam);
			LPWFSCIMREP lpReplenishd = (LPWFSCIMREP)requests[reqId]->lpParam;
			lpReplenishd->usNumberSource = lpReplenishs->usNumberSource;
			int c = 0;
			while (lpReplenishs->lppReplenishTargets[c])c++;

			memAlloc->AllocateMore(4*(c+1), requests[reqId]->lpParam, (void **)&lpReplenishd->lppReplenishTargets);
			lpReplenishd->lppReplenishTargets[c] = 0;
			for (int i = 0; i < c; i++)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMREPTARGET), requests[reqId]->lpParam, (void **)&lpReplenishd->lppReplenishTargets[i]);
				lpReplenishd->lppReplenishTargets[i]->bRemoveAll = lpReplenishs->lppReplenishTargets[i]->bRemoveAll;
				lpReplenishd->lppReplenishTargets[i]->ulNumberOfItemsToMove = lpReplenishs->lppReplenishTargets[i]->ulNumberOfItemsToMove;
				lpReplenishd->lppReplenishTargets[i]->usNumberTarget = lpReplenishs->lppReplenishTargets[i]->usNumberTarget;
					
			}
		}


		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_CASH_IN_LIMIT) 
		{
			LPWFSCIMCASHINLIMIT lpCashInLimits = (LPWFSCIMCASHINLIMIT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMCASHINLIMIT), (void **)&requests[reqId]->lpParam);
			LPWFSCIMCASHINLIMIT lpCashInLimitd = (LPWFSCIMCASHINLIMIT)requests[reqId]->lpParam;
			lpCashInLimitd->ulTotalItemsLimit = lpCashInLimits->ulTotalItemsLimit;
			if (lpCashInLimits->lpAmountLimit) 
			{
				memAlloc->AllocateMore(sizeof(WFSCIMAMOUNTLIMIT), requests[reqId]->lpParam, (void **)&lpCashInLimitd->lpAmountLimit);
				lpCashInLimitd->lpAmountLimit->cCurrencyID[0] = lpCashInLimits->lpAmountLimit->cCurrencyID[0];
				lpCashInLimitd->lpAmountLimit->cCurrencyID[1] = lpCashInLimits->lpAmountLimit->cCurrencyID[1];
				lpCashInLimitd->lpAmountLimit->cCurrencyID[2] = lpCashInLimits->lpAmountLimit->cCurrencyID[2];
				lpCashInLimitd->lpAmountLimit->ulAmount = lpCashInLimits->lpAmountLimit->ulAmount;

			}

			
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_CASH_UNIT_COUNT) 
		{
			LPWFSCIMCOUNT lpCounts = (LPWFSCIMCOUNT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMCOUNT), (void **)&requests[reqId]->lpParam);
			LPWFSCIMCOUNT lpCountd = (LPWFSCIMCOUNT)requests[reqId]->lpParam;
			lpCountd->usCount = lpCounts->usCount;
			memAlloc->AllocateMore(sizeof(USHORT)*lpCounts->usCount, requests[reqId]->lpParam, (void **)&lpCountd->lpusCUNumList);
			for (int i = 0; i < lpCountd->usCount; i++)
				lpCountd->lpusCUNumList[i] = lpCounts->lpusCUNumList[i];

		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_DEVICE_LOCK_CONTROL) 
		{
			LPWFSCIMDEVICELOCKCONTROL lpDeviceLockControls = (LPWFSCIMDEVICELOCKCONTROL)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMDEVICELOCKCONTROL), (void **)&requests[reqId]->lpParam);
			LPWFSCIMDEVICELOCKCONTROL lpDeviceLockControld = (LPWFSCIMDEVICELOCKCONTROL)requests[reqId]->lpParam;
			lpDeviceLockControld->wDeviceAction = lpDeviceLockControls->wDeviceAction;
			lpDeviceLockControld->wCashUnitAction = lpDeviceLockControls->wCashUnitAction;

			if (lpDeviceLockControls->lppUnitLockControl)
			{
				int c = 0;
				while (lpDeviceLockControls->lppUnitLockControl[c])c++;
				memAlloc->AllocateMore(4*(c+1), requests[reqId]->lpParam, (void **)&lpDeviceLockControld->lppUnitLockControl);
				lpDeviceLockControld->lppUnitLockControl[c] = 0;
				for (int i = 0; i < c; i++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMUNITLOCKCONTROL), requests[reqId]->lpParam, (void **)&lpDeviceLockControld->lppUnitLockControl[i]);
					lpDeviceLockControld->lppUnitLockControl[i]->wUnitAction = lpDeviceLockControls->lppUnitLockControl[i]->wUnitAction;
					memAlloc->AllocateMore(strlen(lpDeviceLockControls->lppUnitLockControl[i]->lpPhysicalPositionName)+1, requests[reqId]->lpParam, (void **)&lpDeviceLockControld->lppUnitLockControl[i]->lpPhysicalPositionName);
					strcpy(lpDeviceLockControld->lppUnitLockControl[i]->lpPhysicalPositionName, lpDeviceLockControls->lppUnitLockControl[i]->lpPhysicalPositionName);
				}
			}
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_MODE) 
		{
			LPWFSCIMSETMODE lpModes = (LPWFSCIMSETMODE)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMSETMODE), (void **)&requests[reqId]->lpParam);
			LPWFSCIMSETMODE lpModed = (LPWFSCIMSETMODE)requests[reqId]->lpParam;
			lpModed->wMixedMode = lpModes->wMixedMode;
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_PRESENT_MEDIA) 
		{
			LPWFSCIMPRESENT lpPresents = (LPWFSCIMPRESENT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMPRESENT), (void **)&requests[reqId]->lpParam);
			LPWFSCIMPRESENT lpPresentd = (LPWFSCIMPRESENT)requests[reqId]->lpParam;
			lpPresentd->fwPosition = lpPresents->fwPosition;
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_DEPLETE) 
		{
			LPWFSCIMDEP lpDepletes = (LPWFSCIMDEP)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMDEP), (void **)&requests[reqId]->lpParam);
			LPWFSCIMDEP lpDepleted = (LPWFSCIMDEP)requests[reqId]->lpParam;
			lpDepleted->usNumberTarget = lpDepletes->usNumberTarget;
			int c = 0;
			while (lpDepletes->lppDepleteSources[c])c++;
			
			memAlloc->AllocateMore(sizeof(LPWFSCIMDEPSOURCE)* (c+1), requests[reqId]->lpParam, (void **)&lpDepleted->lppDepleteSources);
			lpDepleted->lppDepleteSources[c] = 0;
			for (int i = 0; i < c; i++)
			{
				memAlloc->AllocateMore(sizeof(WFSCIMDEPSOURCE), requests[reqId]->lpParam, (void **)&lpDepleted->lppDepleteSources[i]);
				lpDepleted->lppDepleteSources[i]->bRemoveAll = lpDepletes->lppDepleteSources[i]->bRemoveAll;
				lpDepleted->lppDepleteSources[i]->ulNumberOfItemsToMove = lpDepletes->lppDepleteSources[i]->ulNumberOfItemsToMove;
				lpDepleted->lppDepleteSources[i]->usNumberSource = lpDepletes->lppDepleteSources[i]->usNumberSource;
			}
		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SET_BLACKLIST) 
		{
			LPWFSCIMBLACKLIST lpBlacklists = (LPWFSCIMBLACKLIST)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMBLACKLIST), (void **)&requests[reqId]->lpParam);
			LPWFSCIMBLACKLIST lpBlacklistd = (LPWFSCIMBLACKLIST)requests[reqId]->lpParam;
			lpBlacklistd->usCount = lpBlacklists->usCount;
			
			memAlloc->AllocateMore(lstrlenW(lpBlacklists->lpszVersion), requests[reqId]->lpParam, (void **)&lpBlacklistd->lpszVersion);
			lstrcpyW(lpBlacklistd->lpszVersion, lpBlacklists->lpszVersion);
			if (lpBlacklistd->lppBlacklistElements)
			{
				int c = 0;
				while (lpBlacklists->lppBlacklistElements[c])c++;
				memAlloc->AllocateMore(4*(c+1), requests[reqId]->lpParam, (void **)&lpBlacklistd->lppBlacklistElements);
				lpBlacklistd->lppBlacklistElements[c] = 0;
				for (int i = 0; i < c; i++)
				{
					memAlloc->AllocateMore(sizeof(WFSCIMBLACKLISTELEMENT), requests[reqId]->lpParam, (void **)&lpBlacklistd->lppBlacklistElements[i]);
					lpBlacklistd->lppBlacklistElements[i]->cCurrencyID[0] = lpBlacklists->lppBlacklistElements[i]->cCurrencyID[0];
					lpBlacklistd->lppBlacklistElements[i]->cCurrencyID[1] = lpBlacklists->lppBlacklistElements[i]->cCurrencyID[1];
					lpBlacklistd->lppBlacklistElements[i]->cCurrencyID[2] = lpBlacklists->lppBlacklistElements[i]->cCurrencyID[2];
					lpBlacklistd->lppBlacklistElements[i]->ulValue = lpBlacklists->lppBlacklistElements[i]->ulValue;
					memAlloc->AllocateMore(lstrlenW(lpBlacklists->lppBlacklistElements[i]->lpszSerialNumber)+1, requests[reqId]->lpParam, (void **)&lpBlacklistd->lppBlacklistElements[i]->lpszSerialNumber);
					lstrcpyW(lpBlacklists->lppBlacklistElements[i]->lpszSerialNumber, lpBlacklistd->lppBlacklistElements[i]->lpszSerialNumber);
				}
			}
			

		}
		if (requests[reqId]->cmd == WFS_CMD_CIM_SYNCHRONIZE_COMMAND) 
		{
			throw "WFS_CMD_CIM_SYNCHRONIZE_COMMAND is not supported";
		}
	}

	if (requests[reqId]->messageType == MSG_GETINFO && lpData)
	{		
		if (requests[reqId]->cmd == WFS_INF_CIM_GET_P6_SIGNATURE) 
		{
			LPWFSCIMGETP6SIGNATURE lpGetP6Signatures = (LPWFSCIMGETP6SIGNATURE)lpData;			
			memAlloc->AllocateBuffer(sizeof(WFSCIMGETP6SIGNATURE), (void **)&requests[reqId]->lpParam);
			LPWFSCIMGETP6SIGNATURE lpGetP6Signatured = (LPWFSCIMGETP6SIGNATURE)requests[reqId]->lpParam;
			lpGetP6Signatured->usIndex = lpGetP6Signatures->usIndex;
			lpGetP6Signatured->usLevel = lpGetP6Signatures->usLevel;

		}
		if (requests[reqId]->cmd == WFS_INF_CIM_GET_ITEM_INFO) 
		{
			LPWFSCIMGETITEMINFO lpGetItemInfos = (LPWFSCIMGETITEMINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMGETITEMINFO), (void **)&requests[reqId]->lpParam);
			LPWFSCIMGETITEMINFO lpGetItemInfod = (LPWFSCIMGETITEMINFO)requests[reqId]->lpParam;
			lpGetItemInfod->dwItemInfoType = lpGetItemInfos->dwItemInfoType;
			lpGetItemInfod->usIndex = lpGetItemInfos->usIndex;
			lpGetItemInfod->usLevel = lpGetItemInfos->usLevel;
		}
		if (requests[reqId]->cmd == WFS_INF_CIM_REPLENISH_TARGET)
		{
			LPWFSCIMREPINFO lpReplenishInfos = (LPWFSCIMREPINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMREPINFO), (void **)&requests[reqId]->lpParam);
			LPWFSCIMREPINFO lpReplenishInfod = (LPWFSCIMREPINFO)requests[reqId]->lpParam;
			lpReplenishInfod->usNumberSource = lpReplenishInfos->usNumberSource;
		}
		if (requests[reqId]->cmd == WFS_INF_CIM_DEPLETE_SOURCE)
		{
			LPWFSCIMDEPINFO lpDepleteInfos = (LPWFSCIMDEPINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMDEPINFO), (void **)&requests[reqId]->lpParam);
			LPWFSCIMDEPINFO lpDepleteInfod = (LPWFSCIMDEPINFO)requests[reqId]->lpParam;
			lpDepleteInfod->usNumberTarget = lpDepleteInfos->usNumberTarget;
		}
		if (requests[reqId]->cmd == WFS_INF_CIM_GET_ALL_ITEMS_INFO) 
		{
			LPWFSCIMGETALLITEMSINFO lpGetAllItemsInfos = (LPWFSCIMGETALLITEMSINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCIMGETALLITEMSINFO), (void **)&requests[reqId]->lpParam);
			LPWFSCIMGETALLITEMSINFO lpGetAllItemsInfod = (LPWFSCIMGETALLITEMSINFO)requests[reqId]->lpParam;
			lpGetAllItemsInfod->usLevel = lpGetAllItemsInfos->usLevel;

		}
	}

}


void CIMSCN83::noteInEscrow()
{
	
}

void CIMSCN83::changed()
{
	if (mpost->bankNoteInEscrow.isPresent)
	{
		noteInEscrow();
	}
}

CIMSCN83::CIMSCN83(volatile SharedData* s, unsigned short version, std::string port, int baudRate, std::string lname) :GenericDevice(s, version)
{	
	this->port = port;
	this->baudRate = baudRate;
	this->configName = lname;
	
	Xfs::getInstance()->l.debug("CIMSCN83::CIMSCN83 executing, port="+this->port);

	mpost = new MpostLite(this->port);	
	mpost->changed.connect(boost::bind((&CIMSCN83::changed), this));
	
	
	initNoteTypes();
	initExponent();
	initCapabilities();
	initCashInStatus();
	initCashInInfo();
	getStoredData();
	renewStatus();
	
	
}

void CIMSCN83::getStoredData()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);
	for(auto n : this->noteTypes )
		this->notesin[n.usNoteID] = pt.get<int>("note"+std::to_string(n.usNoteID),0);

	
	this->cashin[0].ulMaximum = pt.get<int>("maxCassete", 0);
	this->phcu[0].ulMaximum = this->cashin[0].ulMaximum;
	this->cashin[0].ulMinimum = pt.get<int>("minCassete", 0);
	this->cashin[0].ulRejectCount = pt.get<int>("rejected", 0);
	this->phcu[0].ulRejectCount = this->cashin[0].ulRejectCount;

	std::string sConfiguredNotes = pt.get<std::string>("notesConfigured", "");
	this->notesConfigured.clear();
	for (int i = 0; i < sConfiguredNotes.size(); i++)
	{
		std::string n = "";
		n += sConfiguredNotes[i];
		this->notesConfigured.push_back(atoi(n.data()));
	}
	
	for (auto i : this->notesConfigured)
	{
		Xfs::getInstance()->l.debug("note configured:"+std::to_string(i));
	}

}

void CIMSCN83::setStoredData()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);

	for (auto n : this->noteTypes)
		pt.put("note" + std::to_string(n.usNoteID), this->notesin[n.usNoteID]);

	pt.put("maxCassete" , this->cashin[0].ulMaximum);
	pt.put("minCassete", this->cashin[0].ulMinimum);
	pt.put("rejected", this->cashin[0].ulRejectCount);

	std::string sConfiguredNotes;
	for (auto n : this->notesConfigured)
		sConfiguredNotes += std::to_string(n);
	pt.put("notesConfigured", sConfiguredNotes);

	boost::property_tree::write_json(configName, pt);
}

CIMSCN83::~CIMSCN83()
{
	this->mpost->changed.disconnect_all_slots();
	delete mpost;
}
