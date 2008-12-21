#ifndef _unknown_identity_requester_
#define _unknown_identity_requester_

#include "identityrequester.h"

class UnknownIdentityRequester:public IdentityRequester
{
public:
	UnknownIdentityRequester();
	UnknownIdentityRequester(FCPv2 *fcp);

private:
	void Initialize();
	void PopulateIDList();
	
};

#endif	// _unknown_identity_requester_