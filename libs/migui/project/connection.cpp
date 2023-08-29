#include "connection.h"
#include "connectioninput.h"
#include "connectionoutput.h"

Connection::Connection(ConnectionOutput* output, ConnectionInput* input) :
	m_output {output},
	m_input {input}
{}

Connection::~Connection()
{
	delete m_input;
	delete m_output;
}

ConnectionOutput* Connection::output() const
{
	return m_output;
}

ConnectionInput* Connection::input() const
{
	return m_input;
}
