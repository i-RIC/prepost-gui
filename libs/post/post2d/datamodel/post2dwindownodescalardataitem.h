#ifndef POST2DWINDOWNODESCALARDATAITEM_H
#define POST2DWINDOWNODESCALARDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dWindowNodeScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
};

#endif // POST2DWINDOWNODESCALARDATAITEM_H
