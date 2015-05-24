#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <guibase/gridshapeeditdialog.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkPoints.h>
#include <QVector2D>
#include <vtkLabeledDataMapper.h>

class QDialog;
class QAction;
class QColor;
class QString;
class Post2dWindowGridShapeDataSetSetting;

class Post2dWindowGridShapeDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;
public:
	/// Constructor
	Post2dWindowGridShapeDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridShapeDataItem();
	/// Inform that the grid is updated.
	void update();
	void handleStandardItemDoubleClicked();
	QColor color() const;
	QColor indexColor() const;
//	QColor axesColor();
	GridShapeEditDialog::Shape shape() const {return m_shape;}
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void updateZDepthRangeItemCount() {m_zDepthRange.setItemCount(2);}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void assignActionZValues(const ZDepthRange& range);
private:
	void setupActors();
	void setColor(const QColor& color);
	void setIndexColor(const QColor& color);
	void updateActorSettings();
	void loadShapeFromProjectMainFile(const QDomNode& shapeNode);
	void saveShapeToProjectMainFile(QXmlStreamWriter& writer);
	void updateAxesRegion();
	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkActor> m_wireframeActor;
	vtkSmartPointer<vtkMapper> m_outlineMapper;
	vtkSmartPointer<vtkDataSetMapper> m_wireframeMapper;
	vtkSmartPointer<vtkStructuredGridOutlineFilter> m_outlineFilter;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;

	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;

	double m_color[3];
	GridShapeEditDialog::Shape m_shape;
	bool m_indexVisible;
	double m_indexColor[3];

	bool m_definingBoundingBox;
	bool m_draggingSelectedPoints;
	bool m_nearSelectedPoint;

	QAction* m_editNewPositionAction;
	QAction* m_editDeltaAction;
	QVector2D m_dragStartPoint;
public:
	friend class Post2dWindowGridShapeDataSetSetting;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_H
