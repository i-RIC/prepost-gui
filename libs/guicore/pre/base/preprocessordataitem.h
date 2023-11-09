#ifndef PREPROCESSORDATAITEM_H
#define PREPROCESSORDATAITEM_H

#include "../../guicore_global.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "preprocessordatamodeli.h"

#include <QList>

class QIcon;
class QAction;
class MouseBoundingBox;
class PreProcessorWindowI;

class GUICOREDLL_EXPORT PreProcessorDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorDataItem(GraphicsWindowDataItem* parent);
	PreProcessorDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	PreProcessorDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	PreProcessorDataItem(ProjectDataItem* parent);

	~PreProcessorDataItem() override;

	PreProcessorWindowI* preProcessorWindow() const;

protected slots:
	void editCaption();

protected:
	QAction* deleteAction() const;
	MouseBoundingBox* mouseBoundingBox();
	void copyStandardItem();
	PreProcessorDataModelI* dataModel() const override;

public:
	friend class GeoData;
};

#endif // PREPROCESSORDATAITEM_H
