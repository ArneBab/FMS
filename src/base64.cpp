#include "../include/base64.h"

#ifdef XMEM
	#include <xmem.h>
#endif

namespace Base64
{

static const std::string base64chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const bool Encode(const std::vector<unsigned char> &data, std::string &encoded)
{
	std::vector<unsigned char>::size_type currentdatapos=0;

	while(currentdatapos<data.size())
	{
		encoded.push_back(base64chars[(data[currentdatapos]>>2 & 0x3F)]);
		if(currentdatapos+1<data.size())
		{
			encoded.push_back(base64chars[(data[currentdatapos]<<4 & 0x30) | (data[currentdatapos+1]>>4 & 0x0F)]);
			if(currentdatapos+2<data.size())
			{
				encoded.push_back(base64chars[(data[currentdatapos+1]<<2 & 0x3C) | (data[currentdatapos+2]>>6 & 0x03)]);
				encoded.push_back(base64chars[(data[currentdatapos+2] & 0x3F)]);
			}
			else
			{
				encoded.push_back(base64chars[(data[currentdatapos+1]<<2 & 0x3C)]);
				encoded.append("=");
			}
		}
		else
		{
			encoded.push_back(base64chars[(data[currentdatapos]<<4 & 0x30)]);
			encoded.append("==");
		}
		currentdatapos+=3;
	}
	return true;
}

const bool Decode(const std::string &encoded, std::vector<unsigned char> &data)
{
	std::string::size_type charpos=0;
	std::string::size_type encodedpos=0;
	unsigned char currentbyte=0;

	// loop while encoded pos fits in current size
	while(encodedpos+3<encoded.size())
	{
		currentbyte=0;
		charpos=base64chars.find(encoded[encodedpos]);
		if(charpos==std::string::npos)
		{
			return false;
		}
		currentbyte=(charpos<<2 & 0xFC);
		charpos=base64chars.find(encoded[encodedpos+1]);
		if(charpos==std::string::npos)
		{
			return false;
		}
		currentbyte|=(charpos>>4 & 0x03);
		data.push_back(currentbyte);
		currentbyte=(charpos<<4 & 0xF0);

		if(encoded[encodedpos+2]!='=')
		{
			charpos=base64chars.find(encoded[encodedpos+2]);
			if(charpos==std::string::npos)
			{
				return false;
			}
			currentbyte|=(charpos>>2 & 0x0F);
			data.push_back(currentbyte);
			currentbyte=(charpos<<6 & 0xC0);
		}
		
		if(encoded[encodedpos+2]!='=')
		{
			charpos=base64chars.find(encoded[encodedpos+3]);
			if(charpos==std::string::npos)
			{
				return false;
			}
			currentbyte|=(charpos & 0x3F);
			data.push_back(currentbyte);
		}

		encodedpos+=4;
	}

	return true;
}

}	// namespace
