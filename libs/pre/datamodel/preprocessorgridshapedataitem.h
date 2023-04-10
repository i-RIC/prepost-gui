#ifndef PREPROCESSORGRIDSHAPEDATAITEM_H
#define PREPROCESSORGRIDSHAPEDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guibase/xyaxisdisplaysettingdialog.h>
#include <guibase/widget/gridshapeeditdialog.h>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCubeAxesActor2D.h>
#include <QVector2D>
#include <QPoint>

class PreProcessorGridDataItem;

class QAction;

class PreProcessorGridShapeDataItem : public PreProcessorDataItem
{
	Q_OBJECT

public:
	PreProcessorGridShapeDataItem(PreProcessorDataItem* parent);
	virtual ~PreProcessorGridShapeDataItem();

	virtual void informGridUpdate() = 0;
	void handleStandardItemDoubleClicked() override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	virtual void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;

	QAction* editAction() const;
	QAction* openXsectionWindowAction() const;
	QAction* openVXsectionWindowAction() const;
	virtual void updateActionStatus();

	PreProcessorGridDataItem* gridDataItem() const;

private slots:
	void editShape();
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;

	GridShapeEditDialog::Setting m_setting;

	bool m_definingBoundingBox;
	bool m_draggingSelectedPoints;
	bool m_nearSelectedPoint;

	QAction* m_editAction;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;

	QVector2D m_dragStartPoint;
	QPoint m_pressPoint;
};

#endif // PREPROCESSORGRIDSHAPEDATAITEM_H
