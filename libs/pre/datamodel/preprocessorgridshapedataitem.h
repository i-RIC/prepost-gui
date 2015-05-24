#ifndef PREPROCESSORGRIDSHAPEDATAITEM_H
#define PREPROCESSORGRIDSHAPEDATAITEM_H

#include <guicore/pre/base/preprocessordataitem.h>
#include <guibase/gridshapeeditdialog.h>
#include <guibase/xyaxisdisplaysettingdialog.h>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkCubeAxesActor2D.h>
#include <QVector2D>
#include <QPoint>

class QDialog;
class QAction;
class QColor;
class QString;

class PreProcessorGridShapeDataItem : public PreProcessorDataItem
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridShapeDataItem(PreProcessorDataItem* parent);
	virtual ~PreProcessorGridShapeDataItem();
	/// Inform that the grid is updated.
	virtual void informGridUpdate() = 0;
	void handleStandardItemDoubleClicked();
	QColor color();
	QColor indexColor();
//	QColor axesColor();
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	virtual void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*);
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*);
	void addCustomMenuItems(QMenu* menu) override;
	QAction* editAction() {return m_editAction;}
	QAction* openXsectionWindowAction() {return m_openXsectionWindowAction;}
	QAction* openVXsectionWindowAction() {return m_openVXsectionWindowAction;}
	virtual void updateActionStatus();
//	void updateAxesRegion();
private slots:
	void editShape();
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();
protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void setColor(const QColor& color);
	void setIndexColor(const QColor& color);
//	void setAxesColor(const QColor& color);
	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;
//	vtkSmartPointer<vtkCubeAxesActor2D> m_axesActor;

	double m_color[3];
	bool m_indexVisible;
	double m_indexColor[3];

//	bool m_axesVisible;
//	double m_axesColor[3];

	bool m_definingBoundingBox;
	bool m_draggingSelectedPoints;
	bool m_nearSelectedPoint;

//	XYAxisDisplaySettingDialog::Setting m_xAxisSetting;
//	XYAxisDisplaySettingDialog::Setting m_yAxisSetting;

	QAction* m_editAction;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;

	QVector2D m_dragStartPoint;
	QPoint m_pressPoint;
};

#endif // PREPROCESSORGRIDSHAPEDATAITEM_H
