#include "stdafx.h"
#include "IDCCRT310.h"
#include "XFSIDC.H"
#include "CRT_310.h"
#include "autoPort.h"

using namespace boost::filesystem;
using namespace boost::property_tree;

//EVENTS
void IDCCRT310::sendWFS_EXEE_IDC_INVALIDTRACKDATA(HSERVICE hService, WORD fwStatus, std::string track, std::string data)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	   
		for (auto i : events)
		{		
			LPWFSRESULT pResult;
			memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
			pResult->hService = hService;
			pResult->hResult = 0;
			pResult->u.dwEventID = WFS_EXEE_IDC_INVALIDTRACKDATA;

			LPWFSIDCTRACKEVENT trackEvent;
			memAlloc->AllocateMore(sizeof(WFSIDCTRACKEVENT), pResult,(void **)&trackEvent);

			trackEvent->fwStatus = fwStatus;
			
			memAlloc->AllocateMore(data.length()+1, pResult, (void **)&trackEvent->lpstrData);
			memAlloc->AllocateMore(track.length() + 1, pResult, (void **)&trackEvent->lpstrTrack);
			
			memcpy_s(trackEvent->lpstrData, data.length() - 1, data.c_str(), data.length() - 1);
			trackEvent->lpstrData[data.length() - 1] = '\0';

			memcpy_s(trackEvent->lpstrTrack, track.length() - 1,track.c_str(), track.length() - 1);
			trackEvent->lpstrTrack[track.length() - 1] = '\0';


			pResult->lpBuffer = trackEvent;
			sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
		}
	
}

void IDCCRT310::sendWFS_EXEE_IDC_MEDIAINSERTED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_IDC_MEDIAINSERTED;	
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_MEDIAREMOVED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_MEDIAREMOVED;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_EXEE_IDC_MEDIARETAINED(HSERVICE hService) 
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_IDC_MEDIARETAINED;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_EXEE_IDC_INVALIDMEDIA(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_IDC_INVALIDMEDIA;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_CARDACTION(HSERVICE hService,WORD wAction,WORD wPosition)
{

	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_CARDACTION;

		LPWFSIDCCARDACT lpCardAct;
		memAlloc->AllocateMore(sizeof(WFSIDCCARDACT), pResult, (void **)&lpCardAct);

		lpCardAct->wAction = wAction;
		lpCardAct->wPosition = wPosition;
			   
		pResult->lpBuffer = lpCardAct;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_USRE_IDC_RETAINBINTHRESHOLD(HSERVICE hService, WORD fwRetainBin)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_USER_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_USRE_IDC_RETAINBINTHRESHOLD;

		LPWORD lpfwRetainBin;
		memAlloc->AllocateMore(sizeof(WORD), pResult, (void **)&lpfwRetainBin);
		*lpfwRetainBin = fwRetainBin;
		pResult->lpBuffer = lpfwRetainBin;
		sendMessage(i.first, WFS_USER_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_MEDIADETECTED(HSERVICE hService, WORD wResetOut) 
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_MEDIADETECTED;

		LPWORD lpwResetOut;
		memAlloc->AllocateMore(sizeof(WORD), pResult, (void **)&lpwResetOut);
		*lpwResetOut = wResetOut;
		pResult->lpBuffer = lpwResetOut;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_RETAINBINREMOVED(HSERVICE hService) 
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_RETAINBINREMOVED;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_RETAINBININSERTED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_RETAINBININSERTED;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_EXEE_IDC_INSERTCARD(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_IDC_INSERTCARD;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_DEVICEPOSITION(HSERVICE hService, WORD wPosition)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_DEVICEPOSITION;

		LPWFSIDCDEVICEPOSITION lpDevicePosition;
		memAlloc->AllocateMore(sizeof(WFSIDCDEVICEPOSITION), pResult, (void **)&lpDevicePosition);
		lpDevicePosition->wPosition = wPosition;
		pResult->lpBuffer = lpDevicePosition;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_SRVE_IDC_POWER_SAVE_CHANGE(HSERVICE hService, USHORT usPowerSaveRecoveryTime)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_IDC_POWER_SAVE_CHANGE;

		LPWFSIDCPOWERSAVECHANGE lpPowerSaveChange;
		memAlloc->AllocateMore(sizeof(WFSIDCPOWERSAVECHANGE), pResult, (void **)&lpPowerSaveChange);
		lpPowerSaveChange->usPowerSaveRecoveryTime = usPowerSaveRecoveryTime;
		pResult->lpBuffer = lpPowerSaveChange;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_EXEE_IDC_TRACKDETECTED(HSERVICE hService, WORD fwTracks)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_EXEE_IDC_TRACKDETECTED;

		LPWFSIDCTRACKDETECTED lpTrackDetected;
		memAlloc->AllocateMore(sizeof(WFSIDCTRACKDETECTED), pResult, (void **)&lpTrackDetected);
		lpTrackDetected->fwTracks = fwTracks;
		pResult->lpBuffer = lpTrackDetected;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}

void IDCCRT310::sendWFS_EXEE_IDC_EMVCLESSREADSTATUS() {
	throw std::logic_error{ "Function not yet implemented." };
}


void IDCCRT310::copyLpInput(LPVOID lpData, REQUESTID reqId)
{
	
	if (requests[reqId]->messageType == MSG_EXECUTE && lpData)
	{
		if (requests[reqId]->cmd == WFS_CMD_IDC_EJECT_CARD)
		{
			LPWFSIDCEJECTCARD lpEjectCards = (LPWFSIDCEJECTCARD) lpData ;
			memAlloc->AllocateBuffer(sizeof(WFSIDCEJECTCARD), (void **)&requests[reqId]->lpParam);
			LPWFSIDCEJECTCARD lpEjectCardd = (LPWFSIDCEJECTCARD)requests[reqId]->lpParam;
			lpEjectCardd->wEjectPosition = lpEjectCards->wEjectPosition;
		}
		if (requests[reqId]->cmd == WFS_CMD_IDC_READ_RAW_DATA)
		{
			Xfs::getInstance()->l.debug_dev("copyLpInput WFS_CMD_IDC_READ_RAW_DATA");
			LPWORD lpwReadDatas = (LPWORD)lpData;
					
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpwReadDatad = (LPWORD)requests[reqId]->lpParam;
			*lpwReadDatad = *lpwReadDatas;
			Xfs::getInstance()->l.debug_dev("LPWORD created");

		}
		if (requests[reqId]->cmd == WFS_CMD_IDC_CHIP_IO)
		{
			Xfs::getInstance()->l.debug_dev("copyLpInput WFS_CMD_IDC_CHIP_IO");

			LPWFSIDCCHIPIO lpChipIoIns = (LPWFSIDCCHIPIO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSIDCCHIPIO), (void **)&requests[reqId]->lpParam);
			LPWFSIDCCHIPIO lpChipIoInd = (LPWFSIDCCHIPIO)requests[reqId]->lpParam;

			memAlloc->AllocateMore(lpChipIoIns->ulChipDataLength, requests[reqId]->lpParam,(void **) &lpChipIoInd->lpbChipData);
			lpChipIoInd->ulChipDataLength = lpChipIoIns->ulChipDataLength;
			lpChipIoInd->wChipProtocol = lpChipIoIns->wChipProtocol;
			memcpy_s(lpChipIoInd->lpbChipData, lpChipIoInd->ulChipDataLength, lpChipIoIns->lpbChipData, lpChipIoIns->ulChipDataLength);			
			Xfs::getInstance()->l.debug_dev("LPWFSIDCCHIPIO created");

		}
		if (requests[reqId]->cmd == WFS_CMD_IDC_RESET)
		{
			LPWORD lpwResetIns = (LPWORD)lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpwResetInd = (LPWORD)requests[reqId]->lpParam;
			*lpwResetInd = *lpwResetIns;
		}
		if (requests[reqId]->cmd == WFS_CMD_IDC_CHIP_POWER)
		{			
			LPWORD lpwChipPowers = (LPWORD)lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpwChipPowerd = (LPWORD)requests[reqId]->lpParam;
			*lpwChipPowerd = *lpwChipPowers;
		}
		if (requests[reqId]->cmd == WFS_CMD_IDC_SET_GUIDANCE_LIGHT)
		{
			LPWFSIDCSETGUIDLIGHT lpSetGuidLights = (LPWFSIDCSETGUIDLIGHT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSIDCSETGUIDLIGHT), (void **)&requests[reqId]->lpParam);
			LPWFSIDCSETGUIDLIGHT lpSetGuidLightd = (LPWFSIDCSETGUIDLIGHT)requests[reqId]->lpParam;
			lpSetGuidLightd->dwCommand = lpSetGuidLights->dwCommand;
			lpSetGuidLightd->wGuidLight = lpSetGuidLights->wGuidLight;
		}
	}
}


IDCCRT310::IDCCRT310(volatile SharedData* s,  unsigned short version, std::string port, int baudRate, std::string lname):GenericDevice(s,version)
{
	this->port = port;
	this->baudRate = baudRate;
	this->configName = lname;

	hResult = WFS_SUCCESS;
	//Setting reader  capabilities
	this->caps.wClass = WFS_SERVICE_CLASS_IDC;
	this->caps.fwType = WFS_IDC_TYPEMOTOR;
	this->caps.bCompound = 0;
	this->caps.fwReadTracks = WFS_IDC_TRACK1| WFS_IDC_TRACK2;
	this->caps.fwWriteTracks = WFS_IDC_NOTSUPP;
	this->caps.fwChipProtocols = WFS_IDC_CHIPT0| WFS_IDC_CHIPT1;
	this->caps.usCards = 10; 
	this->caps.fwSecType = WFS_IDC_SECNOTSUPP;
	this->caps.fwPowerOnOption = WFS_IDC_NOACTION;
	this->caps.fwPowerOffOption = WFS_IDC_NOACTION;
	this->caps.bFluxSensorProgrammable = 0;
	this->caps.bReadWriteAccessFollowingEject = 0;
	this->caps.fwWriteMode = WFS_IDC_NOTSUPP;
	this->caps.fwChipPower = WFS_IDC_CHIPPOWERCOLD| WFS_IDC_CHIPPOWERWARM;
	this->caps.lpszExtra = 0;
	this->caps.fwDIPMode = WFS_IDC_NOTSUPP;
	this->caps.lpwMemoryChipProtocols = 0;
	for (int i = 0; i < WFS_IDC_GUIDLIGHTS_SIZE; i++)
	{
		this->caps.dwGuidLights[i] = WFS_IDC_GUIDANCE_NOT_AVAILABLE;		
	}
	this->caps.dwGuidLights[0] = WFS_IDC_GUIDANCE_CONTINUOUS | WFS_IDC_GUIDANCE_GREEN;
	

		
		
		
		
	
	
	this->caps.fwEjectPosition = WFS_IDC_EXITPOSITION;
	this->caps.bPowerSaveControl = 0;
	this->caps.usParkingStations = 0;
	this->caps.bAntiFraudModule = 0;
	this->caps.lpdwSynchronizableCommands = 0;
	loadCapabilities();
	
	
	this->status.fwDevice = WFS_IDC_DEVOFFLINE;
	this->status.fwMedia = WFS_IDC_MEDIAUNKNOWN;
	this->status.fwRetainBin = WFS_IDC_RETAINBINOK;
	this->status.fwSecurity = WFS_IDC_SECNOTSUPP;
	this->status.usCards = 0;
	loadRetainedCount();
	this->status.fwChipPower = WFS_IDC_CHIPUNKNOWN;
	this->status.lpszExtra = 0;
	
	for (int i = 0; i < WFS_IDC_GUIDLIGHTS_SIZE; i++)
	{
		this->status.dwGuidLights[i] = WFS_IDC_GUIDANCE_NOT_AVAILABLE;
	}
	


	this->status.fwChipModule = WFS_IDC_CHIPMODUNKNOWN;
	this->status.fwMagReadModule = WFS_IDC_MAGMODUNKNOWN;
	this->status.fwMagWriteModule = WFS_IDC_MAGMODNOTSUPP;
	this->status.fwFrontImageModule = WFS_IDC_IMGMODNOTSUPP;
	this->status.fwBackImageModule = WFS_IDC_IMGMODNOTSUPP;
	this->status.wDevicePosition = WFS_IDC_DEVICEPOSNOTSUPP;
	this->status.usPowerSaveRecoveryTime = 0;
	this->status.lpwParkingStationMedia = 0;
	this->status.wAntiFraudModule = WFS_IDC_AFMNOTSUPP;

	//setting up status
	AutoPort aport(port, baudRate);
	updateCardStatus(aport.port);


}

IDCCRT310::~IDCCRT310()
{
}

void IDCCRT310::getInfoProc(REQUESTID reqId)
{
	if (requests[reqId]->cmd == WFS_INF_IDC_STATUS)
	{		
		auto a = this->memAlloc->AllocateMore(sizeof(WFSIDCSTATUS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);

		LPWFSIDCSTATUS s = (LPWFSIDCSTATUS)requests[reqId]->pResult->lpBuffer;
		s->fwDevice = status.fwDevice;
		s->fwMedia = status.fwMedia;
		s->fwRetainBin = status.fwRetainBin;
		s->fwSecurity = status.fwSecurity;

		loadRetainedCount();
		s->usCards = status.usCards;
		
		
		s->fwChipPower = status.fwChipPower;
		s->lpszExtra = 0;
		for (int i = 0; i < WFS_IDC_GUIDLIGHTS_SIZE; i++)
			s->dwGuidLights[i] = status.dwGuidLights[i];
		
		s->fwChipModule = status.fwChipModule;
		s->fwMagReadModule = status.fwMagReadModule;
		s->fwMagWriteModule = status.fwMagWriteModule;
		s->fwFrontImageModule = status.fwFrontImageModule;
		s->fwBackImageModule = status.fwBackImageModule;
		s->wDevicePosition = status.wDevicePosition;
		s->usPowerSaveRecoveryTime = status.usPowerSaveRecoveryTime;
		s->lpwParkingStationMedia = 0;
		s->wAntiFraudModule =status.wAntiFraudModule;
	}
	else
	if (requests[reqId]->cmd == WFS_INF_IDC_CAPABILITIES)
	{

		auto a = this->memAlloc->AllocateMore(sizeof(WFSIDCCAPS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);


		LPWFSIDCCAPS cpb = (LPWFSIDCCAPS)requests[reqId]->pResult->lpBuffer;


		cpb->wClass = caps.wClass;// WFS_SERVICE_CLASS_IDC;

		cpb->fwType = caps.fwType;// WFS_IDC_TYPEMOTOR;

		cpb->bCompound = caps.bCompound;// 0;

		cpb->fwReadTracks = caps.fwReadTracks;//WFS_IDC_TRACK1 | WFS_IDC_TRACK2;
		
		cpb->fwWriteTracks = caps.fwWriteTracks;// WFS_IDC_NOTSUPP;
		
		cpb->fwChipProtocols = caps.fwChipProtocols;// WFS_IDC_CHIPT0 | WFS_IDC_CHIPT1;
				   		 
		cpb->usCards = caps.usCards;// 10;
					   		 
		cpb->fwSecType = caps.fwSecType;// WFS_IDC_SECNOTSUPP;
				
		cpb->fwPowerOnOption = caps.fwPowerOnOption;// WFS_IDC_NOACTION;
		
		cpb->fwPowerOffOption = caps.fwPowerOffOption;/// WFS_IDC_NOACTION;
				   

		cpb->bFluxSensorProgrammable = caps.bFluxSensorProgrammable;// 0;
				
		cpb->bReadWriteAccessFollowingEject = caps.bReadWriteAccessFollowingEject;// 0;
					   
		cpb->fwWriteMode = caps.fwWriteMode;// WFS_IDC_NOTSUPP;
		
		cpb->fwChipPower = caps.fwChipPower;// WFS_IDC_CHIPPOWERCOLD | WFS_IDC_CHIPPOWERWARM;

		cpb->lpszExtra = caps.lpszExtra;		
		
		cpb->fwDIPMode = caps.fwDIPMode;// WFS_IDC_NOTSUPP;
		
		cpb->lpwMemoryChipProtocols = caps.lpwMemoryChipProtocols;// 0;
		

		for (int i = 0; i < WFS_IDC_GUIDLIGHTS_SIZE; i++)
			cpb->dwGuidLights[i] = caps.dwGuidLights[i];
		
		cpb->dwGuidLights[0] = WFS_IDC_GUIDANCE_CONTINUOUS| WFS_IDC_GUIDANCE_GREEN;
			//| WFS_IDC_GUIDANCE_QUICK_FLASH | WFS_IDC_GUIDANCE_GREEN | WFS_IDC_GUIDANCE_ENTRY | WFS_IDC_GUIDANCE_EXIT;


		
		cpb->fwEjectPosition = caps.fwEjectPosition;// WFS_IDC_EXITPOSITION;
		
		cpb->bPowerSaveControl = caps.bPowerSaveControl;// 0;
		
		cpb->usParkingStations = caps.usParkingStations;// 0;
		
		cpb->bAntiFraudModule = caps.bAntiFraudModule;// 0;
		
		cpb->lpdwSynchronizableCommands = caps.lpdwSynchronizableCommands;// 0;

		
		return;
	}
	else
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
}

void IDCCRT310::executeProc(REQUESTID reqId)
{	
	if (requests[reqId]->cmd == WFS_CMD_IDC_READ_TRACK) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_WRITE_TRACK) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_EJECT_CARD) 
	{
		execWFS_CMD_IDC_EJECT_CARD(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_RETAIN_CARD) 
	{
		execWFS_CMD_IDC_RETAIN_CARD(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_RESET_COUNT) 
	{
		execWFS_CMD_IDC_RESET_COUNT(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_SETKEY) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_READ_RAW_DATA) 
	{
		execWFS_CMD_IDC_READ_RAW_DATA(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_WRITE_RAW_DATA) {}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_CHIP_IO) 
	{
		execWFS_CMD_IDC_CHIP_IO(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_RESET) 
	{
		execWFS_CMD_IDC_RESET(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_CHIP_POWER) 
	{
		execWFS_CMD_IDC_CHIP_POWER(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_PARSE_DATA) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_SET_GUIDANCE_LIGHT) 
	{
		execWFS_CMD_IDC_SET_GUIDANCE_LIGHT(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_POWER_SAVE_CONTROL) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_PARK_CARD) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_EMVCLESS_CONFIGURE) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_EMVCLESS_PERFORM_TRANSACTION) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_EMVCLESS_ISSUERUPDATE) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_IDC_SYNCHRONIZE_COMMAND) 
	{
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
	else
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
}

//EXECUTE
void IDCCRT310::execWFS_CMD_IDC_READ_RAW_DATA(REQUESTID reqId)
{
	LPWORD lpwReadData = (LPWORD)requests[reqId]->lpParam;
	
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_READ_RAW_DATA reqId=" + std::to_string(reqId) + ", lpwReadData=" + std::to_string(*lpwReadData));
	
	
	Xfs::getInstance()->l.debug_dev("(*lpwReadData)&WFS_IDC_TRACK1" + std::to_string((*lpwReadData)&WFS_IDC_TRACK1));
	Xfs::getInstance()->l.debug_dev("(*lpwReadData)&WFS_IDC_TRACK2" + std::to_string((*lpwReadData)&WFS_IDC_TRACK2));


	AutoPort port(this->port, this->baudRate);

	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	

	updateCardStatus(port.port);

	if (status.fwDevice != WFS_IDC_DEVONLINE)
	{
		Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));

		if (status.fwDevice == WFS_IDC_DEVBUSY)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		}
		else
		{
			requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		}
		return;
	}

	if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
	{
		Xfs::getInstance()->l.debug_dev("Media jammed, ...");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		return;
	}


	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		Xfs::getInstance()->l.debug_dev("Media not present, enabling card-in");

		int ec = CRT310_CardSetting(port.port, 0x4, 0x1);
		if (ec != 0)
		{
			GetErrCode(&ec);
			checkEc(ec);
			requests[reqId]->pResult->hResult = this->hResult;
			return;
		}

		sendWFS_EXEE_IDC_INSERTCARD(requests[reqId]->hservice);

		while (1)
		{
			updateCardStatus(port.port);
			if (status.fwMedia == WFS_IDC_MEDIAPRESENT)break;
			if (requests[reqId]->isCancelled)break;
			Sleep(10);
		}
	}



	if (status.fwMedia != WFS_IDC_MEDIAPRESENT)
	if (requests[reqId]->isCancelled)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CANCELED;
		Xfs::getInstance()->l.debug_dev("cancelled");


		Xfs::getInstance()->l.debug_dev("Disabling card-in");

		int ec = CRT310_CardSetting(port.port, 0x1, 0x1);
		if (ec != 0)
		{
			GetErrCode(&ec);
			Xfs::getInstance()->l.debug_dev("Error disabling card-in:" + std::to_string(ec));

			checkEc(ec);
			requests[reqId]->pResult->hResult = this->hResult;
		}
		updateCardStatus(port.port);


		return;
	}

	sendWFS_EXEE_IDC_MEDIAINSERTED(requests[reqId]->hservice);


	if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
	{
		Xfs::getInstance()->l.debug_dev("Media jammed, ...");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		return;
	}


	int sz = 1;
	BYTE track = 0;
	if (((*lpwReadData)&WFS_IDC_TRACK1) && ((*lpwReadData)&WFS_IDC_TRACK2) && ((*lpwReadData)&WFS_IDC_TRACK3))
	{
		track = 0x37;
		sz = 3;
	}
	else
	if (((*lpwReadData)&WFS_IDC_TRACK1) && ((*lpwReadData)&WFS_IDC_TRACK2))
	{
		track = 0x34;
		sz = 2;
	}
	else
	if (((*lpwReadData)&WFS_IDC_TRACK1) && ((*lpwReadData)&WFS_IDC_TRACK3))
	{
		track = 0x36;
		sz = 2;
	}
	else
	if (((*lpwReadData)&WFS_IDC_TRACK2) && ((*lpwReadData)&WFS_IDC_TRACK3))
	{
		track = 0x35;
		sz = 2;
	}
	else	
	if ((*lpwReadData)&WFS_IDC_TRACK1)
	{
		track = 0x31;
	}
	else
	if ((*lpwReadData)&WFS_IDC_TRACK2)
	{
		track = 0x32;
	}
	else
	if ((*lpwReadData)&WFS_IDC_TRACK3)
	{
		track = 0x33;
	}
	
	int len = 0;
	unsigned char data[1024];
	int len1 = 0;
	unsigned char data1[255];
	int len2 = 0;
	unsigned char data2[255];
	int len3 = 0;
	unsigned char data3[255];
	Xfs::getInstance()->l.debug_dev("MC_ReadTracks track cmd=" + std::to_string(track));


	if (track)
	{
		
		int ec = MC_ReadTrack(port.port, 0x30, track, &len, data);

		Xfs::getInstance()->l.debug_dev("MC_ReadTracks ec=" + std::to_string(ec));
		if (ec != 0)
		{
			GetErrCode(&ec);
			Xfs::getInstance()->l.debug_dev("Error reading tracks:" + std::to_string(ec));
			checkEc(ec);
			requests[reqId]->pResult->hResult = this->hResult;
			return;
		}
		Xfs::getInstance()->l.debug_dev("Read tracks =", data, len);




		unsigned char *p = data;
		
		do
		{
			data1[len1] = *p;
			len1++;
			p++;
		} while (*p != 0x1f);
		Xfs::getInstance()->l.debug_dev("Track 1 length=" + std::to_string(len1));
		
			
		do
		{
		    data2[len2] = *p;
		    len2++;
			p++;
		} while (*p != 0x1f);
		Xfs::getInstance()->l.debug_dev("Track 2 length=" + std::to_string(len2));
		
		
		len3 = len - len1 - len2;
		for (int i = 0; i < len3; i++, p++)		
			data3[i] = *p;
		
		Xfs::getInstance()->l.debug_dev("Track 3 length=" + std::to_string(len3));


	}

	int lenAtr = 0;
	unsigned char dataAtr[255];
	short wStatusAtr = WFS_IDC_DATAOK;
	if ((*lpwReadData)&WFS_IDC_CHIP)
	{
		sz++;

		auto ec = CRT310_MovePosition(port.port, 0x04);		
		if(!ec)
		{ 
			ec = CPU_WarmReset(port.port, &cpuType, dataAtr, &lenAtr);
			Xfs::getInstance()->l.debug_dev("Warm reset =" + std::to_string(ec));
			Xfs::getInstance()->l.debug_dev("CPU_WarmReset ec=" + std::to_string(ec));
			Xfs::getInstance()->l.debug_dev("CPU_WarmReset", dataAtr, lenAtr);

			if (ec)
			{
				if (ec == 0x4e)//Unsuccessful
				{
					Xfs::getInstance()->l.debug_dev("Unsuccessful");
					wStatusAtr = WFS_IDC_DATASRCMISSING;

				}
				if (ec == 0x45) //No card in the reader
				{
					Xfs::getInstance()->l.debug_dev("No card in the reader");
					wStatusAtr = WFS_IDC_DATASRCMISSING;
				}
				if (ec == 0x57) //Card is no on the right position
				{
					Xfs::getInstance()->l.debug_dev("Card is no on the right position");
					wStatusAtr = WFS_IDC_DATASRCMISSING;

				}
			}
			if (!lenAtr)
			{
				Xfs::getInstance()->l.debug_dev("Data Length=0");
				wStatusAtr = WFS_IDC_DATAMISSING;
			}
		}
		else
		{
			Xfs::getInstance()->l.debug_dev("Unsuccessful");
			wStatusAtr = WFS_IDC_DATASRCMISSING;
		}		
	}
	
	LPWFSIDCCARDDATA *lppCardData;
	if (sz)
	{
		memAlloc->AllocateMore(sizeof(LPWFSIDCCARDDATA)*(sz+1), requests[reqId]->pResult, (void **)&lppCardData);
		for(int  i =0;i<sz;i++)
		{
			memAlloc->AllocateMore(sizeof(WFSIDCCARDDATA), requests[reqId]->pResult, (void **)&lppCardData[i]);
		}
		lppCardData[sz] = 0;
	}

	requests[reqId]->pResult->lpBuffer = lppCardData;

	int c = 0;
	if ((*lpwReadData)&WFS_IDC_TRACK1)
	{
		Xfs::getInstance()->l.debug_dev("Track 1 data",data1,len1);

		lppCardData[c]->ulDataLength = 0;
		
		if (data1[1] == 0X59) {// read data correclty
			lppCardData[c]->wStatus = WFS_IDC_DATAOK;
			lppCardData[c]->ulDataLength = len1 - 2;
			Xfs::getInstance()->l.debug_dev("Track 1 data read correctly");

		} 
		else
		if (data1[1] == 0X4E)  // read data incorreclty
		{
			lppCardData[c]->wStatus = WFS_IDC_DATAINVALID;
		
		}
		else
			if (data1[1] == 0X4F) {// Inavailabe to ready, magnetic data is 0xE0
				lppCardData[c]->wStatus = WFS_IDC_DATAMISSING;
			} 
				
		lppCardData[c]->wDataSource = WFS_IDC_TRACK1;
		if(lppCardData[c]->ulDataLength)
		{
			memAlloc->AllocateMore(lppCardData[c]->ulDataLength, requests[reqId]->pResult, (void **)&lppCardData[c]->lpbData);
			memcpy_s(lppCardData[c]->lpbData, lppCardData[c]->ulDataLength, &data1[2], lppCardData[c]->ulDataLength);
		}
		c++;
	}
	if ((*lpwReadData)&WFS_IDC_TRACK2)
	{
		Xfs::getInstance()->l.debug_dev("Track 2 data", data2, len2);
		lppCardData[c]->ulDataLength = 0;
		lppCardData[c]->wDataSource = WFS_IDC_TRACK2;
		
		if (data2[1] == 0X59) {// read data correclty
			lppCardData[c]->wStatus = WFS_IDC_DATAOK;
			lppCardData[c]->ulDataLength = len2 - 2;
			Xfs::getInstance()->l.debug_dev("Track 2 data read correctly");

		}
		else
			if (data2[1] == 0X4E)  // read data incorreclty
			{
				lppCardData[c]->wStatus = WFS_IDC_DATAINVALID;

			}
			else
				if (data2[1] == 0X4F) {// Inavailabe to ready, magnetic data is 0xE0
					lppCardData[c]->wStatus = WFS_IDC_DATAMISSING;
				}
		lppCardData[c]->wDataSource = WFS_IDC_TRACK2;

		if (lppCardData[c]->ulDataLength)
		{
			memAlloc->AllocateMore(lppCardData[c]->ulDataLength, requests[reqId]->pResult, (void **)&lppCardData[c]->lpbData);
			memcpy_s(lppCardData[c]->lpbData, lppCardData[c]->ulDataLength, &data2[2], lppCardData[c]->ulDataLength);
		}
		
		c++;
	}

	
	if ((*lpwReadData)&WFS_IDC_TRACK3)
	{
		Xfs::getInstance()->l.debug_dev("Track 3 data", data3, len3);
		lppCardData[c]->ulDataLength = 0;
		lppCardData[c]->wDataSource = WFS_IDC_TRACK3;

		if (data3[1] == 0X59) {// read data correclty
			lppCardData[c]->wStatus = WFS_IDC_DATAOK;
			lppCardData[c]->ulDataLength = len3 - 2;
			Xfs::getInstance()->l.debug_dev("Track 3 data read correctly");

		}
		else
			if (data3[1] == 0X4E)  // read data incorreclty
			{
				lppCardData[c]->wStatus = WFS_IDC_DATAINVALID;

			}
			else
				if (data3[1] == 0X4F) {// Inavailabe to ready, magnetic data is 0xE0
					lppCardData[c]->wStatus = WFS_IDC_DATAMISSING;
				}
		lppCardData[c]->wDataSource = WFS_IDC_TRACK3;

		if (lppCardData[c]->ulDataLength)
		{
			memAlloc->AllocateMore(lppCardData[c]->ulDataLength, requests[reqId]->pResult, (void **)&lppCardData[c]->lpbData);
			memcpy_s(lppCardData[c]->lpbData, lppCardData[c]->ulDataLength, &data3[2], lppCardData[c]->ulDataLength);
		}

		c++;
	}


	if((*lpwReadData)&WFS_IDC_CHIP)
	{
		lppCardData[c]->ulDataLength = 0;
		lppCardData[c]->wDataSource = WFS_IDC_CHIP;

		lppCardData[c]->wStatus = wStatusAtr;
		lppCardData[c]->ulDataLength = lenAtr;
		if (wStatusAtr == WFS_IDC_DATAOK)
		{
			
			memAlloc->AllocateMore(lenAtr, requests[reqId]->pResult, (void **)&lppCardData[c]->lpbData);
			memcpy_s(lppCardData[c]->lpbData,lenAtr, dataAtr, lenAtr);
		}
		c++;
	}



	

	updateCardStatus(port.port);

	requests[reqId]->pResult->hResult = WFS_SUCCESS;

}

void IDCCRT310::execWFS_CMD_IDC_CHIP_POWER(REQUESTID reqId)
{	
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_CHIP_POWER reqId=" + std::to_string(reqId));

	LPWORD lpwChipPower;

	lpwChipPower = (LPWORD)requests[reqId]->lpParam;

	
	AutoPort port(this->port, this->baudRate);
	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	updateCardStatus(port.port);
		
	if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		Xfs::getInstance()->l.debug_dev("Unsuccessful = WFS_ERR_IDC_MEDIAJAM");
		return;
	}
	if (status.fwMedia != WFS_IDC_MEDIAPRESENT)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;	
		Xfs::getInstance()->l.debug_dev("Unsuccessful = WFS_ERR_IDC_NOMEDIA");

		return;
	}
	//from this point  assume that we can reset chip


	BYTE data[255];
	
	int len = 0;
	int ec = 0;

	if (*lpwChipPower == WFS_IDC_CHIPPOWERCOLD) 
	{
		ec = CPU_ColdReset(port.port, 0, &cpuType,data, &len);
	}
	if (*lpwChipPower == WFS_IDC_CHIPPOWERWARM)
	{
		ec = CPU_WarmReset(port.port, &cpuType, data, &len);
	}
	
	if (ec)
	{
		Xfs::getInstance()->l.debug_dev("Error happened ec=" + std::to_string(ec));


		if (ec == 0x4e)//Unsuccessful
		{
			Xfs::getInstance()->l.debug_dev("Unsuccessful");
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_ATRNOTOBTAINED;

		}
		if (ec == 0x45) //No card in the reader
		{
			Xfs::getInstance()->l.debug_dev("No card in the reader");			
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
		}
		if (ec == 0x57) //Card is no on the right position
		{
			Xfs::getInstance()->l.debug_dev("Card is no on the right position");
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
			
			return;
		}
	}
	if (!len)
	{		
		Xfs::getInstance()->l.debug_dev("Data Length=0");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_INVALIDDATA;
		return;
	}

	Xfs::getInstance()->l.debug_dev("Looks like everything is ok, creatinf output result");


	LPWFSIDCCHIPPOWEROUT lpChipPowerOut;
	memAlloc->AllocateMore(sizeof(WFSIDCCHIPPOWEROUT), requests[reqId]->pResult, (void **)&lpChipPowerOut);
	requests[reqId]->pResult->lpBuffer = lpChipPowerOut;
	lpChipPowerOut->ulChipDataLength = len;
	memAlloc->AllocateMore(len, requests[reqId]->pResult, (void **)&lpChipPowerOut->lpbChipData);
	memcpy_s(lpChipPowerOut->lpbChipData,len, data, len);

	requests[reqId]->pResult->hResult = WFS_SUCCESS;

	return;



}

void IDCCRT310::execWFS_CMD_IDC_CHIP_IO(REQUESTID reqId)
{
	LPWFSIDCCHIPIO lpChipIoIn;
	

	lpChipIoIn = (LPWFSIDCCHIPIO)requests[reqId]->lpParam;
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_CHIP_IO reqId=" + std::to_string(reqId));

	Xfs::getInstance()->l.debug_dev("lpChipIoIn", lpChipIoIn->lpbChipData, lpChipIoIn->ulChipDataLength);

	AutoPort port(this->port, this->baudRate);

	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}
	   
	updateCardStatus(port.port);

	if (status.fwDevice != WFS_IDC_DEVONLINE)
	{
		Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));

		if (status.fwDevice == WFS_IDC_DEVBUSY)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		}
		else
		{
			requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		}
		return;
	}

	if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
	{
		Xfs::getInstance()->l.debug_dev("Media jammed, ...");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		return;
	}
	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		Xfs::getInstance()->l.debug_dev("Media not present, ...");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
		return;
	}

	int len;
	BYTE data[512];
	int ec;
	if (lpChipIoIn->wChipProtocol == WFS_IDC_CHIPT0)
	{
		ec = CPU_T0_C_APDU(port.port, lpChipIoIn->ulChipDataLength, lpChipIoIn->lpbChipData, data, &len);
	}
	else
	if (lpChipIoIn->wChipProtocol == WFS_IDC_CHIPT1)
	{
		ec = CPU_T1_C_APDU(port.port, lpChipIoIn->ulChipDataLength, lpChipIoIn->lpbChipData, data, &len);
	}
	if (ec)
	{
		Xfs::getInstance()->l.debug_dev("CAPU  sending returns an ERROR="+std::to_string(ec));
		if (ec == 0x4e){//Failure
			
			Xfs::getInstance()->l.debug_dev("Failure");
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_INVALIDDATA;			
			return;
		} 
		if (ec == 0x45){// No card in the reader
			Xfs::getInstance()->l.debug_dev("No card in the reader");
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
			return;
		} 
		if (ec == 0x57){// Card on un-operative position
			Xfs::getInstance()->l.debug_dev("Card on un-operative position");

			requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
			return;
		} 
	}
	if (!len)
	{
		Xfs::getInstance()->l.debug_dev("len=0, obviously something went wrong");

		requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
		return;
	}

	LPWFSIDCCHIPIO lpChipIoOut;

	memAlloc->AllocateMore(sizeof(WFSIDCCHIPIO), requests[reqId]->pResult, (void **)&lpChipIoOut);
	requests[reqId]->pResult->lpBuffer = lpChipIoOut;
	memAlloc->AllocateMore(len, requests[reqId]->pResult, (void **)&lpChipIoOut->lpbChipData);
	lpChipIoOut->ulChipDataLength = len;
	lpChipIoOut->wChipProtocol = lpChipIoIn->wChipProtocol;
	memcpy_s(lpChipIoOut->lpbChipData, lpChipIoOut->ulChipDataLength, data, len);
	requests[reqId]->pResult->hResult = WFS_SUCCESS;

	Xfs::getInstance()->l.debug_dev("lpChipIoOut", lpChipIoOut->lpbChipData, lpChipIoOut->ulChipDataLength);


	return;


}

void IDCCRT310::execWFS_CMD_IDC_RETAIN_CARD(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_RETAIN_CARD reqId=" + std::to_string(reqId) );

	LPWFSIDCRETAINCARD lpRetainCard;
	memAlloc->AllocateMore(sizeof(WFSIDCRETAINCARD), requests[reqId]->pResult, (void **)&lpRetainCard);
	requests[reqId]->pResult->lpBuffer = lpRetainCard;
	lpRetainCard->fwPosition = WFS_IDC_MEDIAUNKNOWN;
	AutoPort port(this->port, this->baudRate);

	if (!port.port)	
	{		
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	updateCardStatus(port.port);

	lpRetainCard->usCount = status.usCards;
	if(status.fwMedia == WFS_IDC_MEDIAPRESENT || status.fwMedia == WFS_IDC_MEDIAENTERING)
		lpRetainCard->fwPosition = status.fwMedia;

	if (status.fwDevice != WFS_IDC_DEVONLINE)
	{
		Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));
		if (status.fwDevice == WFS_IDC_DEVBUSY)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		}
		else
		{
			requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		}
		return;
	}
	

	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		Xfs::getInstance()->l.debug_dev("Media not present, nothing to retain");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;			
		return;
	}
	else
	{
		if (status.usCards >= caps.usCards)
		{
			sendWFS_USRE_IDC_RETAINBINTHRESHOLD(requests[reqId]->hservice, WFS_IDC_RETAINBINFULL);
			Xfs::getInstance()->l.debug_dev("Mediapresent, but retain BIN is full");
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_RETAINBINFULL;
			return;
		}

		//0x06  = rear position without hold
		int ec = CRT310_MovePosition(port.port, 0x06);
		if (ec != 0)
		{
			GetErrCode(&ec);
			checkEc(ec);
			requests[reqId]->pResult->hResult = this->hResult;
			return;
		}	
	}

	//sendWFS_EXEE_IDC_INSERTCARD(requests[reqId]->hservice);
	while (1)
	{
		updateCardStatus(port.port);		

		if (status.fwMedia != WFS_IDC_MEDIAPRESENT)
			break;		
		Sleep(10);
	}
	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		requests[reqId]->pResult->hResult = WFS_SUCCESS;		
	}
	else
		if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;			
		}

	if (requests[reqId]->pResult->hResult == WFS_SUCCESS)
	{
		sendWFS_EXEE_IDC_MEDIARETAINED(requests[reqId]->hservice);
		status.usCards++;
		lpRetainCard->usCount = status.usCards;
		saveRetainedCount();
		if (status.usCards >= this->retainBinTreshold)
		{
			sendWFS_USRE_IDC_RETAINBINTHRESHOLD(requests[reqId]->hservice, WFS_IDC_RETAINBINHIGH);
		}
	}

}

void IDCCRT310::execWFS_CMD_IDC_EJECT_CARD(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_EJECT_CARD reqId=" + std::to_string(reqId));

	LPWFSIDCEJECTCARD lpEjectCard;
	
	lpEjectCard = (LPWFSIDCEJECTCARD)requests[reqId]->lpParam;
	WORD wEjectPosition;
	BYTE cmd = 0x01;
	if (!lpEjectCard)
		wEjectPosition = WFS_IDC_EXITPOSITION;
	else
		wEjectPosition = lpEjectCard->wEjectPosition;

	if (wEjectPosition == WFS_IDC_TRANSPORTPOSITION)
		cmd = 0x02;

	AutoPort port(this->port, this->baudRate);
	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	updateCardStatus(port.port);	

	if (status.fwDevice != WFS_IDC_DEVONLINE)
	{
		Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));
		if (status.fwDevice == WFS_IDC_DEVBUSY)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		}
		else
		{
			requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		}
		return;
	}

	if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
	{
		Xfs::getInstance()->l.debug_dev("Media jammed");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		return;
	}
	else
	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		Xfs::getInstance()->l.debug_dev("Media not present, nothing to eject");
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_NOMEDIA;
		return;
	}
	else
	{
		
		int ec = CRT310_MovePosition(port.port, cmd);
		if (ec != 0)
		{
			GetErrCode(&ec);
			checkEc(ec);
			requests[reqId]->pResult->hResult = this->hResult;
			return;

		}
	}

	//sendWFS_EXEE_IDC_INSERTCARD(requests[reqId]->hservice);
	while (1)
	{
		updateCardStatus(port.port);
		if (wEjectPosition == WFS_IDC_EXITPOSITION && status.fwMedia == WFS_IDC_MEDIAENTERING)
			break;

		
		if (wEjectPosition == WFS_IDC_EXITPOSITION && status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
			break;
		Sleep(10);
	}

	Xfs::getInstance()->l.debug_dev("Disabling card-in");

	int ec = CRT310_CardSetting(port.port, 0x1, 0x1);
	if (ec != 0)
	{
		GetErrCode(&ec);
		Xfs::getInstance()->l.debug_dev("Error disabling card-in:" + std::to_string(ec));

		checkEc(ec);
		requests[reqId]->pResult->hResult = this->hResult;
	}
	updateCardStatus(port.port);

	if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
	{
		requests[reqId]->pResult->hResult = WFS_SUCCESS;
	}
	else
		if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
		{
			requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
		}

	if (requests[reqId]->pResult->hResult == WFS_SUCCESS)
	{
		sendWFS_SRVE_IDC_MEDIAREMOVED(requests[reqId]->hservice);		
	}

}

void IDCCRT310::execWFS_CMD_IDC_RESET_COUNT(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_RESET_COUNT reqId=" + std::to_string(reqId));

	requests[reqId]->pResult->hResult = WFS_SUCCESS;
	
	status.usCards = 0;		
	saveRetainedCount();		
	sendWFS_USRE_IDC_RETAINBINTHRESHOLD(requests[reqId]->hservice, WFS_IDC_RETAINBINOK);
}

void IDCCRT310::execWFS_CMD_IDC_RESET(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_RESET reqId=" + std::to_string(reqId));
	BYTE eject = 0;

	LPWORD lpwResetIn;	
	lpwResetIn = (LPWORD)requests[reqId]->lpParam;
	if (lpwResetIn)
	{
		if (*lpwResetIn == WFS_IDC_EJECT)eject = 1;
		if (*lpwResetIn == WFS_IDC_RETAIN)eject = 2;
		if (*lpwResetIn == WFS_IDC_NOACTION)eject = 0;
	}

	if (eject == 2 && status.usCards >= caps.usCards)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_RETAINBINFULL;
		return;
	}


	AutoPort port(this->port, this->baudRate);
	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	updateCardStatus(port.port);

	Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));
	if (status.fwDevice == WFS_IDC_DEVBUSY)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		return;
	}
	if (status.fwDevice == WFS_IDC_DEVOFFLINE  ||
		status.fwDevice == WFS_IDC_DEVPOWEROFF ||
		status.fwDevice == WFS_IDC_DEVNODEVICE ||
		status.fwDevice == WFS_IDC_DEVNODEVICE)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_INTERNAL_ERROR;		
		return;
	}
	
	WORD media = status.fwMedia;
	
		
	int ec = CRT310_Reset(port.port, eject);
	if (ec != 0)
	{
		GetErrCode(&ec);
		checkEc(ec);
		requests[reqId]->pResult->hResult = this->hResult;
		return;
	}
	
	
	Sleep(2000);		
	
	updateCardStatus(port.port);

	if (media == WFS_IDC_MEDIAPRESENT ||
		media == WFS_IDC_MEDIAJAMMED ||
		media == WFS_IDC_MEDIAENTERING)
	{
		if (status.fwMedia == WFS_IDC_MEDIAPRESENT)
		{
			sendWFS_SRVE_IDC_MEDIADETECTED(requests[reqId]->hservice, WFS_IDC_CARDREADPOSITION);
		}		
		if (status.fwMedia == WFS_IDC_MEDIAJAMMED)
		{
			sendWFS_SRVE_IDC_MEDIADETECTED(requests[reqId]->hservice, WFS_IDC_CARDJAMMED);

			requests[reqId]->pResult->hResult = WFS_ERR_IDC_MEDIAJAM;
			return;
		}
		
		if (status.fwMedia == WFS_IDC_MEDIANOTPRESENT)
		{
			if(eject == 1)
				sendWFS_SRVE_IDC_MEDIADETECTED(requests[reqId]->hservice, WFS_IDC_CARDEJECTED);
			if (eject == 2)
			{
				sendWFS_SRVE_IDC_MEDIADETECTED(requests[reqId]->hservice, WFS_IDC_CARDRETAINED);
				sendWFS_EXEE_IDC_MEDIARETAINED(requests[reqId]->hservice);
				status.usCards++;
				saveRetainedCount();

				if (status.usCards >= this->caps.usCards)
				{
					sendWFS_USRE_IDC_RETAINBINTHRESHOLD(requests[reqId]->hservice, WFS_ERR_IDC_RETAINBINFULL);
				}
				if (status.usCards >= this->retainBinTreshold)
				{
					sendWFS_USRE_IDC_RETAINBINTHRESHOLD(requests[reqId]->hservice, WFS_IDC_RETAINBINHIGH);
				}				
			}
		}		
	}

	requests[reqId]->pResult->hResult = WFS_SUCCESS;
}

void IDCCRT310::execWFS_CMD_IDC_SET_GUIDANCE_LIGHT(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug_dev("execWFS_CMD_IDC_SET_GUIDANCE_LIGHT reqId=" + std::to_string(reqId));

	LPWFSIDCSETGUIDLIGHT lpSetGuidLight;

	lpSetGuidLight = (LPWFSIDCSETGUIDLIGHT)requests[reqId]->lpParam;

	//lpSetGuidLight->wGuidLight = 0;

	int on = 0;

	Xfs::getInstance()->l.debug_dev("Command received LIGHT = "+std::to_string(lpSetGuidLight->dwCommand));

	if (lpSetGuidLight->dwCommand == 640)
	{
		Xfs::getInstance()->l.debug_dev("Command received LIGHT ON" );

		on = 1;
	}
	else
	{
		Xfs::getInstance()->l.debug_dev("Command received LIGHT OFF");
		on = 0;
	}


	if (lpSetGuidLight->wGuidLight >= WFS_IDC_GUIDLIGHTS_SIZE)
	{
		Xfs::getInstance()->l.debug_dev("Guidlight not avaialabe id=" + std::to_string(lpSetGuidLight->wGuidLight));
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_INVALID_PORT;
		return;
	}
	if (this->caps.dwGuidLights[lpSetGuidLight->wGuidLight] == WFS_IDC_GUIDANCE_NOT_AVAILABLE)
	{
		Xfs::getInstance()->l.debug_dev("Guidlight not avaialabe id=" + std::to_string(lpSetGuidLight->wGuidLight));
		requests[reqId]->pResult->hResult = WFS_ERR_IDC_INVALID_PORT;
		return;
	}
	/*
	if (lpSetGuidLight->dwCommand&WFS_IDC_GUIDANCE_OFF == WFS_IDC_GUIDANCE_OFF)
	{
		Xfs::getInstance()->l.slog("COMMAND = WFS_IDC_GUIDANCE_OFF");
	}
	if (lpSetGuidLight->dwCommand& WFS_IDC_GUIDANCE_CONTINUOUS)
	{
		Xfs::getInstance()->l.slog("COMMAND = WFS_IDC_GUIDANCE_CONTINUOUS");
	}


	if (lpSetGuidLight->dwCommand != WFS_IDC_GUIDANCE_OFF)
	{
		if (!(this->caps.dwGuidLights[lpSetGuidLight->wGuidLight] & lpSetGuidLight->dwCommand))
		{
			Xfs::getInstance()->l.slog("Wrong command=" + std::to_string(lpSetGuidLight->dwCommand));

			requests[reqId]->pResult->hResult = WFS_ERR_IDC_COMMANDUNSUPP;
			return;
		}
	}
	*/
	AutoPort port(this->port, this->baudRate);
	if (!port.port)
	{
		if (port.ec == -101)
			status.fwDevice = WFS_IDC_DEVPOWEROFF;

		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	updateCardStatus(port.port);

	Xfs::getInstance()->l.debug_dev("Status.fwDevice != WFS_IDC_DEVONLINE, faDevice=" + std::to_string(status.fwDevice));
	if (status.fwDevice == WFS_IDC_DEVBUSY)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_OP_IN_PROGRESS;
		return;
	}
	
	BYTE onoff = 0;
	if (on)
		onoff = 0;
	else
		onoff = 1;
	int ec = CRT310_LEDSet(port.port, onoff);
	if (ec != 0)
	{
		GetErrCode(&ec);
		Xfs::getInstance()->l.debug_dev("Error executing ledset=" + std::to_string(ec));
		checkEc(ec);
		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}
	BYTE t1;
	BYTE t2;
	/*
	if (lpSetGuidLight->dwCommand&WFS_IDC_GUIDANCE_OFF)
	{
		Xfs::getInstance()->l.slog("EXECUTING WFS_IDC_GUIDANCE_OFF = ");

		t1 = 0;
		t2 = 0;
	}
	if (lpSetGuidLight->dwCommand != 0x0280)
	{
		Xfs::getInstance()->l.slog("EXECUTING WFS_IDC_GUIDANCE_OFF");
		t1 = 0x01;
		t2 = 0xDF;
	}
	if (lpSetGuidLight->dwCommand&WFS_IDC_GUIDANCE_MEDIUM_FLASH)
	{
		t1 = 0x01;
		t2 = 0xAF;
	}
	if (lpSetGuidLight->dwCommand&WFS_IDC_GUIDANCE_QUICK_FLASH)
	{
		t1 = 0x01;
		t2 = 0x0F;
	}
	if (lpSetGuidLight->dwCommand== 0x0280)
	{
		Xfs::getInstance()->l.slog("EXECUTING WFS_IDC_GUIDANCE_CONTINUOUS");
		t1 = 0xff;
		t2 = 0;
	}
	*/

	if (on)
	{
		onoff = 0;
		Xfs::getInstance()->l.debug_dev("SET LIHGT ON!!!!!");
		t1 = 0xff;
		t2 = 0;
	}
	else
	{
		onoff = 1;
		Xfs::getInstance()->l.debug_dev("SET LIHGT OFFFFFFF!!!!!");
		t1 = 0;
		t2 = 0;
	}

	ec = CRT310_LEDTime(port.port, t1, t2);
	if (ec != 0)
	{
		GetErrCode(&ec);
		Xfs::getInstance()->l.debug_dev("Error executing ledtime=" + std::to_string(ec));
		checkEc(ec);
		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}

	requests[reqId]->pResult->hResult = WFS_SUCCESS;
}


//Supp!W@#
void IDCCRT310::checkEc(int ec)
{
	

	if (ec == -2)// Card Type error
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec)+" (Card Type error)");
	}
	if (ec == - 101)// Serial port error == > Execute CommOpen or CommOpenWithBaut function to configure
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Serial port error == > Execute CommOpen or CommOpenWithBaut function to configure)");
	}
	if (ec == - 102)// Baud Rate setting error == > Execute CommOpenWithBaut to open serial port
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Execute CommOpenWithBaut to open serial port)");
	}
	if (ec == - 200)// 0 Undefined parameter
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Undefined parameter)");
	}
	if (ec == - 202)// 1 Command out of model == > Check if command support by the model number
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Command out of model == > Check if command support by the model number)");
	}
	if (ec == - 204)// 2 Command out of order == > Wrong command
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Command out of order == > Wrong command)");
	}
	if (ec == - 205)// 3 Power error == > Check power outputDC12V
	{
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Power error == > Check power outputDC12V)");
	}
	if (ec == - 206)// 4 Over - short or long card inside
	{
		this->status.fwDevice = WFS_IDC_DEVNODEVICE;
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Over - short or long card inside)");
	}
	if (ec == - 207)// 5 Power off == > Check the power
	{
		this->status.fwDevice = WFS_IDC_DEVNODEVICE;
		Xfs::getInstance()->l.debug_dev("Error code=" + std::to_string(ec) + " (Power off == > Check the power)");
	}



	hResult = WFS_ERR_INTERNAL_ERROR;
}

void IDCCRT310::updateCardStatus(HANDLE port)
{	
	
	Xfs::getInstance()->l.debug_dev("Updating device status (card position/settings)");
	
	//Card position

	BYTE atPosition;
	BYTE frontSetting;
	BYTE rearSetting;

	int ec = CRT310_GetStatus(port, &atPosition, &frontSetting, &rearSetting);
	if (ec)
	{
		Xfs::getInstance()->l.debug_dev("Error, ec="+std::to_string(ec));
		this->status.fwDevice = WFS_IDC_DEVHWERROR;
		return;
	}
	else
		this->status.fwDevice = WFS_IDC_DEVONLINE;

	std::string sAtPosition;
	switch (atPosition)
	{
		case 0X46: {
			sAtPosition = "Over - long card";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;
			this->hResult = WFS_ERR_IDC_CARDTOOLONG;
			break; 
		}//Over - long card
		case 0X47: {
			sAtPosition = "Over - short card";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;
			this->hResult = WFS_ERR_IDC_CARDTOOSHORT;
			break; 
		}// Over - short card
		case 0X48: {
			sAtPosition = "Card exist in front without holding position";
			this->status.fwMedia = WFS_IDC_MEDIAENTERING;
			break; 
		}// Card exist in front without holding position
		case 0X49: {
			sAtPosition = "Card exist in front with holding position";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;

			break; 
		}// Card exist in front with holding position
		case 0X4A: {
			sAtPosition = "Card in the reader";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;
			break; 
		}// Card in the reader
		case 0X4B: {
			sAtPosition = "Card in IC card operation position and contact with IC contact";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;

			break; 
		}// Card in IC card operation position and contact with IC contact
		case 0X4C: {			
			sAtPosition = "Card in rear side with holding position";
			this->status.fwMedia = WFS_IDC_MEDIAPRESENT;

			break; 
		}// Card in rear side with holding position
		case 0X4D: {
			sAtPosition = "Card in rear side without holding position";
			this->status.fwMedia = WFS_IDC_MEDIAENTERING;

			break; 
		}// Card in rear side without holding position
		case 0X4E: {
			sAtPosition = "No card in the reader";
			this->status.fwMedia = WFS_IDC_MEDIANOTPRESENT;

			break; 
		}// No card in the reader
		default	 : {
			this->status.fwMedia = WFS_IDC_MEDIAUNKNOWN;

			sAtPosition = "UNKOWN";

			break; 
		}
	}

	std::string sFrontSetting;

	switch (frontSetting)
	{
		case 0X49: {
			sFrontSetting = "Card in only with magnetic signal";


			break; 
		}// Card in only with magnetic signal
		case 0X4A: {
			sFrontSetting = "Card in only with switch signal";


			break; 
		}// Card in only with switch signal
		case 0X4B: {
			sFrontSetting = "Card in with magnetic signal";

			break; 
		}// Card in with magnetic signal
		case 0X4E: {
			sFrontSetting = "Prohibit card in";

			break; 
		}// Prohibit card in
		default: {
			sFrontSetting = "UNKNOWN";

			break; 
		}
	}

	std::string sRearSetting;

	switch (rearSetting)
	{
		case 0X4A: {
			sRearSetting = "Card in from rear side";

			break; 
		}// Card in from rear side
		case 0X4E: {
			sRearSetting = "Prohibit card in from rear side";


			break; 
		}// Prohibit card in from rear side
		default: {break; }
	}
	
	Xfs::getInstance()->l.debug_dev("Card position device status (card position):");
	Xfs::getInstance()->l.debug_dev("atPosition   = " + std::to_string(atPosition)+" ("+sAtPosition+")");
	Xfs::getInstance()->l.debug_dev("frontSetting = " + std::to_string(frontSetting) + " (" + sFrontSetting + ")");
	Xfs::getInstance()->l.debug_dev("rearSetting  = " + std::to_string(rearSetting) + " (" + sRearSetting + ")");


}

void IDCCRT310::loadCapabilities()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);

	this->caps.usCards = pt.get<int>("RETAINBINMAX");
	this->retainBinTreshold = pt.get<int>("RETAINBINTRESHOLD");
}

void IDCCRT310::loadRetainedCount()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);
	
	this->status.usCards = pt.get<int>("RETAINBIN");
	

}

void IDCCRT310::saveRetainedCount()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);

	pt.put("RETAINBIN", this->status.usCards);

	boost::property_tree::write_json(configName, pt);
	
}