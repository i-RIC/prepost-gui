#ifndef PROJECTDATACOPYTHREAD_H
#define PROJECTDATACOPYTHREAD_H

#include <QThread>

class QString;

class ProjectDataCopyThread : public QThread
{
public:
	ProjectDataCopyThread(const QString& from, const QString& to, QObject* parent);

	bool result() const;

private:
	void run();

private:
	QString m_from;
	QString m_to;

	bool m_result;
};

#endif // PROJECTDATACOPYTHREAD_H
