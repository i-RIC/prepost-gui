#ifndef PREPROCESSORDATAITEM_H
#define PREPROCESSORDATAITEM_H

#include "../../guicore_global.h"
#include "../../datamodel/graphicswindowdataitem.h"
#include "preprocessordatamodelinterface.h"
#include "preprocessorwindowinterface.h"
#include <QList>

class QIcon;
class QAction;
class MouseBoundingBox;

class GUICOREDLL_EXPORT PreProcessorDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorDataItem(GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(parent)
	{}
	/// Constructor
	PreProcessorDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, parent)
	{}
	/// Constructor
	PreProcessorDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: GraphicsWindowDataItem(itemlabel, icon, parent)
	{}
	/// Constructor for root node.
	PreProcessorDataItem(ProjectDataItem* parent)
		: GraphicsWindowDataItem(parent)
	{}
	virtual ~PreProcessorDataItem() {}

	PreProcessorWindowInterface* preProcessorWindow() {
		return dynamic_cast<PreProcessorWindowInterface*>(mainWindow());
	}

protected slots:
	void editCaption();

protected:
	QAction* deleteAction();
	MouseBoundingBox* mouseBoundingBox();
	void copyStandardItem();
	virtual PreProcessorDataModelInterface* dataModel() {
		return dynamic_cast<PreProcessorDataModelInterface*>(GraphicsWindowDataItem::dataModel());
	}

public:
	friend class RawData;
};

#endif // PREPROCESSORDATAITEM_H
