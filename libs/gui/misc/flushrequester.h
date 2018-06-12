#ifndef FLUSHREQUESTER_H
#define FLUSHREQUESTER_H

#include <QObject>

class QString;
class QWidget;

class FlushRequester : public QObject
{
	Q_OBJECT

public:
	static bool requestFlush(const QString& projectFolder, int idx, QWidget* parent);

private:
	static void createFlushFile(const QString& filename, int idx);
	static bool flushIsCopying(const QString& filename);
	static bool waitUntilFlushFileRemoved(const QString& filename, QWidget* parent);
	static QString flushFileName(const QString& projectFolder);

	FlushRequester(QObject* parent);
};

#endif // FLUSHREQUESTER_H
