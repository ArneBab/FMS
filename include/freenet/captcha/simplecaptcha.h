#ifndef _simple_captcha_
#define _simple_captcha_

#include "icaptcha.h"

class SimpleCaptcha:public ICaptcha
{
public:

	void Generate();

	const bool GetPuzzle(std::vector<unsigned char> &puzzle);
	const bool GetSolution(std::vector<unsigned char> &solution);

private:
	const std::string GenerateRandomString(const int len);
	void ReadPuzzleData(const std::string &filename);

	std::vector<unsigned char> m_puzzle;
	std::vector<unsigned char> m_solution;

};

#endif	// _simple_captcha_
