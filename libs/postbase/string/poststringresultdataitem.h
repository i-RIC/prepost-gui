#ifndef POSTSTRINGRESULTDATAITEM_H
#define POSTSTRINGRESULTDATAITEM_H

#include "../postbase_global.h"

#include "poststringresultengine.h"
#include "poststringresultsettingcontainer.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class PostZoneDataItem;

class vtkActor2D;

class POSTBASEDLL_EXPORT PostStringResultDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	PostStringResultDataItem(GraphicsWindowDataItem* parent);
	~PostStringResultDataItem() override;

	void update();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	void showPropertyDialog() override;

private:
	void setupActors();

	QDialog* propertyDialog(QWidget* parent) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void updateActorSetting() override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	virtual PostZoneDataItem* zoneDataItem() const = 0;

	PostStringResultSettingContainer m_setting;
	vtkActor2D* m_actor;
};

#endif // POSTSTRINGRESULTDATAITEM_H
