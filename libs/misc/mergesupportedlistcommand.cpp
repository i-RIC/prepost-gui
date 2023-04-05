#include "mergesupportedlistcommand.h"

MergeSupportedListCommand::MergeSupportedListCommand(int id, bool allowMerge) :
	m_id {id},
	m_allowMerge {allowMerge}
{}

MergeSupportedListCommand::~MergeSupportedListCommand()
{
	for (auto c : m_commands) {
		delete c;
	}
}

void MergeSupportedListCommand::addCommand(QUndoCommand* command)
{
	m_commands.push_back(command);
}

void MergeSupportedListCommand::redo()
{
	for (auto c : m_commands) {
		c->redo();
	}
}

void MergeSupportedListCommand::undo()
{
	for (auto c : m_commands) {
		c->undo();
	}
}

int MergeSupportedListCommand::id() const
{
	return m_id;
}

bool MergeSupportedListCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_allowMerge){return false;}

	auto c = dynamic_cast<const MergeSupportedListCommand*> (other);
	if (c == nullptr) {return false;}

	if (m_commands.size() != c->m_commands.size()) {return false;}

	for (unsigned int i = 0; i < m_commands.size(); ++i) {
		auto c1 = m_commands.at(i);
		auto c2 = c->m_commands.at(i);

		bool ok = c1->mergeWith(c2);
		if (! ok) {return false;}
	}

	return true;
}
