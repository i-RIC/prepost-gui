#ifndef PREPROCESSORDATAITEM_H
#define PREPROCESSORDATAITEM_H

#include "../../guicore_global.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "preprocessordatamodelinterface.h"

#include <QList>

class QIcon;
class QAction;
class MouseBoundingBox;
class PreProcessorWindowInterface;

class GUICOREDLL_EXPORT PreProcessorDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorDataItem(GraphicsWindowDataItem* parent);
	PreProcessorDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	PreProcessorDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	PreProcessorDataItem(ProjectDataItem* parent);

	~PreProcessorDataItem();

	PreProcessorWindowInterface* preProcessorWindow() const;

protected slots:
	void editCaption();

protected:
	QAction* deleteAction() const;
	MouseBoundingBox* mouseBoundingBox();
	void copyStandardItem();
	virtual PreProcessorDataModelInterface* dataModel() const;

public:
	friend class GeoData;
};

#endif // PREPROCESSORDATAITEM_H
