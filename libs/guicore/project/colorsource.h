#ifndef COLORSOURCE_H
#define COLORSOURCE_H

#include "../guicore_global.h"
#include "projectdataitem.h"
#include <QColor>
#include <QList>

class GUICOREDLL_EXPORT ColorSource : public ProjectDataItem
{

public:
	ColorSource(ProjectDataItem* d);
	void load(const QString& filename);
	void loadDefault();
	QColor getColor(int index) const ;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QList<QColor> m_colors;
};

#endif // COLORSOURCE_H
