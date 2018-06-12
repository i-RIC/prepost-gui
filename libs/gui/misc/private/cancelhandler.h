#ifndef CANCELHANDLER_H
#define CANCELHANDLER_H

#include <QObject>

class CancelHandler : public QObject
{
	Q_OBJECT

public:
	CancelHandler(QObject* parent = nullptr);

	bool isCanceled();

public slots:
	void cancel();

private:
	bool m_isCanceled;
};

#endif // CANCELHANDLER_H
