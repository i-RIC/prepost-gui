#ifndef WORKFOLDERINFO_H
#define WORKFOLDERINFO_H

#include "../guicore_global.h"

#include <QObject>

class QString;
class QDateTime;

/// This class stores the information about the specified workfolder.
class GUICOREDLL_EXPORT WorkfolderInfo : public QObject
{
	Q_OBJECT
public:
	WorkfolderInfo(const QString& workfolder, QObject* parent);
	~WorkfolderInfo();

	QString folderName() const;
	QString absolutePath() const;
	bool isLocked() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/workfolderinfo_impl.h"
#endif // _DEBUG

#endif // WORKFOLDERINFO_H
