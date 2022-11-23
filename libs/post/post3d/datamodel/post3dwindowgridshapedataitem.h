#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/widget/gridshapeeditdialog.h>

#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkActor2D.h>
#include <vtkDataSetMapper.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkLabeledDataMapper.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
//#include <vtkCubeAxesActor2D.h>

#include <QVector2D>

class Post3dWindowGridShapeDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

private:
	static const int normalOutlineWidth = 1;
	static const int selectedOutlineWidth = 2;

public:
	Post3dWindowGridShapeDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowGridShapeDataItem();

	void handleStandardItemDoubleClicked() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void update();

private:
	void setupActors();
	void updateActorSettings();
	void loadShapeFromProjectMainFile(const QDomNode& shapeNode);
	void saveShapeToProjectMainFile(QXmlStreamWriter& writer);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void innerUpdateZScale(double scale) override;

	vtkSmartPointer<vtkActor> m_outlineActor;
	vtkSmartPointer<vtkLODActor> m_wireframeActor;
	vtkSmartPointer<vtkMapper> m_outlineMapper;
	vtkSmartPointer<vtkDataSetMapper> m_wireframeMapper;
	vtkSmartPointer<vtkStructuredGridOutlineFilter> m_outlineFilter;
	vtkSmartPointer<vtkPoints> m_pointsBeforeDragging;
	vtkSmartPointer<vtkTransformFilter> m_indexTransformFilter;
	vtkSmartPointer<vtkTransform> m_indexTransform;
	vtkSmartPointer<vtkActor2D> m_indexActor;
	vtkSmartPointer<vtkLabeledDataMapper> m_indexMapper;

	GridShapeEditDialog::Setting m_setting;

	class SetSettingCommand;

public:
	friend class Post3dWindowGridShapeDataSetSetting;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_H
