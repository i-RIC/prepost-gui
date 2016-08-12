#ifndef DISTANCEMEASUREDATAITEM_H
#define DISTANCEMEASUREDATAITEM_H

#include "distancemeasure_api.h"
#include "distancemeasurecopypropertydialog.h"

#include <guibase/vtktool/vtklabel2dactor.h>
#include <guibase/vtktool/vtklineactor.h>
#include <guicore/datamodel/graphicswindowdataitem.h>

#include <QVector2D>
#include <QPoint>
#include <QColor>
#include <QPixmap>
#include <QCursor>

class QMenu;
class QAction;
class DistanceMeasureDialog;

class DISTANCEMEASURE_API DistanceMeasureDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	enum MouseEventMode {
		meNormal,
		meBeforeDefining,
		meDefining,
		meTranslate,
		meTranslatePrepare,
		meMoveVertex,
		meMoveVertexPrepare
	};

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

	void setPoints(const QVector2D& v1, const QVector2D& v2);

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	void addCustomMenuItems(QMenu* menu) override;

public slots:
	void showPropDialog();
	void redefine();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	QVector2D updatePoint2ByShift(const QVector2D& v1, const QVector2D& v2);
	void updateMouseEventMode(const QVector2D& v, VTKGraphicsView* view);
	void setupActors();
	void updateActorSettings();
	QString autoLabel() const;
	void updateMouseCursor();
	void updateMouseCursor(VTKGraphicsView* v);

	vtkLabel2DActor m_labelActor;
	vtkLineActor m_lineActor;

	bool m_showLabel;
	DistanceMeasureCopyPropertyDialog::LabelMode m_labelMode;
	QString m_customLabel;
	int m_fontSize;
	QVector2D m_point1;
	QVector2D m_point2;
	bool m_showMarkers;
	int m_markerSize;
	QColor m_color;

	MouseEventMode m_mouseEventMode;
	bool m_defined;
	bool m_shiftPressed;
	int m_dragPointTarget;

	QPoint m_dragStartPoint;
	QVector2D m_dragStartPoint1;
	QVector2D m_dragStartPoint2;

	QMenu* m_rightClickingMenu;
	QAction* m_redefineAction;
	QAction* m_propertyAction;

	QPixmap m_movePointPixmap;
	QCursor m_movePointCursor;

private:
	class DefineCommand;
	class MoveVertexCommand;
	class RedefineCommand;
	class SetSettingCommand;
	class TranslateCommand;
};

#endif // DISTANCEMEASUREDATAITEM_H
