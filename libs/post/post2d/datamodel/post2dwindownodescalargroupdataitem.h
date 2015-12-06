#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <postbase/post2dwindowcontoursetting.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

#include <QMap>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeScalarDataItem;
class vtkLODActor;
class vtkActor;
class vtkAlgorithm;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;
class Post2dWindowContourSetProperty;
class Post2dWindowContourSelectSolution;

class Post2dWindowNodeScalarGroupDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	/// Constructor
	Post2dWindowNodeScalarGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarGroupDataItem();
	std::string currentSolution() const {return m_setting.currentSolution;}
	ContourSettingWidget::Contour contour() const {return m_setting.contour;}
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setCurrentSolution(const std::string& currentSol);
	bool hasTransparentPart() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool checkKmlExportCondition();
	bool exportKMLHeader(QXmlStreamWriter& writer);
	bool exportKMLFooter(QXmlStreamWriter& writer);
	bool exportKMLForTimestep(QXmlStreamWriter& writer, int index, double time);
	bool exportContourFigureToShape(const QString& filename, double time);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void updateVisibility(bool visible) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupActors();
	void updateActorSettings();
	vtkPolyData* createRangeClippedPolyData(vtkPolyData* polyData);
	vtkPolyData* createValueClippedPolyData(vtkPolyData* polyData);
	vtkPolyData* createColorContourPolyData(vtkPolyData* polyData);
	void setupIsolineSetting(vtkPolyData* polyData);
	void setupColorContourSetting(vtkPolyData* polyData);
	void setupColorFringeSetting(vtkPolyData* polyData);
	void setupScalarBarSetting();
	static vtkPolyData* setupLowerClippedPolygon(vtkPolyData* inputData, double value);
	static vtkPolyData* setupHigherClippedPolygon(vtkPolyData* inputData, double value);

	// Settings
	Post2dWindowContourSetting m_setting;

	// for scalar bar
	QMap<std::string, QString> m_colorbarTitleMap;

	vtkSmartPointer<vtkLODActor> m_contourActor;
	vtkDataSetMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkSmartPointer<vtkLODActor> m_fringeActor;
	vtkDataSetMapper* m_fringeMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	class ShapeExporter;

	ShapeExporter* m_shapeExporter;

	class SetSettingCommand;
	class SelectSolutionCommand;

public:
	friend class Post2dWindowContourSetProperty;
	friend class Post2dWindowContourSelectSolution;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_H
