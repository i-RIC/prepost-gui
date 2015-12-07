#ifndef THREADWITHPROGRESSINFO_H
#define THREADWITHPROGRESSINFO_H

#include "misc_global.h"

#include <QThread>

/// Subclass of QThread that has only one additinal signal progress() that gives progress information
class MISCDLL_EXPORT ThreadWithProgressInfo : public QThread
{
	Q_OBJECT

public:
	/// constructor
	ThreadWithProgressInfo(QObject* parent);
	virtual ~ThreadWithProgressInfo() {}

signals:
	/// This signal is emitted when the thread progresses to 5 percent, 10 percent, etc.
	/**
	 * Emitting this signal is not essential, but when the signals take
	 * much time to finish, emitting this signal helps users feel easy.
	 *
	 * Subclasses of this class may emit this signal in any timing arbitarily.
	 * This signal can be emitted with 1, 2, 3, ... or only with 50, 80, etc.
	 */
	void progress(int percent);
};

#endif // THREADWITHPROGRESSINFO_H
