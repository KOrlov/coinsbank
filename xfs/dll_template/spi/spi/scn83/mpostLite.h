#pragma once
#include "stdafx.h"
#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp> 
#include <boost/signals2.hpp>

using namespace boost;
using namespace boost::signals2;

namespace Mpost {

	enum BanknoteClassification :char
	{
		SpecDisabled = 0x00,// = Classification is disabled
		Class1 = 0x01,// = Class 1 (unidentified banknote)
		Class2 = 0x02,// = Class 2 (suspected counterfeit)
		Class3 = 0x03,// = Class 3 (suspected zero value note)
		Class4 = 0x04// = Class 4 (genuine banknote)
	};
	struct NoteType
	{
		int index;
		char iso[3];
		int base;		
		int exp;
	};
	struct Note
	{
		bool isPresent = false;
		NoteType noteType;
		char orientation;
		char  type;
		char series;
		char compatibility;
		char version;
		char classification;
	};

	

	enum  DeviceState:char
	{
		
		Idling = 1, //The device is idling.Not processing a document.
		Accepting = 2,// The device is drawing in a document.
		EscrowedState = 4,// There is a valid document in escrow.
		Stacking = 8, // The device is stacking a document.
		StackedEvent =  16,// The device has stacked a document.
		Returning = 32,// The device is returning a document to the customer.
		ReturnedEvent = 64,// The device has returned a document to the customer.
	};

	enum  DeviceSatus:char
	{
		
		Cheated = 1,// The device has detected conditions consistent with an attempt to fraud the system.
		Rejected = 2,// The document presented to the device could not be validated and was	returned to the customer.
		Jammed = 4,// The path is blocked and the device has been unable to resolve the	issue.Intervention is required.
		StackerFull = 8,// The cash box is full of documents and no more may be accepted.The device will be out of service until the issue is corrected.
		CassetteAttached = 16, 		
		Paused = 32,//The customer is attempting to feed another note while the previous  note is still being processed.The customer must remove the note to permit processing to continue.
		Calibration = 64
	};

	enum  AdditionalStatus:char
	{
		PowerUp = 1,
		InvalidCommand = 2,
		Failure = 4,
		Denom1 = 8,
		Denom2 = 16,
		Denom3 = 8|16,
		Denom4 = 32,
		Denom5 = 32|8,
		Denom6 = 32 | 16,
		Denom7 = 32 | 16 | 8,
		TransportOpen = 64
	};
	enum  MiscState:char
	{
		Stalled = 1,
		FlashDownload = 2,
		Prestack = 4,
		RawBarcode = 8,
		DeviceCapabilities = 16,
		Disabled = 32
	};
	
	enum  Model :char
	{
		SCAdvance83 = 0x54
	};


	//Command byte0
	enum  Denomination:char
	{
		EnabledDenom1 = 1,
		EnabledDenom2 = 2,
		EnabledDenom3 = 4,
		EnabledDenom4 = 8,
		EnabledDenom5 = 16,
		EnabledDenom6 = 32,
		EnabledDenom7 = 64
	};

	//Command byte1
	enum  OmnibusOpMode1 :char
	{
		SpecialInterrupt = 1,
		HighSecurity = 2,
		OrientationControl1w =4,
		OrientationControl2w = 8,
		OrientationControlAw = 12,
		EscrowMode = 16,
		Stack = 32,
		Return = 64
	};
	//Command byte2
	enum  OmnibusOpMode2 :char
	{
		NoPushMode = 1,
		Barcode = 2,
		PupA = 0x00,
		PupB = 4,
		PupC = 8,
		ExtendedNoteReport = 16,
		ExtendedCouponReporting = 32
	};

	class MpostLite
	{

	public:

		MpostLite(std::string sport);
		~MpostLite();
		
		

		//State members
		std::atomic<bool>  idling = 0;
		std::atomic<bool>  accepting = 0;
		std::atomic<bool>  escrowedState = 0;
		std::atomic<bool>  stacking = 0;
		std::atomic<bool>  stackedEvent = 0;
		std::atomic<bool>  returning = 0;
		std::atomic<bool>  returnedEvent = 0;

		//Status members
		std::atomic<bool>  cheated = 0;
		std::atomic<bool>  rejected = 0;
		std::atomic<bool>  jammed = 0;
		std::atomic<bool>  stackerFull = 0;
		std::atomic<bool>  cassetteAttached = 0;
		std::atomic<bool>  paused = 0;
		std::atomic<bool>  calibration = 0;

		//AdditionalStatus
		std::atomic<bool>  powerUp = 0;
		std::atomic<bool>  invalidCommand = 0;
		std::atomic<bool>  failure = 0;
		std::atomic<bool>  transportOpen = 0;
		
		//MiscState
		std::atomic<bool>  stalled;
		std::atomic<bool>  flashDownload;
		std::atomic<bool>  prestack;
		std::atomic<bool>  rawBarcode;
		std::atomic<bool>  deviceCapabilities;
		std::atomic<bool>  disabled;

		std::vector<std::string> currencies;
		std::vector<NoteType> noteTypes;
		Note bankNoteInEscrow;


		void querySoftwareCRC();
		void queryCashBoxTotal();
		void clearCashBoxTotal();
		void queryVariantComponentName();
		void queryValueTable();
		void accept();
		void configureNoteTypes(std::vector<int>);
		void disableAccept();
		void stack();
		void rollback();
		void reset();
		bool connected();
		boost::signals2::signal<void ()> changed;


	private:


		CRITICAL_SECTION sendCmdCs;

		asio::io_service io;
		asio::serial_port *port;

		std::thread *pollingThread;
		std::atomic<bool> threadActive = false;

		//cmd bytes
		std::atomic<char> cmd1 = 0;
		std::atomic<char> cmd2 = 0;
		std::atomic<char> cmd3 = 0;

		//polling result 6 bytes
		//b0
		std::atomic<char> state;
		//b1
		std::atomic<char> status;
		//b2
		std::atomic<char> additionalStatus;
		//b3
		std::atomic<char> miscState;
		//b4
		char model;
		//b5
		int version;



		//commands

		static const char CmdOmnibus = 0x10;
		static const char CmdCalibrate = 0x40;
		static const char CmdFlashDownload = 0x50;
		static const char CmdAuxiliary = 0x60;
		static const char CmdExpanded = 0x70;

		static const char CmdAuxQuerySoftwareCRC = 0x00;
		static const char CmdAuxQueryCashBoxTotal = 0x01;
		static const char CmdAuxQueryDeviceResets = 0x02;
		static const char CmdAuxClearCashBoxTotal = 0x03;
		static const char CmdAuxQueryAcceptorType = 0x04;
		static const char CmdAuxQueryAcceptorSerialNumber = 0x05;
		static const char CmdAuxQueryAcceptorBootPartNumber = 0x06;
		static const char CmdAuxQueryAcceptorApplicationPartNumber = 0x07;
		static const char CmdAuxQueryAcceptorVariantName = 0x08;
		static const char CmdAuxQueryAcceptorVariantPartNumber = 0x09;
		static const char CmdAuxQueryAcceptorAuditLifeTimeTotals = 0x0A;
		static const char CmdAuxQueryAcceptorAuditQPMeasures = 0x0B;
		static const char CmdAuxQueryAcceptorAuditPerformanceMeasures = 0x0C;
		static const char CmdAuxQueryDeviceCapabilities = 0x0D;
		static const char CmdAuxQueryAcceptorApplicationID = 0x0E;
		static const char CmdAuxQueryAcceptorVariantID = 0x0F;
		static const char CmdAuxQueryBNFStatus = 0x10;
		static const char CmdAuxSetBezel = 0x11;


		//
		int ackToggleBit = 0x0;
		int nakCount = 0; //Consec. NAK count 
		const int pollRate = 200; //msec, recommended

		char denomination = 0; //deni

		//


		void poll();
		char checkSum(char * cmd);
		std::vector<char> makeCommmand(std::vector<char> &payload);
		std::vector<char> sendCommand(std::vector<char> &payload);
		bool isAcked(char* reply);

		void setExtendedPollResults(char *data);
		bool setPollResults(char *data);
		void setMode(char cmd1, char cmd2, char cmd3);

		
		void statusChanged();
		void pollingThreadProc();
		void startPoll();
		void stopPoll();
		
		void flush();
		
	};

}






