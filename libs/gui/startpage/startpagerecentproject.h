#ifndef STARTPAGERECENTPROJECT_H
#define STARTPAGERECENTPROJECT_H

#include "startpagelabel.h"

#include <QString>

class StartPageRecentProject : public StartPageLabel
{

public:
	StartPageRecentProject(const QString& filename, QWidget* parent);
	const QString& filename() {return m_filename;}

private:
	QString m_filename;
};

#endif // STARTPAGERECENTPROJECT_H
