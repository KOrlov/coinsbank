#include "stdafx.h"
#include "mpostLite.h"
#include "xfs.h"
#include "AutoLock.h"
using namespace Mpost;
MpostLite::MpostLite(std::string sport)
{

	

	InitializeCriticalSection(&sendCmdCs);

	this->port = new asio::serial_port(io);
	this->port->open(sport);
	this->port->set_option(asio::serial_port_base::baud_rate(9600));	
	this->port->set_option(asio::serial_port_base::character_size(7));
	
	startPoll();
	this->bankNoteInEscrow.isPresent = false;
	   
}
void MpostLite::setMode(char cmd1, char cmd2, char cmd3)
{
	this->cmd1 = cmd1;
	this->cmd2 = cmd2;
	this->cmd3 = cmd3;
}

//The XOR checksum of bytes 1 through Len-3.
char MpostLite::checkSum(char * cmd)
{
	char chk = 0;
	//start with 1 to exclude stx
	//etx & checksum itself are excluded also
	for (int i = 1; i < cmd[1] - 2; i++)
	{
		chk ^= cmd[i];
	}
	return chk;
}

//Building a command from a payload
std::vector<char> MpostLite::makeCommmand(std::vector<char> &payload)
{
	std::vector<char> cmd;
	int commandLength = 4 + payload.size();
	cmd.push_back(0x02); //STX
	cmd.push_back(commandLength); //size of entire packet

	for (auto c : payload) //copying payload 2 command 
		cmd.push_back(c);


	cmd[2] |= ackToggleBit;
	ackToggleBit ^= 1;
	cmd.push_back(0x03); //ETX
	cmd.push_back(checkSum(cmd.data())); //Calculating checksum

	return cmd;
}

//Sending  and receiving command 
std::vector<char> MpostLite::sendCommand(std::vector<char> &payloadIn)
{
	AutoLock locl(&sendCmdCs);
	Xfs::getInstance()->l.debug_dev("Payload:", payloadIn.data(), payloadIn.size());

	auto cmd = makeCommmand(payloadIn);

	Xfs::getInstance()->l.debug_dev("Sending command:", cmd.data(), cmd.size());

	int  sr = this->port->write_some(boost::asio::buffer(cmd, cmd.size()));

	Xfs::getInstance()->l.debug_dev("Sent bytes:"+std::to_string(sr));

	char resp[255];

	Sleep(50);

	
	int r = 0;

	r=this->port->read_some(boost::asio::buffer(&resp[0],1));

	while (resp[0] != 0x02)
		r = this->port->read_some(boost::asio::buffer(&resp[0], 1));
		

	while (r < 2)	
		r += this->port->read_some(boost::asio::buffer(&resp[r], 255 - r));
	
		
	while (r < resp[1])
		r += this->port->read_some(boost::asio::buffer(&resp[r], 255 - r));
	
	   
	Xfs::getInstance()->l.debug_dev("Response received:", resp, r);


	std::vector<char> payload;

	
	for (int i = 2; i < r - 2; i++)
		payload.push_back(resp[i]);

	
	Xfs::getInstance()->l.debug_dev("Payload:", payload.data(), payload.size());
	

	Xfs::getInstance()->l.debug_dev("Checksum resp:"+ std::to_string(checkSum(resp)));
	
	if (checkSum(resp) != resp[resp[1] - 1])
	{
		Xfs::getInstance()->l.debug_dev("Wrong checksum" );
	}
	else
		Xfs::getInstance()->l.debug_dev("Checksum ok");   

	return payload;
}

//todo: make it 
bool MpostLite::isAcked(char* reply)
{
	Xfs::getInstance()->l.debug_dev("Checking ACK for command");


	if ((reply[2] & 0x01) == ackToggleBit)
	{
		ackToggleBit ^= 0x01;

		nakCount = 0;

		return true;
	}
	else
	{
		nakCount++;

		// If 8 consecutive NAKs are received, force a toggle.
		if (nakCount == 8)
		{
			ackToggleBit ^= 0x01;
			nakCount = 0;
		}
		return false;
	}
}

void MpostLite::statusChanged()
{
	Xfs::getInstance()->l.debug_dev("Device status changed, executing signal");
	changed();
}



void MpostLite::setExtendedPollResults(char *r)
{
	Xfs::getInstance()->l.debug_dev("setExtendedPollResults called");
	if (r[1]== 0x02)
	{
		Xfs::getInstance()->l.debug_dev("subtype = 0x02");
		if (!bankNoteInEscrow.isPresent)
		{
			statusChanged();
		}
		Xfs::getInstance()->l.debug_dev("bankNoteInEscrow.isPresent = true");

		bankNoteInEscrow.isPresent = true;
		int idx = 8;

		
		bankNoteInEscrow.noteType.index = r[idx];
		bankNoteInEscrow.noteType.iso[0] = r[idx + 1];
		bankNoteInEscrow.noteType.iso[1] = r[idx + 2];
		bankNoteInEscrow.noteType.iso[2] = r[idx + 3];

		char bv[4];

		bv[0] = r[idx + 4];
		bv[1] = r[idx + 5];
		bv[2] = r[idx + 6];
		bv[3] = 0;
		std::string sbv(bv);
		bankNoteInEscrow.noteType.base = atoi(sbv.data());
		if (r[idx + 7] == '-')
			bankNoteInEscrow.noteType.base *= -1;
		char be[3];
		be[0] = r[idx + 8];
		be[1] = r[idx + 9];
		be[2] = 0;
		std::string sbe(be);
		bankNoteInEscrow.noteType.exp = atoi(sbe.data());
		return;
		bankNoteInEscrow.orientation = r[idx + 10];
		bankNoteInEscrow.type = r[idx + 11];
		bankNoteInEscrow.series = r[idx + 12];
		bankNoteInEscrow.compatibility = r[idx + 13];
		bankNoteInEscrow.version = r[idx + 14];
		bankNoteInEscrow.classification = r[idx + 15];


	}
}

bool MpostLite::setPollResults(char *data) 
{
	bool ch = false;
	if (data[0] == 0x70 || data[0] == 0x71)
	{
		setExtendedPollResults(data);
		return true;
	}

	bankNoteInEscrow.isPresent = false; 



	if (state.load() != data[1] ||
		status.load() != data[2] ||
		additionalStatus.load() != data[3] ||
		miscState.load() != data[4])
	{

		statusChanged(); 
		ch = true;
	}
	
	ch = true;

	state = data[1];
	status = data[2];
	additionalStatus = data[3];
	miscState = data[4];
	model = data[5];
	version = data[6];

	std::string printStr;
	if (state&DeviceState::Idling) {
		this->idling = true;
		printStr = "Idling";
	}
	else
	{
		this->idling = false;
	}
	if (state&DeviceState::Accepting) {
		this->accepting = true;
		printStr += "|Accepting";
	}
	else
	{
		this->accepting = false;
	}
	if (state&DeviceState::EscrowedState)
	{
		this->escrowedState = true;
		printStr += "|EscrowedState";
	}
	else
	{
		this->escrowedState = false;
	}

	if (state&DeviceState::Stacking) {
		this->stacking = true;
		printStr += "|Stacking";
	}
	else
	{
		this->stacking = false;
	}

	if (state&DeviceState::StackedEvent)
	{
		this->stackedEvent = true;
		printStr += "|StackedEvent";
	}
	else
	{
		this->stackedEvent = false;
	}

	if (state&DeviceState::Returning)
	{
		this->returning = true;
		printStr += "|Returning";
	}
	else
	{
		this->returning = false;
	}

	if (state&DeviceState::ReturnedEvent)
	{
		this->returnedEvent = true;
		printStr += "|ReturnedEvent";
	}
	else
	{
		this->returnedEvent = false;
	}

	if(ch)
		Xfs::getInstance()->l.debug_dev("Device state:" + printStr);


	printStr = "";
	if (status&DeviceSatus::Cheated)
	{
		this->cheated = true;
		printStr = "Cheated";
	}
	else
	{
		this->cheated = false;
	}

	if (status&DeviceSatus::Rejected)
	{
		printStr += "|Rejected";
		this->rejected = true;
	}
	else
	{
		this->rejected = false;
	}


	if (status&DeviceSatus::Jammed)
	{
		this->jammed = true;
		printStr += "|Jammed";
	}
	else
	{
		this->jammed = false;
	}

	if (status&DeviceSatus::StackerFull)
	{
		this->stackerFull = true;
		printStr += "|StackerFull";
	}
	else
	{
		this->stackerFull = false;
	}

	if (status&DeviceSatus::CassetteAttached)
	{
		this->cassetteAttached = true;
		printStr += "|CassetteAttached";
	}
	else
	{
		this->cassetteAttached = false;
	}
	if (status&DeviceSatus::Paused)
	{
		this->paused = true;
		printStr += "|Paused";
	}
	else
	{
		this->paused = false;
	}

	if (status&DeviceSatus::Calibration)
	{
		this->calibration = true;
		printStr += "|Calibration";
	}
	else
	{
		this->calibration = false;
	}
	if (ch)
		Xfs::getInstance()->l.debug_dev("Device status:" + printStr);


	printStr = "";
	if (additionalStatus&AdditionalStatus::PowerUp)
	{
		this->powerUp = true;
		printStr = "PowerUp";
	}
	else
	{
		this->powerUp = false;
	}

	if (additionalStatus&AdditionalStatus::InvalidCommand)
	{
		this->invalidCommand = true;
		printStr += "|InvalidCommand";
	}
	else
	{
		this->invalidCommand = false;
	}

	if (additionalStatus&AdditionalStatus::Failure)
	{
		this->failure = true;
		printStr += "|Failure";
	}
	else
	{
		this->failure = false;
	}

	if (additionalStatus&AdditionalStatus::TransportOpen)
	{
		this->transportOpen = true;
		printStr += "|TransportOpen";
	}
	else
	{
		this->transportOpen = false;
	}

	if (additionalStatus&AdditionalStatus::TransportOpen)
	{
		this->transportOpen = true;
		printStr += "|TransportOpen";
	}
	else
	{
		this->transportOpen = false;
	}


	/* 
	//EXTENDED MODE DOESNT REPORT NOTES IN THIS MESSSAGE
	char billNum = (additionalStatus.load() >> 3) & 0x7;

	if (ch)
		Xfs::getInstance()->l.slog("Additional status:" + printStr);
	if (billNum)
	{
		if (ch)
			Xfs::getInstance()->l.slog("!!!!!!!!!!!!!!!!!!:" + std::to_string(billNum));

	}
	if (ch)
		Xfs::getInstance()->l.slog("Accepted bill num:" + std::to_string(billNum));
		*/

	printStr = "";
	if (miscState&MiscState::Stalled)
	{
		this->stalled = true;
		printStr = "Stalled";
	}
	else
	{
		this->stalled = false;
	}


	if (miscState&MiscState::FlashDownload)
	{
		printStr += "|FlashDownload";
		this->flashDownload = true;
	}
	else
	{
		this->flashDownload = false;
	}

	if (miscState&MiscState::Prestack)
	{
		printStr += "|Prestack";
		this->prestack = true;
	}
	else
	{
		this->prestack = false;
	}

	if (miscState&MiscState::RawBarcode)
	{
		this->rawBarcode = true;
		printStr += "|RawBarcode";
	}
	else
	{
		this->rawBarcode = false;
	}

	if (miscState&MiscState::DeviceCapabilities)
	{
		this->deviceCapabilities = true;
		printStr += "|DeviceCapabilities";
	}
	else
	{
		this->deviceCapabilities = false;
	}

	if (miscState&MiscState::Disabled)
	{
		this->disabled = true;
		printStr += "|Disabled";
	}
	else
	{
		this->disabled = false;
	}
	if (ch)
		Xfs::getInstance()->l.debug_dev("Misc state:" + printStr);

	if (ch)
	{
		printStr = "";
		if (model &(char)Model::SCAdvance83)printStr = "SCAdvance83";
		Xfs::getInstance()->l.debug_dev("Model:" + printStr);

		Xfs::getInstance()->l.debug_dev("SW Version:" + std::to_string(version));
	}
	return ch;

}

void MpostLite::poll()
{

	std::vector<char>c = { CmdOmnibus, cmd1.load(), cmd2.load(), cmd3.load()};
	auto r = sendCommand(c);

	


	auto b = setPollResults(r.data());
	//if (b)
	{
		Xfs::getInstance()->l.debug_dev("Sending poll command:",c.data(),c.size());
		Xfs::getInstance()->l.debug_dev("poll data______:", r.data(), r.size());
	}
	return;
}

bool MpostLite::connected()
{
	if (!this->port)return 0;
	return this->port->is_open();
}
void MpostLite::configureNoteTypes(std::vector<int> enabledNotes)
{
	Xfs::getInstance()->l.debug_dev("configureNoteTypes");


	char notesEnabled1 = 0x0;
	char notesEnabled2 = 0x0;
	char notesEnabled3 = 0x0;
	for (auto n : enabledNotes)
	{
		if (n == 1)notesEnabled1 |= 0x03;
		if (n == 2)notesEnabled1 |= 0x0C;
		if (n == 3)notesEnabled1 |= 0x30;
		if (n == 4)
		{
			notesEnabled1 |= 0x40;
			notesEnabled2 |= 0x01;
		}
			
		if (n == 5)notesEnabled2 |= 0x02;
		if (n == 6)notesEnabled2 |= 0x04;
		if (n == 7)notesEnabled2 |= 0x08;
	}

	
	


	std::vector<char>c = { 0x70, 0x03,0x7f, 0x1C, 0x10, notesEnabled1, notesEnabled2,notesEnabled3,0,0,0,0,0,
		0,0	};

	sendCommand(c);


	
}

void  MpostLite::accept()
{
	Xfs::getInstance()->l.debug_dev("Enable accepting:");

	
	Xfs::getInstance()->l.debug_dev("cmd1:" + std::to_string(cmd1.load()));


	cmd1 = 0x7f; //all types	
	cmd2 = 0x00 | OmnibusOpMode1::OrientationControlAw | OmnibusOpMode1::EscrowMode;
	cmd3 = 0x00 | OmnibusOpMode2::ExtendedNoteReport;
}

void  MpostLite::stack()
{
	Xfs::getInstance()->l.debug_dev("stack:");

	cmd1 = 0x7f; //all types	
	cmd2 = 0x00 | OmnibusOpMode1::OrientationControlAw | OmnibusOpMode1::EscrowMode | OmnibusOpMode1::Stack;
	cmd3 = 0x00 | OmnibusOpMode2::ExtendedNoteReport;
}

void  MpostLite::rollback()
{
	Xfs::getInstance()->l.debug_dev("return:");

	cmd1 = 0x7f; //all types	
	cmd2 = 0x00 | OmnibusOpMode1::OrientationControlAw | OmnibusOpMode1::EscrowMode | OmnibusOpMode1::Return;
	cmd3 = 0x00 | OmnibusOpMode2::ExtendedNoteReport;
}

void MpostLite::reset()
{
	Xfs::getInstance()->l.debug_dev("reset:");
	Xfs::getInstance()->l.debug_dev("stopig poll");

	stopPoll();
	
	AutoLock locl(&sendCmdCs);

	Xfs::getInstance()->l.debug_dev("Sending reset command:");
	std::vector<char>c = { 0x60, 0x7F, 0x7F, 0x7F };


	Xfs::getInstance()->l.debug_dev("Payload:", c.data(), c.size());

	auto cmd = makeCommmand(c);

	Xfs::getInstance()->l.debug_dev("Sending command:", cmd.data(), cmd.size());

	int  sr = this->port->write_some(boost::asio::buffer(cmd, cmd.size()));

	Xfs::getInstance()->l.debug_dev("Sent bytes:" + std::to_string(sr));
	

	Sleep(11000);

	
	
	Xfs::getInstance()->l.debug_dev("starting poll");

	

	startPoll();
}

void  MpostLite::disableAccept()
{
	Xfs::getInstance()->l.debug_dev("Disable accepting:");

	cmd1 = 0x00; //all types
	cmd2 = 0x00;	
	cmd3 = 0x00;
}



void MpostLite::querySoftwareCRC()
{
	Xfs::getInstance()->l.debug_dev("Sending querySoftwareCRC command:");
	std::vector<char>c = { 0x60, 0x00, 0x00, 0x00 };
	auto r = sendCommand(c);

	Xfs::getInstance()->l.debug_dev("querySoftwareCRC data______:", r.data(), r.size());
	return;
}

void MpostLite::queryCashBoxTotal()
{
	Xfs::getInstance()->l.debug_dev("Sending queryCashBoxTotal command:");
	std::vector<char>c = { 0x60, 0x00, 0x00, 0x01 };
	auto r = sendCommand(c);

	Xfs::getInstance()->l.debug_dev("queryCashBoxTotal data______:", r.data(), r.size());
	return;
}

void MpostLite::queryVariantComponentName()
{
	Xfs::getInstance()->l.debug_dev("Sending queryVariantComponentName command:");
	std::vector<char>c = { 0x60, 0x00, 0x00, CmdAuxQueryAcceptorVariantName};
	auto r = sendCommand(c);

	Xfs::getInstance()->l.debug_dev("queryVariantComponentName data______:", r.data(), r.size());

	int i = 1;
	char cr[3];
	
	while (i < r.size() && r[i])
	{
		cr[(i - 1) % 3] = r[i];		
		i++;
	}
	currencies.push_back(cr);
	for(auto ck:currencies)
		Xfs::getInstance()->l.debug_dev("currency:"+std::string(ck));
	
	
	return;
}

void MpostLite::clearCashBoxTotal()
{
	Xfs::getInstance()->l.debug_dev("Sending clearCashBoxTotal command:");
	std::vector<char>c = { 0x60, 0x00, 0x00, CmdAuxClearCashBoxTotal };
	auto r = sendCommand(c);

	Xfs::getInstance()->l.debug_dev("clearCashBoxTotal data______:", r.data(), r.size());
}

void MpostLite::queryValueTable()
{
	Xfs::getInstance()->l.debug_dev("Sending queryValueTable command:");
	std::vector<char>c = { 0x70, 0x06, 0x00, 0x00,0x00 };
	auto r = sendCommand(c);
	Xfs::getInstance()->l.debug_dev("queryValueTable data______:", r.data(), r.size());
	for (int i = 0; i < 7; i++)
	{
		int  idx = 8 + i * 10;
		
		NoteType nt;


		nt.index = r[idx];
		nt.iso[0] = r[idx+1];
		nt.iso[1] = r[idx + 2];
		nt.iso[2] = r[idx + 3];
		
		char bv[4];

		bv[0] = r[idx + 4];
		bv[1] = r[idx + 5];
		bv[2] = r[idx + 6];
		bv[3] = 0;
		std::string sbv(bv);
		nt.base = atoi(sbv.data());
		if (r[idx + 7] == '-')
			nt.base *= -1;
		char be[3];
		be[0] = r[idx + 8];
		be[1] = r[idx + 9];
		be[2] = 0;
		std::string sbe(be);
		nt.exp = atoi(sbe.data());
		noteTypes.push_back(nt);
	}
	
	for (auto n : noteTypes)
	{
		Xfs::getInstance()->l.debug_dev("note type:"+std::to_string(n.index) + ", iso="+n.iso[0]+ n.iso[1]+ n.iso[2]+", base value="+std::to_string(n.base)+ ", exp="+std::to_string(n.exp));
	}
	
}

void MpostLite::startPoll()
{
	flush();
	this->pollingThread = new thread(&MpostLite::pollingThreadProc, this);
}
void MpostLite::stopPoll()
{
	threadActive = false;
	pollingThread->join();
	delete pollingThread;

}

void MpostLite::pollingThreadProc()
{
	threadActive = true;
	
	while (threadActive)
	{
		this->poll();
		Sleep(200);

	}
}

void MpostLite::flush()
{	
	PurgeComm(this->port->native_handle(), PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}




MpostLite::~MpostLite()
{
	
	stopPoll();
	port->close();
	delete port;
	DeleteCriticalSection(&sendCmdCs);

}

