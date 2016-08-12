#ifndef COLORSOURCE_H
#define COLORSOURCE_H

#include "../guicore_global.h"
#include "projectdataitem.h"

class QColor;
class QString;

class GUICOREDLL_EXPORT ColorSource : public ProjectDataItem
{
public:
	ColorSource(ProjectDataItem* d);
	~ColorSource();

	void load(const QString& filename);
	void loadDefault();
	QColor getColor(int index) const ;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/colorsource_impl.h"
#endif // _DEBUG

#endif // COLORSOURCE_H
