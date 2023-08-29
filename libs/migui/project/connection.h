#ifndef CONNECTION_H
#define CONNECTION_H

class ConnectionInput;
class ConnectionOutput;

class Connection
{
public:
	Connection(ConnectionOutput* output, ConnectionInput* input);
	~Connection();

	ConnectionOutput* output() const;
	ConnectionInput* input() const;

private:
	ConnectionOutput* m_output;
	ConnectionInput* m_input;
};

#endif // CONNECTION_H
