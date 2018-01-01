#include "GlobalIncludes.h"

HResultException::HResultException(HRESULT HResult, std::string Filename, const size_t LineNumber)
{
	std::stringstream ss;

	_com_error err(HResult);
	LPCTSTR errMsg = err.ErrorMessage();

	ss << "HRESULT " << std::showbase << std::hex << HResult 
		<< " was returned at line number " << std::dec << LineNumber
		<< " in " << CaptureFilename(Filename) <<".\n"
		<< "Error description: "	<< errMsg;
	
	error = ss.str();
}