#ifndef GRIDBIRDEYEWINDOWDATAMODEL_H
#define GRIDBIRDEYEWINDOWDATAMODEL_H

#include <guicore/datamodel/graphicswindowsimpledatamodel.h>
#include <QList>
#include <QMap>
#include <QColor>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkWarpScalar.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkScalarBarWidget.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

#include <string>

class ProjectDataItem;
class GridBirdEyeWindow;
class GridBirdEyeWindowGraphicsView;
class GridBirdEyeWindowEditZScaleCommand;
class GridBirdEyeWindowEditColorCommand;

class GridBirdEyeWindowDataModel : public GraphicsWindowSimpleDataModel
{
	Q_OBJECT

private:
	const static int DEFAULT_SCALE = 3;

public:
	enum ColorType {ctNode, ctCell, ctCustom};
	/// Constructor
	GridBirdEyeWindowDataModel(GridBirdEyeWindow* w, ProjectDataItem* parent);
	/// Destructor
	~GridBirdEyeWindowDataModel();
	/// The view to display pre-processor main region
	GridBirdEyeWindowGraphicsView* graphicsView() const;
	void editZScale();
	void displaySetting();
	void updateGrid();
	void updateAxes();
	void setAxesColor(const QColor& col);
	QColor axesColor() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

private:
	void init();
	void setDefaultColor();
	void updateColor();
	void updateScalarBar();

	ColorType m_colorType;
	std::string m_attributeName;
	QColor m_customColor;

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkWarpScalar> m_warp;
	vtkSmartPointer<vtkCubeAxesActor2D> m_cubeAxesActor;

	vtkSmartPointer<vtkAxesActor> m_axesActor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

public:
	friend class GridBirdEyeWindowEditZScaleCommand;
	friend class GridBirdEyeWindowEditColorCommand;
	friend class GridBirdEyeWindowEditAxesCommand;
};

#endif // GRIDBIRDEYEWINDOWDATAMODEL_H
