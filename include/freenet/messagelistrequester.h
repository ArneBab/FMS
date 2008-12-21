#ifndef _messagelistrequester_
#define _messagelistrequester_

#include "iindexrequester.h"

#include <map>

class MessageListRequester:public IIndexRequester<long>
{
public:
	MessageListRequester();
	MessageListRequester(FCPv2 *fcp);

private:
	virtual void Initialize();
	virtual void PopulateIDList();
	void StartRequest(const long &id);
	void StartRedirectRequest(FCPMessage &message);
	const bool HandleAllData(FCPMessage &message);
	const bool HandleGetFailed(FCPMessage &message);
	void GetBoardList(std::map<std::string,bool> &boards);
	const bool CheckDateNotFuture(const std::string &datestr) const;
	const bool CheckDateWithinMaxDays(const std::string &datestr) const;

	bool m_localtrustoverrides;
	bool m_savetonewboards;
	long m_messagedownloadmaxdaysbackward;

};

#endif	// _messagelistrequester_
