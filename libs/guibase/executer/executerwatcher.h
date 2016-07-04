#ifndef EXECUTERWATCHER_H
#define EXECUTERWATCHER_H

#include "../guibase_global.h"

class ExecuterI;

class QString;

class GUIBASEDLL_EXPORT ExecuterWatcher
{
public:
	ExecuterWatcher(ExecuterI* executer);
	virtual ~ExecuterWatcher();

	virtual void setMessage(const QString& message);

	virtual void execute() = 0;

protected:
	ExecuterI* executer() const;

private:
	ExecuterI* m_executer;
};

#endif // EXECUTERWATCHER_H
