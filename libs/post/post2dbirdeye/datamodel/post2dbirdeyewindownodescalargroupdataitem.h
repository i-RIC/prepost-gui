#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <guibase/contoursettingwidget.h>
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

class Post2dBirdEyeWindowNodeScalarDataItem;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;

class Post2dBirdEyeWindowNodeScalarGroupDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	/// Constructor
	Post2dBirdEyeWindowNodeScalarGroupDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupDataItem();
	std::string currentSolution() const {return m_setting.currentSolution;}
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setCurrentSolution(const std::string& currentSol);
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

public slots:
	void exclusivelyCheck(Post2dBirdEyeWindowNodeScalarDataItem* item);

protected:
	void updateVisibility(bool visible) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();
	void updateActorSettings();
	void createRangeClippedPolyData();
	void createValueClippedPolyData();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	Post2dWindowContourSetting m_setting;
	// for scalar bar
	QMap<std::string, QString> m_colorbarTitleMap;

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
	class SelectSolutionCommand;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_H
