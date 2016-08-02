#ifndef AXIS2DDATAITEM_H
#define AXIS2DDATAITEM_H

#include "axis2d_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class AXIS2D_API Axis2dDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Axis2dDataItem(GraphicsWindowDataItem* parent);
	~Axis2dDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void updateVisibility(bool visible) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/axis2ddataitem_impl.h"
#endif // _DEBUG

#endif // AXIS2DDATAITEM_H
