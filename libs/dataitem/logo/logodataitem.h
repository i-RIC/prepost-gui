#ifndef LOGODATAITEM_H
#define LOGODATAITEM_H

#include "logo_api.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class QMenu;

class LOGO_API LogoDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	LogoDataItem(GraphicsWindowDataItem* parent);
	~LogoDataItem();

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;

public slots:
	void showPropertyDialog() override;

private:
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void updateActorSetting() override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/logodataitem_impl.h"
#endif // _DEBUG

#endif // LOGODATAITEM_H
