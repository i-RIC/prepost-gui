#ifndef POSTCROSSSECTIONWINDOWPROJECTDATAITEM_H
#define POSTCROSSSECTIONWINDOWPROJECTDATAITEM_H

#include "postcrosssection_global.h"

#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <misc/windowgeometrycontainer.h>

class POSTCROSSSECTION_EXPORT PostCrosssectionWindowProjectDataItem : public PostProcessorWindowProjectDataItem
{
public:
	PostCrosssectionWindowProjectDataItem(ProjectDataItem* parent, int index, QWidget* parentWindow);
	bool setupInitialSetting();
	QString windowId() const override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	WindowGeometryContainer m_geometry;
};

#endif // POSTCROSSSECTIONWINDOWPROJECTDATAITEM_H
