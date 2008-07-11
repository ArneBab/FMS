#include "../../include/freenet/fmsversionrequester.h"
#include "../../include/freenet/fmsversionxml.h"
#include "../../include/option.h"
#include "../../include/stringfunctions.h"

#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>

FMSVersionRequester::FMSVersionRequester()
{
	Initialize();
}

FMSVersionRequester::FMSVersionRequester(FCPv2 *fcp):IFCPConnected(fcp)
{
	Initialize();
}

const bool FMSVersionRequester::HandleAllData(FCPMessage &message)
{
	std::vector<char> data;
	long datalength;
	FMSVersionXML xml;

	StringFunctions::Convert(message["DataLength"],datalength);

	// wait for all data to be received from connection
	while(m_fcp->Connected() && m_fcp->ReceiveBufferSize()<datalength)
	{
		m_fcp->Update(1);
	}

	// if we got disconnected- return immediately
	if(m_fcp->Connected()==false)
	{
		return false;
	}

	// receive the file
	data.resize(datalength);
	m_fcp->ReceiveRaw(&data[0],datalength);

	// update latest edition #
	std::vector<std::string> parts;
	StringFunctions::Split(message["Identifier"],"/",parts);
	if(parts.size()>2)
	{
		std::string editionstr=parts[2];
		Option::Instance()->Set("FMSVersionEdition",editionstr);
	}

	// parse file into xml and update the database
	if(xml.ParseXML(std::string(data.begin(),data.end()))==true)
	{

		SQLite3DB::Statement st=m_db->Prepare("REPLACE INTO tblFMSVersion(Major,Minor,Release,Notes,Changes,PageKey,SourceKey) VALUES(?,?,?,?,?,?,?);");
		st.Bind(0,xml.GetMajor());
		st.Bind(1,xml.GetMinor());
		st.Bind(2,xml.GetRelease());
		st.Bind(3,xml.GetNotes());
		st.Bind(4,xml.GetChanges());
		st.Bind(5,xml.GetPageKey());
		st.Bind(6,xml.GetSourceKey());
		st.Step();

		m_log->debug("FMSVersionRequester::HandleAllData parsed FMSVersion XML file : "+message["Identifier"]);
	}
	else
	{
		m_log->error("FMSVersionRequester::HandleAllData error parsing FMSVersion XML file : "+message["Identifier"]);
	}

	return true;
}

const bool FMSVersionRequester::HandleGetFailed(FCPMessage &message)
{
	std::vector<std::string> parts;
	StringFunctions::Split(message["Identifier"],"/",parts);

	// fatal error - don't try to download again
	if(message["Fatal"]=="true")
	{
		if(parts.size()>2)
		{
			std::string editionstr=parts[2];
			Option::Instance()->Set("FMSVersionEdition",editionstr);
		}
		m_log->debug("FMSVersionRequester::HandleGetFailed Fatal GetFailed for "+message["Identifier"]);
	}

	return true;
}

const bool FMSVersionRequester::HandleMessage(FCPMessage &message)
{
	if(message["Identifier"].find(m_fcpuniquename)==0)
	{
		
		// ignore DataFound
		if(message.GetName()=="DataFound")
		{
			return true;
		}

		if(message.GetName()=="AllData")
		{
			return HandleAllData(message);
		}

		if(message.GetName()=="GetFailed")
		{
			return HandleGetFailed(message);
		}

		if(message.GetName()=="IdentifierCollision")
		{
			return true;
		}

	}

	return false;	
}

void FMSVersionRequester::Initialize()
{
	m_fcpuniquename="FMSVersionRequester";
	m_lastchecked=Poco::Timestamp();
	m_lastchecked-=Poco::Timespan(0,6,0,0,0);
}

void FMSVersionRequester::Process()
{
	Poco::DateTime now;

	// check every 6 hours
	if((m_lastchecked+Poco::Timespan(0,5,45,0,0))<=now)
	{
		StartRequest();
		m_lastchecked=now;
	}
}

void FMSVersionRequester::RegisterWithThread(FreenetMasterThread *thread)
{
	thread->RegisterFCPConnected(this);
	thread->RegisterFCPMessageHandler(this);
	thread->RegisterPeriodicProcessor(this);
}

void FMSVersionRequester::StartRequest()
{
	FCPMessage message;
	std::string key="";
	std::string editionstr="0";
	int edition=0;
	
	Option::Instance()->Get("FMSVersionKey",key);
	if(Option::Instance()->Get("FMSVersionEdition",editionstr))
	{
		StringFunctions::Convert(editionstr,edition);
		edition++;
		StringFunctions::Convert(edition,editionstr);
	}

	//start request
	message.SetName("ClientGet");
	message["URI"]=key+editionstr+"/FMSVersion.xml";
	message["Identifier"]=m_fcpuniquename+"|"+message["URI"];
	message["ReturnType"]="direct";
	message["MaxSize"]="30000";		// 30K

	m_fcp->SendMessage(message);

}