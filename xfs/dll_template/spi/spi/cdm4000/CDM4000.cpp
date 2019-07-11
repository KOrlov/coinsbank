#include "stdafx.h"
#include "CDM4000.h"
#include <boost/algorithm/string.hpp>
#include "cdm4000/MFSCommCDM.h"
#define TE1STING
using namespace boost::filesystem;
using namespace boost::property_tree;



void CDM4000::checkStatus()
{
#ifdef TESTING
	this->status.fwDevice = WFS_CDM_DEVONLINE;
	return;
#endif // TESTING



	Xfs::getInstance()->l.debug("checkStatus() execution");

	if (!cdmOpened)
	{
		Xfs::getInstance()->l.debug("cdm is not opened");
		this->status.fwDevice = WFS_CDM_DEVOFFLINE;
		return;
	}
	else
		this->status.fwDevice = WFS_CDM_DEVONLINE;
	CDMSTATUS s;

	Xfs::getInstance()->l.debug("cdm is not opened");

	if (!MFSCommCDM_Status(&s))
	{
		Xfs::getInstance()->l.debug("!MFSCommCDM_Status(&s)");
	}
	Xfs::getInstance()->l.debug("s.error_cd=" + std::to_string(s.error_cd) + " s.reject_cd=" + std::to_string(s.reject_cd));
	if (s.error_cd == 0x30)
	{
		this->status.fwDevice = WFS_CDM_DEVONLINE;
	}
	else
	if (s.error_cd < 0x5E)
	{
		this->status.fwDevice = WFS_CDM_DEVHWERROR;
	}
	   
	this->nearEndHardware[0] = (s.sensor[0] & 0x20) == 0x20; //cassete 0
	this->nearEndHardware[1] = (s.sensor[2] & 0x08) == 0x08; //cassete 1
	this->nearEndHardware[2] = (s.sensor[3] & 0x08) == 0x08; //cassete 2
	this->nearEndHardware[3] = (s.sensor[4] & 0x08) == 0x08; //cassete 3


	if (s.error_cd == 0x5E)//CASSETTE_BOX1_NO
	{
		this->cashUnits[0].usStatus = WFS_CDM_STATCUMISSING;
		this->phcu[0].usPStatus = WFS_CDM_STATCUMISSING;
	}
	if (s.error_cd == 0x5F)//CASSETTE_BOX2_NO
	{
		this->cashUnits[1].usStatus = WFS_CDM_STATCUMISSING;
		this->phcu[1].usPStatus = WFS_CDM_STATCUMISSING;
	}
	if (s.error_cd == 0x60)//CASSETTE_BOX3_NO
	{
		this->cashUnits[2].usStatus = WFS_CDM_STATCUMISSING;
		this->phcu[2].usPStatus = WFS_CDM_STATCUMISSING;
	}
	if (s.error_cd == 0x61)//CASSETTE_BOX4_NO
	{
		this->cashUnits[3].usStatus = WFS_CDM_STATCUMISSING;
		this->phcu[3].usPStatus = WFS_CDM_STATCUMISSING;
	}
	
	char buf[10];
	itoa(s.error_cd, &buf[0], 16);
	errLine = "s.error_cd == "+std::string(buf);
}

bool CDM4000::openCdm()
{


#ifdef TESTING
	cdmOpened = true;
	return true;
#endif 

	cdmOpened = MFSCommCDM_OpenPort(cdmHWND, this->port);
	return cdmOpened;
}

bool CDM4000::closeCdm()
{
#ifdef TESTING
	return true;
#endif 
	return MFSCommCDM_ClosePort();
}


void CDM4000::copyLpInput(LPVOID lpData, REQUESTID reqId) 
{
	Xfs::getInstance()->l.debug("copyLpInput:"+std::to_string(requests[reqId]->messageType)+ ", command="+ std::to_string(requests[reqId]->cmd));


	if (requests[reqId]->messageType == MSG_EXECUTE && lpData)
	{
		if (requests[reqId]->cmd == WFS_CMD_CDM_DENOMINATE)
		{
			LPWFSCDMDENOMINATE lpDenominates = (LPWFSCDMDENOMINATE)lpData;
			
			memAlloc->AllocateBuffer(sizeof(WFSCDMDENOMINATE), (void **)&requests[reqId]->lpParam);

			LPWFSCDMDENOMINATE lpDenominated = (LPWFSCDMDENOMINATE)requests[reqId]->lpParam;
			lpDenominated->usTellerID = lpDenominates->usTellerID;
			lpDenominated->usMixNumber = lpDenominates->usMixNumber;
			if (!lpDenominates->lpDenomination)return;
			memAlloc->AllocateMore(sizeof(WFSCDMDENOMINATION), requests[reqId]->lpParam,(void **)&lpDenominated->lpDenomination);
			lpDenominated->lpDenomination->cCurrencyID[0] = lpDenominates->lpDenomination->cCurrencyID[0];
			lpDenominated->lpDenomination->cCurrencyID[1] = lpDenominates->lpDenomination->cCurrencyID[1];
			lpDenominated->lpDenomination->cCurrencyID[2] = lpDenominates->lpDenomination->cCurrencyID[2];

			lpDenominated->lpDenomination->ulAmount = lpDenominates->lpDenomination->ulAmount;
			lpDenominated->lpDenomination->ulCashBox = lpDenominates->lpDenomination->ulCashBox;
			lpDenominated->lpDenomination->usCount = lpDenominates->lpDenomination->usCount;
			if(lpDenominated->lpDenomination->usCount)
				memAlloc->AllocateMore(sizeof(SHORT)*lpDenominated->lpDenomination->usCount, requests[reqId]->lpParam, (void **)&lpDenominated->lpDenomination->lpulValues);
			for (int i = 0; i < lpDenominated->lpDenomination->usCount; i++)
				lpDenominated->lpDenomination->lpulValues[i] = lpDenominates->lpDenomination->lpulValues[i];
			
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_DISPENSE)
		{
			Xfs::getInstance()->l.debug("WFS_CMD_CDM_DISPENSE params parse ENTER");
			LPWFSCDMDISPENSE lpDispenses = (LPWFSCDMDISPENSE)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMDISPENSE), (void **)&requests[reqId]->lpParam);

			LPWFSCDMDISPENSE lpDispensed = (LPWFSCDMDISPENSE)requests[reqId]->lpParam;
			lpDispensed->bPresent = lpDispenses->bPresent;
			lpDispensed->fwPosition = lpDispenses->fwPosition;
			lpDispensed->usMixNumber = lpDispenses->usMixNumber;
			lpDispensed->usTellerID = lpDispenses->usTellerID;

			if (!lpDispenses->lpDenomination)return;

			memAlloc->AllocateMore(sizeof(WFSCDMDENOMINATION), requests[reqId]->lpParam, (void **)&lpDispensed->lpDenomination);
			lpDispensed->lpDenomination->cCurrencyID[0] = lpDispenses->lpDenomination->cCurrencyID[0];
			lpDispensed->lpDenomination->cCurrencyID[1] = lpDispenses->lpDenomination->cCurrencyID[1];
			lpDispensed->lpDenomination->cCurrencyID[2] = lpDispenses->lpDenomination->cCurrencyID[2];

			lpDispensed->lpDenomination->ulAmount = lpDispenses->lpDenomination->ulAmount;
			lpDispensed->lpDenomination->ulCashBox = lpDispenses->lpDenomination->ulCashBox;
			lpDispensed->lpDenomination->usCount = lpDispenses->lpDenomination->usCount;
			
			if (lpDispensed->lpDenomination->usCount)
				memAlloc->AllocateMore(sizeof(long)*lpDispensed->lpDenomination->usCount, requests[reqId]->lpParam, (void **)&lpDispensed->lpDenomination->lpulValues);
			for (int i = 0; i < lpDispensed->lpDenomination->usCount; i++)
				lpDispensed->lpDenomination->lpulValues[i] = lpDispenses->lpDenomination->lpulValues[i];
			Xfs::getInstance()->l.debug("WFS_CMD_CDM_DISPENSE params parse EXIT");


		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_COUNT)
		{
			LPWFSCDMPHYSICALCU lpPhysicalCUs = (LPWFSCDMPHYSICALCU)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMPHYSICALCU), (void **)&requests[reqId]->lpParam);
			LPWFSCDMPHYSICALCU lpPhysicalCUd = (LPWFSCDMPHYSICALCU)requests[reqId]->lpParam;
			lpPhysicalCUd->bEmptyAll = lpPhysicalCUs->bEmptyAll;
			lpPhysicalCUd->fwPosition = lpPhysicalCUs->fwPosition;
			if (lpPhysicalCUs->lpPhysicalPositionName)
			{
				memAlloc->AllocateMore(strlen(lpPhysicalCUs->lpPhysicalPositionName)+1, requests[reqId]->lpParam, (void **)&lpPhysicalCUd->lpPhysicalPositionName);
				strcpy(lpPhysicalCUd->lpPhysicalPositionName, lpPhysicalCUs->lpPhysicalPositionName);				
			}			
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_PRESENT)
		{
			LPWORD lpfwPositions = (LPWORD) lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpfwPositiond = (LPWORD)lpData;
			*lpfwPositiond = *lpfwPositions;
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_REJECT)
		{			
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_RETRACT)
		{
			LPWFSCDMRETRACT lpRetracts = (LPWFSCDMRETRACT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMRETRACT), (void **)&requests[reqId]->lpParam);
			LPWFSCDMRETRACT lpRetractd = (LPWFSCDMRETRACT)requests[reqId]->lpParam;
			lpRetractd->fwOutputPosition = lpRetracts->fwOutputPosition;
			lpRetractd->usIndex = lpRetracts->usIndex;
			lpRetractd->usRetractArea = lpRetracts->usRetractArea;
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_OPEN_SHUTTER || requests[reqId]->cmd == WFS_CMD_CDM_CLOSE_SHUTTER)
		{
			LPWORD lpfwPositions= (LPWORD)lpData;
			memAlloc->AllocateBuffer(sizeof(WORD), (void **)&requests[reqId]->lpParam);
			LPWORD lpfwPositiond = (LPWORD)  requests[reqId]->lpParam;
			*lpfwPositiond = *lpfwPositions;

		}
		
		if (requests[reqId]->cmd == WFS_CMD_CDM_SET_TELLER_INFO)
		{
			return;
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_SET_CASH_UNIT_INFO || requests[reqId]->cmd == WFS_CMD_CDM_END_EXCHANGE)
		{
			Xfs::getInstance()->l.debug("WFS_CMD_CDM_END_EXCHANGE="+std::to_string((int)lpData));

			LPWFSCDMCUINFO lpCUInfos = (LPWFSCDMCUINFO)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMCUINFO), (void **)&requests[reqId]->lpParam);
			LPWFSCDMCUINFO lpCUInfod = (LPWFSCDMCUINFO)requests[reqId]->lpParam;
			lpCUInfod->usTellerID = lpCUInfos->usTellerID;
			lpCUInfod->usCount = lpCUInfos->usCount;

			if (!lpCUInfod->usCount)return;
			memAlloc->AllocateMore(4* lpCUInfod->usCount, requests[reqId]->lpParam, (void **)&lpCUInfod->lppList);

			for (int i = 0; i < lpCUInfod->usCount; i++)
			{			
				memAlloc->AllocateMore(sizeof(WFSCDMCASHUNIT), requests[reqId]->lpParam, (void **)&lpCUInfod->lppList[i]);

				lpCUInfod->lppList[i]->usNumber = lpCUInfos->lppList[i]->usNumber;
				lpCUInfod->lppList[i]->usType = lpCUInfos->lppList[i]->usType;
				if (lpCUInfos->lppList[i]->lpszCashUnitName)
				{
					memAlloc->AllocateMore(strlen(lpCUInfos->lppList[i]->lpszCashUnitName)+1, requests[reqId]->lpParam, (void **)&lpCUInfod->lppList[i]->lpszCashUnitName);
					strcpy(lpCUInfod->lppList[i]->lpszCashUnitName, lpCUInfos->lppList[i]->lpszCashUnitName);
				}

				for(int c=0;c<5;c++)
					lpCUInfod->lppList[i]->cUnitID[c] = lpCUInfos->lppList[i]->cUnitID[c];
				for (int c = 0; c < 3; c++)
					lpCUInfod->lppList[i]->cCurrencyID[c]= lpCUInfos->lppList[i]->cCurrencyID[c];
				lpCUInfod->lppList[i]->ulValues= lpCUInfos->lppList[i]->ulValues;
				lpCUInfod->lppList[i]->ulInitialCount = lpCUInfos->lppList[i]->ulInitialCount;
				lpCUInfod->lppList[i]->ulCount = lpCUInfos->lppList[i]->ulCount;
				lpCUInfod->lppList[i]->ulRejectCount = lpCUInfos->lppList[i]->ulRejectCount;
				lpCUInfod->lppList[i]->ulMinimum = lpCUInfos->lppList[i]->ulMinimum;
				lpCUInfod->lppList[i]->ulMaximum = lpCUInfos->lppList[i]->ulMaximum;
				lpCUInfod->lppList[i]->bAppLock = lpCUInfos->lppList[i]->bAppLock;
				lpCUInfod->lppList[i]->usStatus = lpCUInfos->lppList[i]->usStatus;
				lpCUInfod->lppList[i]->usNumPhysicalCUs  = lpCUInfos->lppList[i]->usNumPhysicalCUs;				
				lpCUInfod->lppList[i]->ulDispensedCount = lpCUInfos->lppList[i]->ulDispensedCount;
				lpCUInfod->lppList[i]->ulPresentedCount = lpCUInfos->lppList[i]->ulPresentedCount;
				lpCUInfod->lppList[i]->ulRetractedCount = lpCUInfos->lppList[i]->ulRetractedCount;
				
				
				
				continue;
				if (!lpCUInfos->lppList[i]->lppPhysical)continue;
				
				memAlloc->AllocateMore(4* lpCUInfos->lppList[i]->usNumPhysicalCUs, requests[reqId]->lpParam, (void **)&lpCUInfod->lppList[i]->lppPhysical);
				
				for (int c = 0; c < lpCUInfos->lppList[i]->usNumPhysicalCUs; c++)
				{
					memAlloc->AllocateMore(sizeof(WFSCDMPHCU), requests[reqId]->lpParam, (void **)&lpCUInfod->lppList[i]->lppPhysical[c]);

					if (lpCUInfos->lppList[i]->lppPhysical[c]->lpPhysicalPositionName)
					{
						memAlloc->AllocateMore(strlen(lpCUInfos->lppList[i]->lppPhysical[c]->lpPhysicalPositionName)+1, requests[reqId]->lpParam, (void **)&lpCUInfod->lppList[i]->lppPhysical[c]->lpPhysicalPositionName);
						strcpy(lpCUInfod->lppList[i]->lppPhysical[c]->lpPhysicalPositionName, lpCUInfos->lppList[i]->lppPhysical[c]->lpPhysicalPositionName);
					}

					for (int k = 0; k < 5; k++)
						lpCUInfod->lppList[i]->lppPhysical[c]->cUnitID[k] = lpCUInfos->lppList[i]->lppPhysical[c]->cUnitID[k];
					
					lpCUInfod->lppList[i]->lppPhysical[c]->ulInitialCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulInitialCount;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulCount;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulRejectCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulRejectCount;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulMaximum = lpCUInfos->lppList[i]->lppPhysical[c]->ulMaximum;
					lpCUInfod->lppList[i]->lppPhysical[c]->usPStatus = lpCUInfos->lppList[i]->lppPhysical[c]->usPStatus;
					lpCUInfod->lppList[i]->lppPhysical[c]->bHardwareSensor = lpCUInfos->lppList[i]->lppPhysical[c]->bHardwareSensor;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulDispensedCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulDispensedCount;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulPresentedCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulPresentedCount;
					lpCUInfod->lppList[i]->lppPhysical[c]->ulRetractedCount = lpCUInfos->lppList[i]->lppPhysical[c]->ulRetractedCount;
				}			

			}

		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_START_EXCHANGE)
		{
			LPWFSCDMSTARTEX lpStartExs = (LPWFSCDMSTARTEX) lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMSTARTEX), (void **)&requests[reqId]->lpParam);
			LPWFSCDMSTARTEX lpStartExd = (LPWFSCDMSTARTEX)requests[reqId]->lpParam;
			lpStartExd->usTellerID = lpStartExs->usTellerID;
			lpStartExd->fwExchangeType = lpStartExs->fwExchangeType;
			lpStartExd->usCount = lpStartExs->usCount;
			if (!lpStartExd->usCount)return;
			memAlloc->AllocateMore(sizeof(short)*lpStartExd->usCount, requests[reqId]->lpParam, (void **)&lpStartExd->lpusCUNumList);

			for (int c = 0; c < lpStartExd->usCount; c++)
				lpStartExd->lpusCUNumList[c] = lpStartExs->lpusCUNumList[c];

		}
		
		if (requests[reqId]->cmd == WFS_CMD_CDM_OPEN_SAFE_DOOR)
		{
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_CALIBRATE_CASH_UNIT)
		{
			LPWFSCDMCALIBRATE lpCalibrateIns = (LPWFSCDMCALIBRATE)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMCALIBRATE), (void **)&requests[reqId]->lpParam);
			LPWFSCDMCALIBRATE lpCalibrateInd = (LPWFSCDMCALIBRATE)requests[reqId]->lpParam;
			lpCalibrateInd->usNumOfBills = lpCalibrateIns->usNumOfBills;
			lpCalibrateInd->usNumber = lpCalibrateIns->usNumber;
			if (!lpCalibrateIns->lpPosition)return;
			memAlloc->AllocateMore(sizeof(LPWFSCDMITEMPOSITION), requests[reqId]->lpParam, (void **)&lpCalibrateInd->lpPosition);
			memAlloc->AllocateMore(sizeof(WFSCDMITEMPOSITION), requests[reqId]->lpParam, (void **)&lpCalibrateInd->lpPosition[0]);
			
			lpCalibrateInd->lpPosition[0]->usNumber = lpCalibrateIns->lpPosition[0]->usNumber;
			lpCalibrateInd->lpPosition[0]->fwOutputPosition = lpCalibrateIns->lpPosition[0]->fwOutputPosition;
			if (lpCalibrateIns->lpPosition[0]->lpRetractArea)
			{
				memAlloc->AllocateMore(sizeof(WFSCDMRETRACT), requests[reqId]->lpParam, (void **)&lpCalibrateInd->lpPosition[0]->lpRetractArea);
				lpCalibrateInd->lpPosition[0]->lpRetractArea->fwOutputPosition = lpCalibrateIns->lpPosition[0]->lpRetractArea->fwOutputPosition;
				lpCalibrateInd->lpPosition[0]->lpRetractArea->usIndex = lpCalibrateIns->lpPosition[0]->lpRetractArea->usIndex;
				lpCalibrateInd->lpPosition[0]->lpRetractArea->usRetractArea = lpCalibrateIns->lpPosition[0]->lpRetractArea->usRetractArea;
			}			
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_SET_MIX_TABLE)
		{
			return;
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_RESET || requests[reqId]->cmd == WFS_CMD_CDM_TEST_CASH_UNITS)
		{
			LPWFSCDMITEMPOSITION lpResetIns = (LPWFSCDMITEMPOSITION)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMITEMPOSITION), (void **)&requests[reqId]->lpParam);
			LPWFSCDMITEMPOSITION lpResetInd = (LPWFSCDMITEMPOSITION)requests[reqId]->lpParam;
			lpResetInd->fwOutputPosition = lpResetIns->fwOutputPosition;
			lpResetInd->usNumber = lpResetIns->usNumber;
			if (!lpResetIns->lpRetractArea)return;
			memAlloc->AllocateMore(sizeof(WFSCDMRETRACT), requests[reqId]->lpParam, (void **)&lpResetInd->lpRetractArea);
			lpResetInd->lpRetractArea->fwOutputPosition = lpResetIns->lpRetractArea->fwOutputPosition;
			lpResetInd->lpRetractArea->usIndex = lpResetIns->lpRetractArea->usIndex;
			lpResetInd->lpRetractArea->usRetractArea = lpResetIns->lpRetractArea->usRetractArea;

		}
		
		if (requests[reqId]->cmd == WFS_CMD_CDM_SET_GUIDANCE_LIGHT)
		{
			LPWFSCDMSETGUIDLIGHT lpSetGuidLights = (LPWFSCDMSETGUIDLIGHT)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMSETGUIDLIGHT), (void **)&requests[reqId]->lpParam);
			LPWFSCDMSETGUIDLIGHT lpSetGuidLightd = (LPWFSCDMSETGUIDLIGHT)requests[reqId]->lpParam;
			lpSetGuidLightd->dwCommand = lpSetGuidLights->dwCommand;
			lpSetGuidLightd->wGuidLight = lpSetGuidLights->wGuidLight;

		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_POWER_SAVE_CONTROL)
		{
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_PREPARE_DISPENSE)
		{
			LPWFSCDMPREPAREDISPENSE lpPrepareDispenses = (LPWFSCDMPREPAREDISPENSE)lpData;
			memAlloc->AllocateBuffer(sizeof(WFSCDMPREPAREDISPENSE), (void **)&requests[reqId]->lpParam);
			LPWFSCDMPREPAREDISPENSE lpPrepareDispensed = (LPWFSCDMPREPAREDISPENSE)requests[reqId]->lpParam;
			lpPrepareDispensed->wAction = lpPrepareDispenses->wAction;				

		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_SET_BLACKLIST)
		{
		}
		if (requests[reqId]->cmd == WFS_CMD_CDM_SYNCHRONIZE_COMMAND)
		{
		}
			
	}
	if (requests[reqId]->messageType == MSG_GETINFO && lpData)
	{
		if (requests[reqId]->cmd == WFS_INF_CDM_STATUS)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_CAPABILITIES)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_CASH_UNIT_INFO)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_TELLER_INFO)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_CURRENCY_EXP)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_MIX_TYPES)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_MIX_TABLE)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_PRESENT_STATUS)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_GET_ITEM_INFO)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_GET_BLACKLIST)
		{
		}
		if (requests[reqId]->cmd == WFS_INF_CDM_GET_ALL_ITEMS_INFO)
		{
		}		
	}
}

void CDM4000::getInfoProc(REQUESTID reqId) 
{
	if (requests[reqId]->cmd == WFS_INF_CDM_STATUS)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_STATUS enter");

		getWFS_INF_CDM_STATUS(reqId);

		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_STATUS exit");
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CDM_CAPABILITIES)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CAPABILITIES enter");

		getWFS_INF_CDM_CAPABILITIES(reqId);

		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CAPABILITIES exit");
	}
	else
	if (requests[reqId]->cmd == WFS_INF_CDM_CASH_UNIT_INFO)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CASH_UNIT_INFO enter");

		getWFS_INF_CDM_CASH_UNIT_INFO(reqId);
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CASH_UNIT_INFO exit");

	}	
	else
	if (requests[reqId]->cmd == WFS_INF_CDM_CURRENCY_EXP)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CURRENCY_EXP enter");
		getWFS_INF_CDM_CURRENCY_EXP(reqId);
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_CURRENCY_EXP exit");

	}	
	else
	if (requests[reqId]->cmd == WFS_INF_CDM_MIX_TYPES)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_MIX_TYPES enter");

		getWFS_INF_CDM_MIX_TYPES(reqId);
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_MIX_TYPES exit");

	}
	else
	if (requests[reqId]->cmd == WFS_INF_CDM_MIX_TABLE)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_MIX_TABLE enter");

		getWFS_INF_CDM_MIX_TABLE(reqId);
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_INF_CDM_MIX_TABLE exit");

	}
	else
	{
		Xfs::getInstance()->l.debug("!!!!!WFS_ERR_UNSUPP_CATEGORY!!!!!");

		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_CATEGORY;
	}
		


}
void CDM4000::executeProc(REQUESTID reqId) 
{
	if (requests[reqId]->cmd == WFS_CMD_CDM_SET_CASH_UNIT_INFO)
	{
		execWFS_CMD_CDM_SET_CASH_UNIT_INFO(reqId);
	}		
	else
	if (requests[reqId]->cmd == WFS_CMD_CDM_START_EXCHANGE)
	{
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_CMD_CDM_START_EXCHANGE enter");

		execWFS_CMD_CDM_START_EXCHANGE(reqId);
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_CMD_CDM_START_EXCHANGE exit");

	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CDM_END_EXCHANGE)
	{	
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_CMD_CDM_END_EXCHANGE enter");

		execWFS_CMD_CDM_END_EXCHANGE(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CDM_DISPENSE)
	{

		
		Xfs::getInstance()->l.debug("requests[reqId]->cmd == WFS_CMD_CDM_DISPENSE enter");

		execWFS_CMD_CDM_DISPENSE(reqId);
	}
	else
	if (requests[reqId]->cmd == WFS_CMD_CDM_RESET)
	{
		execWFS_CMD_CDM_RESET(reqId);
	}	
	else		
		requests[reqId]->pResult->hResult = WFS_ERR_UNSUPP_COMMAND;
}

void CDM4000::sendWFS_SRVE_CDM_SAFEDOOROPEN(HSERVICE hService)
{
	throw "event not supported";
}

void CDM4000::sendWFS_SRVE_CDM_SAFEDOORCLOSED(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_USRE_CDM_CASHUNITTHRESHOLD(HSERVICE hService, int cu)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_USER_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->lpBuffer = constructLPWFSCDMCASHUNIT(pResult, cu);
		pResult->u.dwEventID = WFS_USRE_CDM_CASHUNITTHRESHOLD;
		sendMessage(i.first, WFS_USER_EVENT, (LPARAM)pResult);
	}	
	
}
void CDM4000::sendWFS_SRVE_CDM_CASHUNITINFOCHANGED(HSERVICE hService, int cu)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->lpBuffer = constructLPWFSCDMCASHUNIT(pResult, cu);
		pResult->u.dwEventID = WFS_SRVE_CDM_CASHUNITINFOCHANGED;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CDM4000::sendWFS_SRVE_CDM_TELLERINFOCHANGED(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_DELAYEDDISPENSE(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_STARTDISPENSE(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_CASHUNITERROR(HSERVICE hService, int cu, WORD wFailure)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;

		memAlloc->AllocateMore(sizeof(WFSCDMCUERROR), pResult, (void**)&pResult->lpBuffer);
		
		LPWFSCDMCUERROR s = (LPWFSCDMCUERROR)pResult->lpBuffer;
		s->lpCashUnit = constructLPWFSCDMCASHUNIT(pResult, cu);
		s->wFailure = wFailure;
		pResult->u.dwEventID = WFS_EXEE_CDM_CASHUNITERROR;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}
}
void CDM4000::sendWFS_SRVE_CDM_ITEMSTAKEN(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_SRVE_CDM_COUNTS_CHANGED(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_PARTIALDISPENSE(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_SUBDISPENSEOK(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_INCOMPLETEDISPENSE(HSERVICE hService, int amount, int cashBox, std::vector<long> values)
{	
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;

		memAlloc->AllocateMore(sizeof(WFSCDMDENOMINATION), pResult, (void**)&pResult->lpBuffer);
		LPWFSCDMDENOMINATION s = (LPWFSCDMDENOMINATION)pResult->lpBuffer;
		
		s->cCurrencyID[0] = 'E';
		s->cCurrencyID[1] = 'U';
		s->cCurrencyID[2] = 'R';
		s->ulAmount = amount;
		s->ulCashBox = cashBox;
		s->usCount = values.size();
		memAlloc->AllocateMore(sizeof(long)*values.size(), pResult, (void**)&s->lpulValues);

		for (int c = 0; c < s->usCount; c++)
			s->lpulValues[c] = values[c];

		pResult->u.dwEventID = WFS_EXEE_CDM_INCOMPLETEDISPENSE;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}


}
void CDM4000::sendWFS_EXEE_CDM_NOTEERROR(HSERVICE hService, short r)
{
	map<HWND, EventReg> events = getEventMap(hService, WFS_EXECUTE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;

		memAlloc->AllocateMore(sizeof(LPUSHORT), pResult, (void**)&pResult->lpBuffer);
		LPUSHORT s = (LPUSHORT)pResult->lpBuffer;
		*s = r;
		
		pResult->u.dwEventID = WFS_EXEE_CDM_NOTEERROR;
		sendMessage(i.first, WFS_EXECUTE_EVENT, (LPARAM)pResult);
	}

}
void CDM4000::sendWFS_SRVE_CDM_ITEMSPRESENTED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SERVICE_EVENT);
	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SRVE_CDM_ITEMSPRESENTED;
		sendMessage(i.first, WFS_SERVICE_EVENT, (LPARAM)pResult);
	}
}
void CDM4000::sendWFS_SRVE_CDM_MEDIADETECTED(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_INPUT_P6(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_SRVE_CDM_DEVICEPOSITION(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_SRVE_CDM_POWER_SAVE_CHANGE(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_EXEE_CDM_INFO_AVAILABLE(HSERVICE hService)
{
	throw "event not supported";

}
void CDM4000::sendWFS_EXEE_CDM_INCOMPLETERETRACT(HSERVICE hService)
{
	throw "event not supported";
}
void CDM4000::sendWFS_SRVE_CDM_SHUTTERSTATUSCHANGED(HSERVICE hService)
{
	throw "event not supported";
}

void CDM4000::execWFS_CMD_CDM_DENOMINATE(REQUESTID reqId) 
{
	throw "command not supported";
}

void CDM4000::execWFS_CMD_CDM_DISPENSE(REQUESTID reqId) 
{
	Xfs::getInstance()->l.debug("execWFS_CMD_CDM_DISPENSE enter");


	checkStatus();

	

	if (status.fwDevice != WFS_CDM_DEVONLINE)
	{
		Xfs::getInstance()->l.debug("status.fwDevice != WFS_CDM_DEVONLINE =="+std::to_string(status.fwDevice));


		requests[reqId]->pResult->hResult = WFS_ERR_HARDWARE_ERROR;
		return;
	}
	if (exchangeActive)
	{
		Xfs::getInstance()->l.debug("exchangeActive");

		requests[reqId]->pResult->hResult = WFS_ERR_CDM_EXCHANGEACTIVE;
		return;
	}
	Xfs::getInstance()->l.debug("LPWFSCDMDISPENSE lpDispense = (LPWFSCDMDISPENSE)requests[reqId]->lpParam; enter");

	LPWFSCDMDISPENSE lpDispense = (LPWFSCDMDISPENSE)requests[reqId]->lpParam;	

	Xfs::getInstance()->l.debug("LPWFSCDMDISPENSE lpDispense = (LPWFSCDMDISPENSE)requests[reqId]->lpParam; exit");
	
	bool tresholdCu0 = false;
	bool tresholdCu1 = false;
	bool tresholdCu2 = false;
	bool tresholdCu3 = false;
	bool tresholdCu4 = false;

	


	char cu0;
	char cu1;
	char cu2;
	char cu3;


	cu0 = lpDispense->lpDenomination->lpulValues[0];
	cu1 = lpDispense->lpDenomination->lpulValues[1];
	cu2 = lpDispense->lpDenomination->lpulValues[2];
	cu3 = lpDispense->lpDenomination->lpulValues[3];

	if (maxDispense < (cu0 + cu1 + cu2 + cu3))
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CDM_TOOMANYITEMS;
		return;
	}


	if (cu0 > cashUnits[0].ulCount||
		cu1 > cashUnits[1].ulCount ||
		cu2 > cashUnits[2].ulCount ||
		cu3 > cashUnits[3].ulCount)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CDM_NOTDISPENSABLE;
		
		return;		
	}
	CDMCBXITEM lpCbxItem;
	for (int i = 0; i < MAX_CASSETTE; i++)
	{
		lpCbxItem.count[i] = 0;
	}
	lpCbxItem.count[0] = cu0;
	lpCbxItem.count[1] = cu1;
	lpCbxItem.count[2] = cu2;
	lpCbxItem.count[3] = cu3;

	CDMMULTIDISPENSE lpResult;	
	  

	Xfs::getInstance()->l.debug("TO DISPENSE 1:"+std::to_string(cu0));
	Xfs::getInstance()->l.debug("TO DISPENSE 2:" + std::to_string(cu1));
	Xfs::getInstance()->l.debug("TO DISPENSE 3:" + std::to_string(cu2));
	Xfs::getInstance()->l.debug("TO DISPENSE 4:" + std::to_string(cu3));
	
	bool res;
		
#ifdef TESTING
	res = true;
	lpResult.count[0] = cu0;
	lpResult.count[1] = cu1;
	lpResult.count[2] = cu2;
	lpResult.count[3] = cu3;
	lpResult.error_cd = 0x30;
#endif

#ifndef TESTING
	res = MFSCommCDM_MultiDispense(&lpCbxItem, &lpResult);
#endif // TESTING	
	
	
	if (lpResult.error_cd == 0x30)
	{
		Xfs::getInstance()->l.debug("Dispensed with no_error");
	}
	else
	{
		Xfs::getInstance()->l.debug("Dispensed with error:" + std::to_string(lpResult.error_cd));
		Xfs::getInstance()->l.debug("Dispensed with reject:" + std::to_string(lpResult.reject_cd));
	}


	cu0 = lpResult.count[0];
	cu1 = lpResult.count[1];
	cu2 = lpResult.count[2];
	cu3 = lpResult.count[3];
	
	Xfs::getInstance()->l.debug("DISPENSED 1:" + std::to_string(cu0));
	Xfs::getInstance()->l.debug("DISPENSED 2:" + std::to_string(cu1));
	Xfs::getInstance()->l.debug("DISPENSED 3:" + std::to_string(cu2));
	Xfs::getInstance()->l.debug("DISPENSED 4:" + std::to_string(cu3));




	Xfs::getInstance()->l.debug("REQUEST=" + std::to_string(reqId) + "PRESULT="+std::to_string((long)requests[reqId]->pResult));

	memAlloc->AllocateMore(sizeof(WFSCDMDENOMINATION), requests[reqId]->pResult, (void**)&requests[reqId]->pResult->lpBuffer);
	LPWFSCDMDENOMINATION lpDenomination = (LPWFSCDMDENOMINATION)requests[reqId]->pResult->lpBuffer;

	CDMLASTDISPENSE lastDispense;

	MFSCommCDM_LastDispense(&lastDispense);

	//Set counters
	//Rejected
	auto cr0 = lastDispense.last_dispense_count[0] - lastDispense.last_pick_count[0];
	auto cr1 = lastDispense.last_dispense_count[1] - lastDispense.last_pick_count[1];
	auto cr2 = lastDispense.last_dispense_count[2] - lastDispense.last_pick_count[2];
	auto cr3 = lastDispense.last_dispense_count[3] - lastDispense.last_pick_count[3];


	
	cashUnits[0].ulPresentedCount += cu0;
	cashUnits[1].ulPresentedCount += cu1;
	cashUnits[2].ulPresentedCount += cu2;
	cashUnits[3].ulPresentedCount += cu3;


	cashUnits[0].ulDispensedCount += cu0;
	cashUnits[1].ulDispensedCount += cu1;
	cashUnits[2].ulDispensedCount += cu2;
	cashUnits[3].ulDispensedCount += cu3;

	cashUnits[0].ulDispensedCount += cr0;
	cashUnits[1].ulDispensedCount += cr1;
	cashUnits[2].ulDispensedCount += cr2;
	cashUnits[3].ulDispensedCount += cr3;

	cashUnits[0].ulRejectCount += cr0;
	cashUnits[1].ulRejectCount += cr1;
	cashUnits[2].ulRejectCount += cr2;
	cashUnits[3].ulRejectCount += cr3;


	cashUnits[0].ulCount -= cu0;
	cashUnits[1].ulCount -= cu1;
	cashUnits[2].ulCount -= cu2;
	cashUnits[3].ulCount -= cu3;

	cashUnits[0].ulCount -= cr0;
	cashUnits[1].ulCount -= cr1;
	cashUnits[2].ulCount -= cr2;
	cashUnits[3].ulCount -= cr3;

	/////
	phcu[0].ulPresentedCount += cu0;
	phcu[1].ulPresentedCount += cu1;
	phcu[2].ulPresentedCount += cu2;
	phcu[3].ulPresentedCount += cu3;


	phcu[0].ulDispensedCount += cu0;
	phcu[1].ulDispensedCount += cu1;
	phcu[2].ulDispensedCount += cu2;
	phcu[3].ulDispensedCount += cu3;

	phcu[0].ulDispensedCount += cr0;
	phcu[1].ulDispensedCount += cr1;
	phcu[2].ulDispensedCount += cr2;
	phcu[3].ulDispensedCount += cr3;

	phcu[0].ulRejectCount += cr0;
	phcu[1].ulRejectCount += cr1;
	phcu[2].ulRejectCount += cr2;
	phcu[3].ulRejectCount += cr3;


	phcu[0].ulCount -= cu0;
	phcu[1].ulCount -= cu1;
	phcu[2].ulCount -= cu2;
	phcu[3].ulCount -= cu3;

	phcu[0].ulCount -= cr0;
	phcu[1].ulCount -= cr1;
	phcu[2].ulCount -= cr2;
	phcu[3].ulCount -= cr3;
	   
	setStoredData();
	   	  
	lpDenomination->cCurrencyID[0] = 'E';
	lpDenomination->cCurrencyID[1] = 'U';
	lpDenomination->cCurrencyID[2] = 'R';
	lpDenomination->ulCashBox = 0;
	lpDenomination->ulAmount = 0;
	lpDenomination->usCount = 5;
	
	memAlloc->AllocateMore(sizeof(long)*lpDenomination->usCount, requests[reqId]->pResult, (void**)&lpDenomination->lpulValues);
	
	lpDenomination->lpulValues[0] = cu0;
	lpDenomination->lpulValues[1] = cu1;
	lpDenomination->lpulValues[2] = cu2;
	lpDenomination->lpulValues[3] = cu3;

	checkStatus();
	
	

	if (cu0)updateCashUnitStatusFromCountersAndRaiseEvent(requests[reqId]->hservice,0);
	if (cu1)updateCashUnitStatusFromCountersAndRaiseEvent(requests[reqId]->hservice,1);
	if (cu2)updateCashUnitStatusFromCountersAndRaiseEvent(requests[reqId]->hservice,2);
	if (cu3)updateCashUnitStatusFromCountersAndRaiseEvent(requests[reqId]->hservice,3);
	updateCashUnitStatusFromCountersAndRaiseEvent(requests[reqId]->hservice,4);
	
	
}

void CDM4000::updateCashUnitStatusFromCountersAndRaiseEvent(HSERVICE hService, int n)
{
	auto lastStatus = cashUnits[n].usStatus;
	updateCashUnitStatusFromCounters(n);
	if (lastStatus == cashUnits[n].usStatus)return;	

	sendWFS_USRE_CDM_CASHUNITTHRESHOLD(hService, n);
}

void CDM4000::updateCashUnitStatusFromCounters(int n)
{
	cashUnits[n].usStatus = WFS_CDM_STATCUOK;

	if (cashUnits[n].usType == WFS_CDM_TYPEREJECTCASSETTE)
	{
		if (cashUnits[n].ulCount > cashUnits[n].ulMaximum)
		{
			cashUnits[n].usStatus = WFS_CDM_STATCUHIGH;
			phcu[n].usPStatus = WFS_CDM_STATCUHIGH;
			return;
		}
	}

	if (cashUnits[n].ulCount <= 0)
	{
		cashUnits[n].usStatus = WFS_CDM_STATCUEMPTY; 
		phcu[n].usPStatus = WFS_CDM_STATCUEMPTY;
		return;
	}

	if (cashUnits[n].ulMinimum)
	{
		if (cashUnits[n].ulCount < cashUnits[n].ulMinimum)
		{
			cashUnits[n].usStatus = WFS_CDM_STATCULOW;
			phcu[n].usPStatus = WFS_CDM_STATCULOW;
			return;
		}
	}
	else
	{
		if (nearEndHardware[n])
		{
			cashUnits[n].usStatus = WFS_CDM_STATCULOW;
			phcu[n].usPStatus = WFS_CDM_STATCULOW;
			return;
		}
	}
}


void CDM4000::execWFS_CMD_CDM_COUNT(REQUESTID reqId){}
void CDM4000::execWFS_CMD_CDM_PRESENT(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_REJECT(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_RETRACT(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_OPEN_SHUTTER(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_CLOSE_SHUTTER(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_SET_TELLER_INFO(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_SET_CASH_UNIT_INFO(REQUESTID reqId)
{
	LPWFSCDMCUINFO lpCUInfo = (LPWFSCDMCUINFO)requests[reqId]->lpParam;

	for (int i = 0; i < lpCUInfo->usCount; i++)
	{
	
		if (lpCUInfo->lppList[i]->lpszCashUnitName)
		{
			std::string cname(lpCUInfo->lppList[i]->lpszCashUnitName);
			this->cuNames[i] = cname;
		}

		unsigned long ulInitialCount = lpCUInfo->lppList[i]->ulInitialCount;
		unsigned long ulCount = lpCUInfo->lppList[i]->ulCount;
		unsigned long ulRejectCount = lpCUInfo->lppList[i]->ulRejectCount;
		unsigned long ulDispensedCount = lpCUInfo->lppList[i]->ulDispensedCount;
		unsigned long ulPresentedCount = lpCUInfo->lppList[i]->ulPresentedCount;
		unsigned long ulRetractedCount = lpCUInfo->lppList[i]->ulRetractedCount;

		if (lpCUInfo->lppList[i]->lppPhysical&&lpCUInfo->lppList[i]->lppPhysical[0])
		{
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulInitialCount)ulInitialCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulInitialCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulCount)ulCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulRejectCount)ulRejectCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulRejectCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulDispensedCount)ulDispensedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulDispensedCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulPresentedCount)ulPresentedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulPresentedCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulRetractedCount)ulRetractedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulRetractedCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->lpPhysicalPositionName)
			{
				std::string cname(lpCUInfo->lppList[i]->lppPhysical[0]->lpPhysicalPositionName);
				this->phcuPositions[i] = cname;
			}
		}
		
		this->cashUnits[i].bAppLock = lpCUInfo->lppList[i]->bAppLock;
		this->cashUnits[i].ulInitialCount = ulInitialCount;
		this->cashUnits[i].ulCount = ulCount;
		this->cashUnits[i].ulRejectCount = ulRejectCount;
		this->cashUnits[i].ulDispensedCount = ulDispensedCount;
		this->cashUnits[i].ulPresentedCount = ulPresentedCount;
		this->cashUnits[i].ulRetractedCount = ulRetractedCount;
		
		this->phcu[i].ulInitialCount = ulInitialCount;
		this->phcu[i].ulCount = ulCount;
		this->phcu[i].ulRejectCount = ulRejectCount;
		this->phcu[i].ulDispensedCount = ulDispensedCount;
		this->phcu[i].ulPresentedCount = ulPresentedCount;
		this->phcu[i].ulRetractedCount = ulRetractedCount;			   
	}
	setStoredData();

}
void CDM4000::execWFS_CMD_CDM_START_EXCHANGE(REQUESTID reqId)
{
	LPWFSCDMSTARTEX lpStartEx = (LPWFSCDMSTARTEX)requests[reqId]->lpParam;
	if (exchangeActive)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CDM_EXCHANGEACTIVE;
		return;
	}
	for (int i = 0; i < lpStartEx->usCount; i++)
	{		
		if (lpStartEx->lpusCUNumList[i] > this->cashUnits.size())
		{
			requests[reqId]->pResult->hResult = WFS_ERR_CDM_CASHUNITERROR;
			return;
		}
	}
	exchangeActive = true;

	memAlloc->AllocateMore(sizeof(WFSCDMCUINFO), requests[reqId]->pResult, (void**)&requests[reqId]->pResult->lpBuffer);

	LPWFSCDMCUINFO lpCUInfo = (LPWFSCDMCUINFO)requests[reqId]->pResult->lpBuffer;
	lpCUInfo->usTellerID = 0;
	lpCUInfo->usCount = cashUnits.size();
	memAlloc->AllocateMore(4* lpCUInfo->usCount, requests[reqId]->pResult, (void**)&lpCUInfo->lppList);
	for (int i = 0; i < cashUnits.size(); i++)
	{
		lpCUInfo->lppList[i] = constructLPWFSCDMCASHUNIT(requests[reqId]->pResult, i);
	}

}
void CDM4000::execWFS_CMD_CDM_END_EXCHANGE(REQUESTID reqId)
{
	if (!exchangeActive)
	{
		requests[reqId]->pResult->hResult = WFS_ERR_CDM_NOEXCHANGEACTIVE;
		return;
	}
	LPWFSCDMCUINFO lpCUInfo = (LPWFSCDMCUINFO)requests[reqId]->lpParam;

	for (int i = 0; i < lpCUInfo->usCount; i++)
	{

		unsigned long ulInitialCount = lpCUInfo->lppList[i]->ulInitialCount;
		unsigned long ulCount = lpCUInfo->lppList[i]->ulCount;
		unsigned long ulRejectCount = lpCUInfo->lppList[i]->ulRejectCount;
		unsigned long ulDispensedCount = lpCUInfo->lppList[i]->ulDispensedCount;
		unsigned long ulPresentedCount = lpCUInfo->lppList[i]->ulPresentedCount;
		unsigned long ulRetractedCount = lpCUInfo->lppList[i]->ulRetractedCount;

		if (lpCUInfo->lppList[i]->lppPhysical&&lpCUInfo->lppList[i]->lppPhysical[0])
		{
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulInitialCount)ulInitialCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulInitialCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulCount)ulCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulRejectCount)ulRejectCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulRejectCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulDispensedCount)ulDispensedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulDispensedCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulPresentedCount)ulPresentedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulPresentedCount;
			if (lpCUInfo->lppList[i]->lppPhysical[0]->ulRetractedCount)ulRetractedCount = lpCUInfo->lppList[i]->lppPhysical[0]->ulRetractedCount;
		}

		this->cashUnits[i].bAppLock = lpCUInfo->lppList[i]->bAppLock;
		this->cashUnits[i].ulInitialCount = ulInitialCount;
		this->cashUnits[i].ulCount = ulCount;
		this->cashUnits[i].ulRejectCount = ulRejectCount;
		this->cashUnits[i].ulDispensedCount = ulDispensedCount;
		this->cashUnits[i].ulPresentedCount = ulPresentedCount;
		this->cashUnits[i].ulRetractedCount = ulRetractedCount;

		this->phcu[i].ulInitialCount = ulInitialCount;
		this->phcu[i].ulCount = ulCount;
		this->phcu[i].ulRejectCount = ulRejectCount;
		this->phcu[i].ulDispensedCount = ulDispensedCount;
		this->phcu[i].ulPresentedCount = ulPresentedCount;
		this->phcu[i].ulRetractedCount = ulRetractedCount;
	}
	setStoredData();
}
void CDM4000::execWFS_CMD_CDM_OPEN_SAFE_DOOR(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_CALIBRATE_CASH_UNIT(REQUESTID reqId){}
void CDM4000::execWFS_CMD_CDM_SET_MIX_TABLE(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_RESET(REQUESTID reqId)
{
	Xfs::getInstance()->l.debug("execWFS_CMD_CDM_RESET");

	bool b = MFSCommCDM_Reset();

	Xfs::getInstance()->l.debug("execWFS_CMD_CDM_RESET="+std::to_string(b));

}
void CDM4000::execWFS_CMD_CDM_TEST_CASH_UNITS(REQUESTID reqId){}
void CDM4000::execWFS_CMD_CDM_SET_GUIDANCE_LIGHT(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_POWER_SAVE_CONTROL(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_PREPARE_DISPENSE(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_SET_BLACKLIST(REQUESTID reqId)
{
	throw "command not supported";
}
void CDM4000::execWFS_CMD_CDM_SYNCHRONIZE_COMMAND(REQUESTID reqId)
{
	throw "command not supported";
}


void CDM4000::getWFS_INF_CDM_STATUS(REQUESTID reqId)
{
	checkStatus();
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCDMSTATUS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCDMSTATUS s = (LPWFSCDMSTATUS)requests[reqId]->pResult->lpBuffer;

	s->fwDevice = status.fwDevice;
	s->fwSafeDoor = status.fwSafeDoor;
	s->fwDispenser = status.fwDispenser;
	s->fwIntermediateStacker = status.fwIntermediateStacker;

	this->memAlloc->AllocateMore(4*2, requests[reqId]->pResult, (void **)&s->lppPositions);
	s->lppPositions[1] = 0;
	this->memAlloc->AllocateMore(sizeof(WFSCDMOUTPOS), requests[reqId]->pResult, (void **)&s->lppPositions[0]);
	s->lppPositions[0]->fwJammedShutterPosition = WFS_CDM_SHUTTERPOS_NOTSUPPORTED;
	s->lppPositions[0]->fwPosition = WFS_CDM_POSFRONT;
	s->lppPositions[0]->fwPositionStatus = WFS_CDM_PSNOTSUPPORTED;
	s->lppPositions[0]->fwShutter = WFS_CDM_SHTNOTSUPPORTED;
	s->lppPositions[0]->fwTransport = WFS_CDM_TPNOTSUPPORTED;
	s->lppPositions[0]->fwTransportStatus = WFS_CDM_TPSTATNOTSUPPORTED;

	s->lpszExtra = 0;

	memAlloc->AllocateMore(this->errLine.size() + 2, requests[reqId]->pResult,(void **) &s->lpszExtra);
	strcpy(s->lpszExtra, this->errLine.data());
	s->lpszExtra[this->errLine.size()] = 0;
	s->lpszExtra[this->errLine.size()+1] = 0;
	for(int i = 0;i< WFS_CDM_GUIDLIGHTS_SIZE;i++)
		s->dwGuidLights[i] = status.dwGuidLights[i];
	s->wDevicePosition;
	s->usPowerSaveRecoveryTime;
	s->wAntiFraudModule;
}
void CDM4000::getWFS_INF_CDM_CAPABILITIES(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCDMCAPS), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCDMCAPS s = (LPWFSCDMCAPS)requests[reqId]->pResult->lpBuffer;
	s->wClass = caps.wClass;
	s->fwType = caps.fwType;
	s->wMaxDispenseItems = caps.wMaxDispenseItems;
	s->bCompound = caps.bCompound;
	s->bShutter  = caps.bShutter;
	s->bShutterControl = caps.bShutterControl;
	s->fwRetractAreas = caps.fwRetractAreas;
	s->fwRetractTransportActions = caps.fwRetractTransportActions;
	s->fwRetractStackerActions = caps.fwRetractStackerActions;
	s->bSafeDoor = caps.bSafeDoor;
	s->bCashBox = caps.bCashBox;
	s->bIntermediateStacker = caps.bIntermediateStacker;
	s->bItemsTakenSensor = caps.bItemsTakenSensor;
	s->fwPositions = caps.fwPositions;
	s->fwMoveItems = caps.fwMoveItems;
	s->fwExchangeType = caps.fwExchangeType;
	s->lpszExtra = 0;
	for(int i = 0;i< WFS_CDM_GUIDLIGHTS_SIZE;i++)
		s->dwGuidLights[i] = caps.dwGuidLights[i];
	s->bPowerSaveControl = caps.bPowerSaveControl;
	s->bPrepareDispense = caps.bPrepareDispense;
	s->bAntiFraudModule = caps.bAntiFraudModule;
	s->dwItemInfoTypes = caps.dwItemInfoTypes;
	s->bBlacklist = caps.bBlacklist;
	s->lpdwSynchronizableCommands = 0;
}
void CDM4000::getWFS_INF_CDM_CASH_UNIT_INFO(REQUESTID reqId)
{
	
	auto a = this->memAlloc->AllocateMore(sizeof(WFSCDMCUINFO), requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCDMCUINFO s = (LPWFSCDMCUINFO)requests[reqId]->pResult->lpBuffer;
	s->usCount = 5;
	s->usTellerID = 0;
	
	
	a = this->memAlloc->AllocateMore(4*s->usCount, requests[reqId]->pResult, (void **)&s->lppList);
	for (int i = 0; i < s->usCount; i++)
	{
		s->lppList[i] = constructLPWFSCDMCASHUNIT(requests[reqId]->pResult, i);
	}

}
void CDM4000::getWFS_INF_CDM_TELLER_INFO(REQUESTID reqId){}
void CDM4000::getWFS_INF_CDM_CURRENCY_EXP(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(LPWFSCDMCURRENCYEXP)*2, requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);
	LPWFSCDMCURRENCYEXP *s = (LPWFSCDMCURRENCYEXP*)requests[reqId]->pResult->lpBuffer;
	s[1] = 0;
	a = this->memAlloc->AllocateMore(sizeof(WFSCDMCURRENCYEXP), requests[reqId]->pResult, (void **)&s[0]);
	
	
	
	s[0]->cCurrencyID[0] = 'E';
	s[0]->cCurrencyID[1] = 'U';
	s[0]->cCurrencyID[2] = 'R';

	s[0]->sExponent = 0;

}
void CDM4000::getWFS_INF_CDM_MIX_TYPES(REQUESTID reqId)
{
	auto a = this->memAlloc->AllocateMore(sizeof(LPWFSCDMMIXTYPE)*2, requests[reqId]->pResult, (void **)&requests[reqId]->pResult->lpBuffer);


	LPWFSCDMMIXTYPE *lppMixTypes = (LPWFSCDMMIXTYPE*)requests[reqId]->pResult->lpBuffer;
	a = this->memAlloc->AllocateMore(sizeof(WFSCDMMIXTYPE) , requests[reqId]->pResult, (void **)&lppMixTypes[0]);

	lppMixTypes[1] = 0;
}
void CDM4000::getWFS_INF_CDM_MIX_TABLE(REQUESTID reqId)
{
	requests[reqId]->pResult->hResult = WFS_ERR_CDM_INVALIDMIXNUMBER;
}
void CDM4000::getWFS_INF_CDM_PRESENT_STATUS(REQUESTID reqId){}
void CDM4000::getWFS_INF_CDM_GET_ITEM_INFO(REQUESTID reqId){}
void CDM4000::getWFS_INF_CDM_GET_BLACKLIST(REQUESTID reqId){}
void CDM4000::getWFS_INF_CDM_GET_ALL_ITEMS_INFO(REQUESTID reqId){}

void CDM4000::initCaps()
{
	this->caps.wClass = WFS_SERVICE_CLASS_CDM;
	this->caps.fwType = WFS_CDM_SELFSERVICEBILL;
	this->caps.wMaxDispenseItems = 50;
	this->caps.bCompound = false;
	this->caps.bShutter = false;
	this->caps.bShutterControl = true;
	this->caps.fwRetractAreas = WFS_CDM_RA_NOTSUPP;
	this->caps.fwRetractTransportActions = WFS_CDM_NOTSUPP;
	this->caps.fwRetractStackerActions;
	this->caps.bSafeDoor = false;
	this->caps.bCashBox = false;
	this->caps.bIntermediateStacker = false;
	this->caps.bItemsTakenSensor = false;
	this->caps.fwPositions = WFS_CDM_POSFRONT;
	this->caps.fwMoveItems = 0;
	this->caps.fwExchangeType = WFS_CDM_EXBYHAND;
	this->caps.lpszExtra = 0;
	
	for (int i = 0; i < WFS_CDM_GUIDLIGHTS_SIZE; i++)
		caps.dwGuidLights[i] = 0;
	this->caps.bPowerSaveControl = false;
	this->caps.bPrepareDispense = false;
	this->caps.bAntiFraudModule = false;
	this->caps.dwItemInfoTypes = 0;
	this->caps.bBlacklist = false;;
	this->caps.lpdwSynchronizableCommands = 0;
	
}
void CDM4000::initStatus()
{
	this->status.fwDevice = WFS_CDM_DEVOFFLINE;
	this->status.fwSafeDoor = WFS_CDM_DOORNOTSUPPORTED;
	this->status.fwDispenser = WFS_CDM_DISPCUUNKNOWN;
	this->status.fwIntermediateStacker = WFS_CDM_ISNOTSUPPORTED;
	this->status.lppPositions = 0;
	this->status.lpszExtra = 0;
	for (int i = 0; i < WFS_CDM_GUIDLIGHTS_SIZE; i++)
			status.dwGuidLights[i] = 0;

	this->status.wDevicePosition = WFS_CDM_DEVICEPOSNOTSUPP;
	this->status.usPowerSaveRecoveryTime = 0;
	this->status.wAntiFraudModule = false;

}

void CDM4000::initCashUnits()
{
	for (int i = 0; i < cashUnits.size(); i++)
	{
		cashUnits[i].usStatus = WFS_CDM_STATCUOK;
		phcu[i].usPStatus = WFS_CDM_STATCUOK;
	}
		
}

LPWFSCDMCASHUNIT  CDM4000::constructLPWFSCDMCASHUNIT(LPVOID pResult, int n)
{
	auto cu = cashUnits[n];
	auto hcu = phcu[n];
	LPWFSCDMCASHUNIT lpcu;
	
	auto a = memAlloc->AllocateMore(sizeof(WFSCDMCASHUNIT), pResult, (void**)&lpcu);
	
	lpcu->usNumber = cu.usNumber;
	lpcu->usType = cu.usType;

	memAlloc->AllocateMore(cuNames[n].size()+1, pResult, (void**)&lpcu->lpszCashUnitName);
	lpcu->lpszCashUnitName[cuNames[n].size()] = 0;
	strcpy(lpcu->lpszCashUnitName, cuNames[n].data());
	//lpcu->lpszCashUnitName = 0;
	
	
	for(int c = 0;c<5;c++)
		lpcu->cUnitID[c] = cu.cUnitID[c];
	for (int c = 0; c < 3; c++)
		lpcu->cCurrencyID[c] = cu.cCurrencyID[c];
	lpcu->ulValues = cu.ulValues;
	lpcu->ulInitialCount = cu.ulInitialCount;
	lpcu->ulCount = cu.ulCount; 
	lpcu->ulRejectCount = cu.ulRejectCount;
	lpcu->ulMinimum = cu.ulMinimum;
	lpcu->ulMaximum = cu.ulMaximum;
	lpcu->bAppLock = cu.bAppLock;
	lpcu->usStatus = cu.usStatus;
	lpcu->usNumPhysicalCUs = 1;	
	
	lpcu->ulDispensedCount = cu.ulDispensedCount;
	lpcu->ulPresentedCount = cu.ulPresentedCount;
	lpcu->ulRetractedCount = cu.ulRetractedCount;

	a = memAlloc->AllocateMore(4, pResult, (void**)&lpcu->lppPhysical);
	a = memAlloc->AllocateMore(sizeof(WFSCDMPHCU), pResult, (void**)&lpcu->lppPhysical[0]);
	

	//lpcu->lppPhysical[0]->lpPhysicalPositionName = 0;
	memAlloc->AllocateMore(phcuPositions[n].size() + 1, pResult, (void**)&lpcu->lppPhysical[0]->lpPhysicalPositionName);
	lpcu->lppPhysical[0]->lpPhysicalPositionName[phcuPositions[n].size()] = 0;
	strcpy(lpcu->lppPhysical[0]->lpPhysicalPositionName, phcuPositions[n].data());


	for(int c = 0;c<5;c++)
		lpcu->lppPhysical[0]->cUnitID[c] = cu.cUnitID[c];
	lpcu->lppPhysical[0]->ulInitialCount = hcu.ulInitialCount;
	lpcu->lppPhysical[0]->ulCount = hcu.ulCount;
	lpcu->lppPhysical[0]->ulRejectCount = hcu.ulRejectCount;
	lpcu->lppPhysical[0]->ulMaximum = hcu.ulMaximum;
	lpcu->lppPhysical[0]->usPStatus = hcu.usPStatus;
	lpcu->lppPhysical[0]->bHardwareSensor = hcu.bHardwareSensor;
	lpcu->lppPhysical[0]->ulDispensedCount = hcu.ulDispensedCount;
	lpcu->lppPhysical[0]->ulPresentedCount = hcu.ulPresentedCount;
	lpcu->lppPhysical[0]->ulRetractedCount = hcu.ulRetractedCount;

	return lpcu;
}



std::vector<string> CDM4000::split(std::string text)
{
	std::vector<string> r;
	boost::split(r, text, [](char c) {return c == ','; });
	return r;
}

void CDM4000::getStoredData()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);
	int checkCount = 0;
	auto units = pt.get<std::string>("units", "D,D,D,D,R");
	auto sunits = split(units);
	checkCount = sunits.size();

	auto unitid = pt.get<std::string>("unitid", "0,0,0,0,0");
	auto sunitid = split(unitid);
	if (checkCount != sunitid.size())
	{
		Xfs::getInstance()->l.debug("unitid parameter configuration error");
		throw "getStoredData";			
	}
		
	auto unitnames = pt.get<std::string>("unitnames", "0,0,0,0,0");
	auto sunitnames = split(unitnames);
	if (checkCount != sunitnames.size())
	{
		Xfs::getInstance()->l.debug("unitnames parameter configuration error");
		throw "getStoredData";
	}

	auto unitpositions = pt.get<std::string>("unitpositions", "0,0,0,0,0");
	auto sunitpositions = split(unitpositions);
	if (checkCount != sunitpositions.size())
	{
		Xfs::getInstance()->l.debug("unitpositions parameter configuration error");
		throw "getStoredData";
	}



	auto unitvalues = pt.get<std::string>("unitvalues", "0,0,0,0,0");
	auto sunitvalues = split(unitvalues);
	if (checkCount != sunitvalues.size())
	{
		Xfs::getInstance()->l.debug("unitvalues parameter configuration error");
		throw "getStoredData";
	}



	auto unitinitialcount = pt.get<std::string>("unitinitialcount", "0,0,0,0,0");
	auto sunitinitialcount = split(unitinitialcount);
	if (checkCount != sunitinitialcount.size())
	{
		Xfs::getInstance()->l.debug("unitinitialcount parameter configuration error");
		throw "getStoredData";
	}

	auto unitcount = pt.get<std::string>("unitcount", "0,0,0,0,0");
	auto sunitcount = split(unitcount);
	if (checkCount != sunitcount.size())
	{
		Xfs::getInstance()->l.debug("unitcount parameter configuration error");
		throw "getStoredData";
	}


	auto unitrejected = pt.get<std::string>("unitrejected", "0,0,0,0,0");
	auto sunitrejected = split(unitrejected);
	if (checkCount != sunitrejected.size())
	{
		Xfs::getInstance()->l.debug("unitrejected parameter configuration error");
		throw "getStoredData";
	}

		
	auto unitminimum = pt.get<std::string>("unitminimum", "0,0,0,0,0");
	auto sunitminimum = split(unitminimum);
	if (checkCount != sunitminimum.size())
	{
		Xfs::getInstance()->l.debug("unitminimum parameter configuration error");
		throw "getStoredData";
	}


	auto unitmaximum = pt.get<std::string>("unitmaximum", "0,0,0,0,0");
	auto sunitmaximum = split(unitmaximum);
	if (checkCount != sunitmaximum.size())
	{
		Xfs::getInstance()->l.debug("unitmaximum parameter configuration error");
		throw "getStoredData";
	}

	auto unitdispensed = pt.get<std::string>("unitdispensed", "0,0,0,0,0");
	auto sunitdispensed = split(unitdispensed);
	if (checkCount != sunitdispensed.size())
	{
		Xfs::getInstance()->l.debug("unitdispensed parameter configuration error");
		throw "getStoredData";
	}


	auto unitpresented = pt.get<std::string>("unitpresented", "0,0,0,0,0");
	auto sunitpresented = split(unitpresented);
	if (checkCount != sunitpresented.size())
	{
		Xfs::getInstance()->l.debug("unitpresented parameter configuration error");
		throw "getStoredData";
	}


	auto unitretracted = pt.get<std::string>("unitretracted", "0,0,0,0,0");
	auto sunitretracted = split(unitretracted);
	
	if (checkCount != sunitretracted.size())
	{
		Xfs::getInstance()->l.debug("unitretracted parameter configuration error");
		throw "getStoredData";
	}
	cuNames.clear();
	phcuPositions.clear();

	for (int i = 0; i < checkCount; i++)
	{
		cuNames.push_back(sunitnames[i]);
		phcuPositions.push_back(sunitpositions[i]);

		WFSCDMCASHUNIT cu;

		cu.usNumber = i+1;
		cu.usType = sunits[i] == "D"? WFS_CDM_TYPEBILLCASSETTE: sunits[i] == "R"? WFS_CDM_TYPEREJECTCASSETTE: WFS_CDM_TYPENA;
		cu.lpszCashUnitName = 0;

		if (sunitid[i].size() != 5)
		{

			Xfs::getInstance()->l.debug("unit id parameter configuration error (length)");
			throw "getStoredData";
		}
		
		for (int j = 0; j < 5; j++)
		{
			cu.cUnitID[j] = sunitid[i][j];
		}
		cu.cCurrencyID[0] = 'E';
		cu.cCurrencyID[1] = 'U';
		cu.cCurrencyID[2] = 'R';
		
		cu.ulValues = atoi(sunitvalues[i].data());
		cu.ulInitialCount = atoi(sunitinitialcount[i].data());
		cu.ulCount = atoi(sunitcount[i].data());
		cu.ulRejectCount = atoi(sunitrejected[i].data());
		cu.ulMinimum = atoi(sunitminimum[i].data());
		cu.ulMaximum = atoi(sunitmaximum[i].data());

		cu.ulDispensedCount = atoi(sunitdispensed[i].data());
		cu.ulPresentedCount = atoi(sunitpresented[i].data());
		cu.ulRetractedCount = atoi(sunitretracted[i].data());

		this->cashUnits.push_back(cu);

		WFSCDMPHCU ph;

		ph.ulCount = cu.ulCount;

		ph.lpPhysicalPositionName = 0;
		for (int c = 0; c < 5; c++)
		{
			ph.cUnitID[c] = cu.cUnitID[c];
		}

		
		ph.ulInitialCount = cu.ulInitialCount;
		ph.ulCount = cu.ulCount;
		ph.ulRejectCount = cu.ulRejectCount;
		ph.ulMaximum = cu.ulMaximum;
		
		ph.ulDispensedCount = cu.ulDispensedCount;
		ph.ulPresentedCount = cu.ulPresentedCount;
		ph.ulRetractedCount = cu.ulRetractedCount;

		phcu.push_back(ph);
	}

	this->maxDispense = pt.get<int>("maxdispense", 20);

	for (int i = 0; i < cashUnits.size(); i++)
		updateCashUnitStatusFromCounters(i);

}

void CDM4000::setStoredData()
{
	ptree pt;
	boost::property_tree::read_json(configName, pt);

	std::string unitnames = "";
	std::string unitpositions = "";
	std::string units = "";
	std::string unitid = "";
	std::string unitvalues = "";
	std::string unitinitialcount = "";
	std::string	unitcount = "";
	std::string unitrejected = "";
	std::string unitminimum = "";
	std::string unitmaximum = "";
	std::string unitdispensed = "";
	std::string unitpresented = "";
	std::string unitretracted = "";


	for (int i = 0;i<cashUnits.size();i++)
	{
		bool needComma = (cashUnits.size() - 1 > i);
		auto cu = cashUnits[i];

		unitnames += cuNames[i];
		unitnames += needComma ? "," : "";

		unitpositions += phcuPositions[i];
		unitpositions += needComma ? "," : "";

		units += cu.usType == WFS_CDM_TYPEBILLCASSETTE ? "D" : cu.usType == WFS_CDM_TYPEREJECTCASSETTE ? "R" : "N";
		units += needComma ? "," : "";

		unitid += std::string(cu.cUnitID, 5);
		unitid += needComma ? "," : "";

		unitvalues += std::to_string(cashUnits[i].ulValues);
		unitvalues += needComma ? "," : "";

		unitinitialcount += std::to_string(cashUnits[i].ulInitialCount);
		unitinitialcount += needComma ? "," : "";

		unitcount += std::to_string(cashUnits[i].ulCount);
		unitcount += needComma ? "," : "";

		unitrejected += std::to_string(cashUnits[i].ulRejectCount);
		unitrejected += needComma ? "," : "";

		unitminimum += std::to_string(cashUnits[i].ulMinimum);
		unitminimum += needComma ? "," : "";

		unitmaximum += std::to_string(cashUnits[i].ulMaximum);
		unitmaximum += needComma ? "," : "";

		unitdispensed += std::to_string(cashUnits[i].ulDispensedCount);
		unitdispensed += needComma ? "," : "";
		
		unitpresented += std::to_string(cashUnits[i].ulPresentedCount);
		unitpresented += needComma ? "," : "";

		unitretracted += std::to_string(cashUnits[i].ulRetractedCount);
		unitretracted += needComma ? "," : "";
	}

	pt.put("unitnames", unitnames);
	pt.put("unitpositions", unitpositions);
	pt.put("units", units);
	pt.put("unitid", unitid);
	pt.put("unitvalues", unitvalues);
	pt.put("unitinitialcount", unitinitialcount);
	pt.put("unitcount", unitcount);
	pt.put("unitrejected", unitrejected);
	pt.put("unitminimum", unitminimum);
	pt.put("unitmaximum", unitmaximum);
	pt.put("unitdispensed", unitdispensed);
	pt.put("unitpresented", unitpresented);
	pt.put("unitretracted", unitretracted);

	pt.put("maxdispense", this->maxDispense);

	boost::property_tree::write_json(configName, pt);

}




CDM4000::CDM4000(volatile SharedData* s, unsigned short version, std::string port, int baudRate, std::string lname) :GenericDevice(s, version)
{
	this->configName = lname;
	this->port = atoi(port.data());
	initCaps();
	initStatus();
	exchangeActive = false;
	getStoredData();
	
	//setStoredData();
	initCashUnits();
	bool res = openCdm();
	if (res)
	{
		Xfs::getInstance()->l.debug("CDM OPENED");
	}
	else
		Xfs::getInstance()->l.debug("CDM OPEN FAILED");

	
	
	
	checkStatus();


}


CDM4000::~CDM4000()
{
	
	closeCdm();
}
