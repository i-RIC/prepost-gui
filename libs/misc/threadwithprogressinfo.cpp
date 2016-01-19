#include "threadwithprogressinfo.h"

ThreadWithProgressInfo::ThreadWithProgressInfo(QObject* parent) :
	QThread {parent}
{}

ThreadWithProgressInfo::~ThreadWithProgressInfo()
{}
