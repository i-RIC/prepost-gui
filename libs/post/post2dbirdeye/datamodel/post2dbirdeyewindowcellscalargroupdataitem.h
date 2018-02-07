#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/widget/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guibase/structuredgridregion.h>
#include <postbase/post2dwindowcontoursetting.h>

#include <QMap>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

class NamedGraphicWindowDataItem;
class Post2dBirdEyeWindowCellScalarDataItem;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;

class Post2dBirdEyeWindowCellScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	Post2dBirdEyeWindowCellScalarGroupDataItem(Post2dBirdEyeWindowDataItem* parent, CheckFlag cflag, ReorderFlag rflag, DeleteFlag dflag);
	~Post2dBirdEyeWindowCellScalarGroupDataItem();

	std::string target() const;
	void setTarget(const std::string& target);

	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void handleStandardItemChange() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void updateVisibility(bool visible) override;
	void innerUpdateZScale(double scale) override;
	void undoCommands(QDialog* propDialog, QUndoCommand* parent);

private:
	void setupActors();
	void updateActorSettings();
	void createRangeClippedPolyData();
	void createValueClippedPolyData();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowContourSetting m_setting;

	vtkSmartPointer<vtkWarpScalar> m_warp;
	vtkLODActor* m_contourActor;
	vtkDataSetMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkLODActor* m_fringeActor;
	vtkDataSetMapper* m_fringeMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

	class SetSettingCommand;

public:
	friend class Post2dBirdEyeWindowCellScalarGroupTopDataItem;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_H
