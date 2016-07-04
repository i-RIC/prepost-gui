#ifndef PROJECTDATAMOVETHREAD_H
#define PROJECTDATAMOVETHREAD_H

#include <QThread>

class ProjectDataMoveThread : public QThread
{
public:
	ProjectDataMoveThread(const QString& from, const QString& to, QObject* parent);
	bool result() const;

private:
	void run();

private:
	QString m_from;
	QString m_to;
	bool m_result;
};

#endif // PROJECTDATAMOVETHREAD_H
