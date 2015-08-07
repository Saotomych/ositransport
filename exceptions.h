#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "ositransport_global.h"
#include <exception>

namespace OSIExceptions {

class OSITRANSPORTSHARED_EXPORT CExIllegalArgument: public std::exception
{
	QString m_strErr;
	CExIllegalArgument(){}
public:
	CExIllegalArgument(QString strErr): m_strErr(strErr) {}

};

class OSITRANSPORTSHARED_EXPORT CExConnectError: public std::exception
{
	QString m_strErr;
	CExConnectError(){}
public:
	CExConnectError(QString strErr): m_strErr(strErr) {}

};

class OSITRANSPORTSHARED_EXPORT CExIllegalClassMember: public std::exception
{
	QString m_strErr;
	CExIllegalClassMember(){}
public:
	CExIllegalClassMember(QString strErr): m_strErr(strErr) {}

};

class OSITRANSPORTSHARED_EXPORT CExIOError: public std::exception
{
	QString m_strErr;
	CExIOError(){}
public:
	CExIOError(QString strErr): m_strErr(strErr) {}

};

}

#endif
