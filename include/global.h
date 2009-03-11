#ifndef _global_
#define _global_

#include <string>
#include <Poco/ScopedLock.h>
#include <Poco/Mutex.h>

#define VERSION_MAJOR		"0"
#define VERSION_MINOR		"3"
#define VERSION_RELEASE		"33"
#define FMS_VERSION			VERSION_MAJOR"."VERSION_MINOR"."VERSION_RELEASE
#define FMS_FREESITE_USK	"USK@0npnMrqZNKRCRoGojZV93UNHCMN-6UU3rRSAmP6jNLE,~BG-edFtdCC1cSH4O3BWdeIYa8Sw5DfyrSV-TKdO5ec,AQACAAE/fms/95/"
#define FMS_VERSION_EDITION	"35"

typedef Poco::ScopedLock<Poco::FastMutex> Guard;

std::string CreateShortIdentityName(const std::string &name, const std::string &publickey);

#endif	// _global_
