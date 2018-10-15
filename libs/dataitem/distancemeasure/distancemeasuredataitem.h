#ifndef DISTANCEMEASUREDATAITEM_H
#define DISTANCEMEASUREDATAITEM_H

#include "distancemeasure_api.h"
#include "distancemeasuresetting.h"
#include "distancemeasurecopypropertydialog.h"

#include <guicore/datamodel/graphicswindowdataitem.h>

class QMenu;

class DISTANCEMEASURE_API DistanceMeasureDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	DistanceMeasureDataItem(const QString& name, GraphicsWindowDataItem* parent);
	~DistanceMeasureDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v) override;
	void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void setPoints(const QPointF& v1, const QPointF& v2);

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void showPropDialog();
	void redefine();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QPointF updatePoint2ByShift(const QPointF& v1, const QPointF& v2);
	void updateMouseEventMode(const QPointF& v, VTKGraphicsView* view);
	void setupActors();
	void updateActorSettings();
	QString autoLabel() const;
	void updateMouseCursor();
	void updateMouseCursor(VTKGraphicsView* v);

private:
	class Impl;
	Impl* impl;

	class DefineCommand;
	class MoveVertexCommand;
	class RedefineCommand;
	class SetSettingCommand;
	class TranslateCommand;
};

#ifdef _DEBUG
	#include "private/distancemeasuredataitem_impl.h"
#endif // _DEBUG

#endif // DISTANCEMEASUREDATAITEM_H
