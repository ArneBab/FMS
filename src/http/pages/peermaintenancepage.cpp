#include "../../../include/http/pages/peermaintenancepage.h"
#include "../../../include/stringfunctions.h"

#include <Poco/DateTime.h>
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include <Poco/DateTimeFormatter.h>

#ifdef XMEM
	#include <xmem.h>
#endif

const std::string PeerMaintenancePage::GeneratePage(const std::string &method, const std::map<std::string,std::string> &queryvars)
{
	std::string content="";
	SQLite3DB::Statement st;
	std::string tempval;
	Poco::DateTime date;

	if(queryvars.find("formaction")!=queryvars.end() && ValidateFormPassword(queryvars))
	{
		if((*queryvars.find("formaction")).second=="removenotseen")
		{
			m_db->Execute("DELETE FROM tblIdentity WHERE LastSeen IS NULL;");
		}
		else if((*queryvars.find("formaction")).second=="removelastseen20")
		{
			date=Poco::Timestamp();
			date-=Poco::Timespan(20,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE LastSeen<?;");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
			st.Step();
		}
		else if((*queryvars.find("formaction")).second=="removeneversent")
		{
			m_db->Execute("DELETE FROM tblIdentity WHERE IdentityID NOT IN (SELECT IdentityID FROM tblMessage GROUP BY IdentityID);");
		}
		else if((*queryvars.find("formaction")).second=="removelastseenneversent20")
		{
			date=Poco::Timestamp();
			date-=Poco::Timespan(20,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE IdentityID NOT IN (SELECT IdentityID FROM tblMessage GROUP BY IdentityID) AND LastSeen<?;");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
			st.Step();
		}
		else if((*queryvars.find("formaction")).second=="removedaysago" && queryvars.find("daysago")!=queryvars.end() && (*queryvars.find("daysago")).second!="")
		{
			int tempint=10000;
			StringFunctions::Convert((*queryvars.find("daysago")).second,tempint);
			date=Poco::Timestamp();
			date-=Poco::Timespan(tempint,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE LastSeen<?;");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
			st.Step();
		}
		else if((*queryvars.find("formaction")).second=="removenulldaysago" && queryvars.find("daysago")!=queryvars.end() && (*queryvars.find("daysago")).second!="")
		{
			int tempint=10000;
			StringFunctions::Convert((*queryvars.find("daysago")).second,tempint);
			date=Poco::Timestamp();
			date-=Poco::Timespan(tempint,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE LastSeen<? AND LocalMessageTrust IS NULL AND LocalTrustListTrust IS NULL;");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
			st.Step();
		}
		else if((*queryvars.find("formaction")).second=="removeposted30daysago")
		{
			date=Poco::Timestamp();
			date-=Poco::Timespan(30,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE IdentityID IN (SELECT tblIdentity.IdentityID FROM tblIdentity INNER JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE (SELECT MAX(MessageDate) FROM tblMessage WHERE tblMessage.IdentityID=tblIdentity.IdentityID)<=? GROUP BY tblIdentity.IdentityID);");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d"));
			st.Step();
		}
		else if((*queryvars.find("formaction")).second=="removeadded20daysneversent")
		{
			date=Poco::Timestamp();
			date-=Poco::Timespan(20,0,0,0,0);
			st=m_db->Prepare("DELETE FROM tblIdentity WHERE IdentityID IN (SELECT tblIdentity.IdentityID FROM tblIdentity LEFT JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE tblMessage.IdentityID IS NULL AND tblIdentity.DateAdded<?);");
			st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
			st.Step();
		}
	}

	content+="<h2>Peer Maintenance</h2>";
	content+="<p class=\"paragraph\">Removing a peer will not remove the messages they sent, but will remove everything else about that peer, including their trust levels.</p>";
	content+="<p>";
	content+="<a href=\"recentlyadded.htm\">Recently Added Peers</a>";
	content+="</p>";
	content+="<table>";
	content+="<tr><th colspan=\"3\">Stats</th></tr>";

	content+="<tr>";
	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity;");
	st.Step();
	st.ResultText(0,tempval);
	content+="<td>"+tempval+"</td>";
	content+="<td>known peers</td>";
	content+="</tr>";

	content+="<tr>";
	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity WHERE LastSeen IS NULL;");
	st.Step();
	st.ResultText(0,tempval);
	content+="<td>"+tempval+"</td>";
	content+="<td>never seen</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removenotseen\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	date=Poco::Timestamp();
	date-=Poco::Timespan(20,0,0,0,0);
	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity WHERE LastSeen<?;");
	st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
	st.Step();
	st.ResultText(0,tempval);
	content+="<tr>";
	content+="<td>"+tempval+"</td>";
	content+="<td>last seen more than 20 days ago</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removelastseen20\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	date=Poco::Timestamp();
	date-=Poco::Timespan(30,0,0,0,0);
	st=m_db->Prepare("SELECT COUNT(*) FROM (SELECT tblIdentity.IdentityID FROM tblIdentity INNER JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE (SELECT MAX(MessageDate) FROM tblMessage WHERE tblMessage.IdentityID=tblIdentity.IdentityID)<=? GROUP BY tblIdentity.IdentityID);");
	st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d"));
	st.Step();
	st.ResultText(0,tempval);
	content+="<tr>";
	content+="<td>"+tempval+"</td>";
	content+="<td>last sent a message more than 30 days ago</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removeposted30daysago\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity LEFT JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE tblMessage.IdentityID IS NULL;");
	st.Step();
	st.ResultText(0,tempval);
	content+="<tr>";
	content+="<td>"+tempval+"</td>";
	content+="<td>never sent a message</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removeneversent\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	date=Poco::Timestamp();
	date-=Poco::Timespan(20,0,0,0,0);
	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity LEFT JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE tblMessage.IdentityID IS NULL AND tblIdentity.DateAdded<?;");
	st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
	st.Step();
	st.ResultText(0,tempval);
	content+="<tr>";
	content+="<td>"+tempval+"</td>";
	content+="<td>added more than 20 days ago and never sent a message</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removeadded20daysneversent\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	date=Poco::Timestamp();
	date-=Poco::Timespan(20,0,0,0,0);
	st=m_db->Prepare("SELECT COUNT(*) FROM tblIdentity LEFT JOIN tblMessage ON tblIdentity.IdentityID=tblMessage.IdentityID WHERE tblMessage.IdentityID IS NULL AND tblIdentity.LastSeen<?;");
	st.Bind(0,Poco::DateTimeFormatter::format(date,"%Y-%m-%d %H:%M:%S"));
	st.Step();
	st.ResultText(0,tempval);
	content+="<tr>";
	content+="<td>"+tempval+"</td>";
	content+="<td>last seen more than 20 days ago and never sent a message</td>";
	content+="<td>";
	content+="<form name=\"frmremove\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removelastseenneversent20\">";
	content+="<input type=\"submit\" value=\"Remove\">";
	content+="</form>";
	content+="</td>";
	content+="</tr>";

	content+="<tr>";
	content+="<td><form name=\"frmdelete\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removedaysago\"></td>";
	content+="<td>last seen <input type=\"text\" name=\"daysago\" size=\"2\"> days ago</td>";
	content+="<td><input type=\"submit\" value=\"Remove\"></form></td>";
	content+="</tr>";

	content+="<tr>";
	content+="<td><form name=\"frmdelete\" method=\"POST\">";
	content+=CreateFormPassword();
	content+="<input type=\"hidden\" name=\"formaction\" value=\"removenulldaysago\"></td>";
	content+="<td>last seen <input type=\"text\" name=\"daysago\" size=\"2\"> days ago, and have null local trust</td>";
	content+="<td><input type=\"submit\" value=\"Remove\"></form></td>";
	content+="</tr>";

	content+="</table>";

	return StringFunctions::Replace(m_template,"[CONTENT]",content);
}

const bool PeerMaintenancePage::WillHandleURI(const std::string &uri)
{
	if(uri.find("peermaintenance.")!=std::string::npos)
	{
		return true;
	}
	else
	{
		return false;
	}
}
