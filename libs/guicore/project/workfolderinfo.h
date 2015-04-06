#ifndef WORKFOLDERINFO_H
#define WORKFOLDERINFO_H

#include "../guicore_global.h"

#include <QObject>
#include <QString>
#include <QDateTime>

/// This class stores the information about the specified workfolder.
class GUICOREDLL_EXPORT WorkfolderInfo : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	WorkfolderInfo(const QString& workfolder, QObject* parent);
	/// Folder name
	const QString& folderName() const {return m_folderName;}
	/// Last modified time
	const QDateTime& lastModifiedTime() const {return m_lastModifiedTime;}
	/// Absolute path of workfolder
	const QString& absolutePath() const {return m_absolutePath;}
	/// Whether the workfolder is locked.
	bool isLocked(){return m_isLocked;}

private:
	QString m_folderName;
	QDateTime m_lastModifiedTime;
	QString m_absolutePath;
	bool m_isLocked;
};

#endif // WORKFOLDERINFO_H
