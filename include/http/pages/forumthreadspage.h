#ifndef _forumthreads_page_
#define _forumthreads_page_

#include "forumpage.h"

class ForumThreadsPage:public ForumPage
{
public:
	ForumThreadsPage(const std::string &templatestr):ForumPage(templatestr,"forumthreads.htm")	{}

	IPageHandler *New()	{ return new ForumThreadsPage(m_template); }

private:
	const std::string GeneratePage(const std::string &method, const std::map<std::string,std::string> &queryvars);
};

#endif	// _forumthreads_page_