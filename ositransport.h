#ifndef OSITRANSPORT_H
#define OSITRANSPORT_H

#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/XmlOutputterHook.h>
#include <cppunit/XmlOutputter.h>

#define UNITTEST
#include "ositransport_global.h"

class Ositransport: public QObject, CppUnit::TestCase
{

	Q_OBJECT

public:
    Ositransport();

public slots:
	void slotConnectionReceive(const CConnection* that);
	void slotConnectionStop(const CConnection* that);
};

#endif // OSITRANSPORT_H
