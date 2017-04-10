#include "startpagerecentproject.h"

#include <QDir>
#include <QFileInfo>

namespace {

const int MAXFILENAMELEN = 35;
const int FOLDERNAMELEN = 10;

} // namespace

StartPageRecentProject::StartPageRecentProject(const QString& filename, QWidget* parent) :
	StartPageLabel(parent),
	m_filename {filename}
{
	QFileInfo finfo(filename);
	QString title = finfo.baseName();
	setTitle(title);

	QString subtitle = filename;
	if (subtitle.length() >= MAXFILENAMELEN) {
		QString filename = finfo.fileName();
		QString path = finfo.path();
		QString foldernameBase = path.right(FOLDERNAMELEN);
		QString foldernameHead = path.left(MAXFILENAMELEN - filename.length() - FOLDERNAMELEN - 4);
		subtitle = foldernameHead;
		subtitle.append("...");
		subtitle.append(foldernameBase);
		subtitle.append("/");
		subtitle.append(filename);
	}

	setSubtitle(QDir::toNativeSeparators(subtitle));
}
