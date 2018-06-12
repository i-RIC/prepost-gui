#ifndef FLUSHREQUESTER_H
#define FLUSHREQUESTER_H

class QString;

class FlushRequester
{
public:
	static void requestFlush(const QString& projectFolder, int idx, int timeOutMSec);

private:
	static void createFlushFile(const QString& filename, int idx);
	static bool flushIsCopying(const QString& filename);
	static void waitUntilFlushFileRemoved(const QString& filename, int timeOutMSec);
	static QString flushFileName(const QString& projectFolder);

	FlushRequester();
};

#endif // FLUSHREQUESTER_H
