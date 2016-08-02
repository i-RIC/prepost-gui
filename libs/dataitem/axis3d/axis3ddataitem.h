#ifndef AXIS3DDATAITEM_H
#define AXIS3DDATAITEM_H

#include "axis3d_api.h"
#include <guicore/datamodel/graphicswindowdataitem.h>

class AXIS3D_API Axis3dDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Axis3dDataItem(GraphicsWindowDataItem* parent);
	~Axis3dDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	void updateVisibility(bool visible) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/axis3ddataitem_impl.h"
#endif // _DEBUG

#endif // AXIS3DDATAITEM_H
