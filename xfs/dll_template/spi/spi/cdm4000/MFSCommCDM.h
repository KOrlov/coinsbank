#pragma once
#pragma comment(lib, "CDM4000\\MFSCommCDM.lib" )
#include "stdafx.h"
#define MFSCOMMCDMDLL_API   __declspec(dllimport)

// -----------------------------------
// 			Exported Data
// -----------------------------------
#define MAX_CASSETTE				6

typedef struct _cdm_cfg_status {
	BYTE major_no;
	BYTE minor_no;
	BYTE cbx_type;
} CDMCFGSTATUS, *LPCDMCFGSTATUS;

typedef struct _cdm_status {
	BYTE error_cd;
	BYTE reject_cd;

//	BYTE sensor[7];
	BYTE sensor[8];
} CDMSTATUS, *LPCDMSTATUS;

typedef struct _cdm_cbx_item {
	BYTE count[MAX_CASSETTE];
} CDMCBXITEM, *LPCDMCBXITEM;

typedef struct _cdm_multi_dispense {
	BYTE error_cd;
	BYTE reject_cd;
//	BYTE sensor[7];
	BYTE sensor[8];

	BYTE count[MAX_CASSETTE];
} CDMMULTIDISPENSE, *LPCDMMULTIDISPENSE;

typedef struct _cdm_last_dispense {
	BYTE last_cmd;
	BYTE last_error_cd;
	BYTE last_cbx;

	BYTE last_dispense_count[MAX_CASSETTE];
	BYTE last_divert_count[MAX_CASSETTE];
	BYTE last_pick_count[MAX_CASSETTE];
} CDMLASTDISPENSE, *LPCDMLASTDISPENSE;

typedef struct _cdm_diagnostic {
	BYTE error_cd;
	BYTE reject_cd;

//	BYTE sensor[7];
	BYTE sensor[8];
	CHAR result_msg[200];
} CDMDIAGNOSTIC, *LPCDMDIAGNOSTIC;

typedef struct _cdm_set_bill_thickness {
	BYTE cbx_no;
	BYTE thickness;
	BYTE margin;
} CDMSETBILLTHICKNESS, *LPCDMSETBILLTHICKNESS;

typedef struct _cdm_get_bill_thickness {
	BYTE error_cd;
	BYTE thickness;
	BYTE margin;
} CDMGETBILLTHICKNESS, *LPCDMGETBILLTHICKNESS;

typedef struct _cdm_set_bill_size {
	BYTE cbx_no;
	BYTE size;
	BYTE margin;
} CDMSETBILLSIZE, *LPCDMSETBILLSIZE;

typedef struct _cdm_get_bill_size {
	BYTE error_cd;
	BYTE size;
	BYTE margin;
} CDMGETBILLSIZE, *LPCDMGETBILLSIZE;

typedef struct _cdm_learn {
	BYTE error_cd;
	BYTE reject_cd;

	BYTE size_avg;
	BYTE thickness_avg;
} CDMLEARN, *LPCDMLEARN;

// -----------------------------------
// 			Exported Function
// -----------------------------------

MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_OpenPort(HWND hWnd, int nPortNum);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_ClosePort();

MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_Reset();
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_CfgStatus(LPCDMCFGSTATUS lpCfgStatus);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_Status(LPCDMSTATUS lpStatus);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_MultiDispense(LPCDMCBXITEM lpCbxItem, LPCDMMULTIDISPENSE lpResult);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_LastDispense(LPCDMLASTDISPENSE lpResult);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_Diagnostic(LPCDMDIAGNOSTIC lpResult);

MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_SetBillThickness(LPCDMSETBILLTHICKNESS lpSet, LPBYTE lpbyErrorCd);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_GetBillThickness(BYTE byCbxNo, LPCDMGETBILLTHICKNESS lpGet);

MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_SetBillSize(LPCDMSETBILLSIZE lpSet, LPBYTE lpbyErrorCd);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_GetBillSize(BYTE byCbxNo, LPCDMGETBILLSIZE lpGet);
MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_LearnBill(BYTE byCbxNo, BYTE byItemCount, LPCDMLEARN lpResult);

MFSCOMMCDMDLL_API BOOL __stdcall MFSCommCDM_GetDLLVersion(LPBYTE major_num,LPBYTE minor_num);

/*MFSCOMMCDMDLL_API BOOL MFSCommCDM_OpenPort(HWND hWnd, UINT nPortNum);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_ClosePort();

MFSCOMMCDMDLL_API BOOL MFSCommCDM_Reset();
MFSCOMMCDMDLL_API BOOL MFSCommCDM_CfgStatus(LPCDMCFGSTATUS lpCfgStatus);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_Status(LPCDMSTATUS lpStatus);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_MultiDispense(LPCDMCBXITEM lpCbxItem, LPCDMMULTIDISPENSE lpResult);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_LastDispense(LPCDMLASTDISPENSE lpResult);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_Diagnostic(LPCDMDIAGNOSTIC lpResult);

MFSCOMMCDMDLL_API BOOL MFSCommCDM_SetBillThickness(LPCDMSETBILLTHICKNESS lpSet, LPBYTE lpbyErrorCd);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_GetBillThickness(BYTE byCbxNo, LPCDMGETBILLTHICKNESS lpGet);

MFSCOMMCDMDLL_API BOOL MFSCommCDM_SetBillSize(LPCDMSETBILLSIZE lpSet, LPBYTE lpbyErrorCd);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_GetBillSize(BYTE byCbxNo, LPCDMGETBILLSIZE lpGet);
MFSCOMMCDMDLL_API BOOL MFSCommCDM_LearnBill(BYTE byCbxNo, BYTE byItemCount, LPCDMLEARN lpResult);

MFSCOMMCDMDLL_API BOOL MFSCommCDM_GetDLLVersion(LPBYTE major_num,LPBYTE minor_num);*/