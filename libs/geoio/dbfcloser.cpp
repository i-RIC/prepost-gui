#include "dbfcloser.h"

DBFCloser::DBFCloser(DBFHandle handle) :
	m_handle {handle}
{}

DBFCloser::~DBFCloser()
{
	DBFClose(m_handle);
}
