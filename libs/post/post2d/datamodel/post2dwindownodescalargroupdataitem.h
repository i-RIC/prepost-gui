#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <postbase/post2dwindowcontoursetting.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

#include <QMap>

class Post2dWindowNodeScalarDataItem;
class vtkLODActor;
class vtkActor;
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
	QString currentSolution() const {return m_setting.currentSolution;}
	ContourSettingWidget::Contour contour() const {return m_setting.contour;}
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setCurrentSolution(const QString& currentSol);
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

public slots:
	void exclusivelyCheck(Post2dWindowNodeScalarDataItem* item);

protected:
	void updateVisibility(bool visible) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupActors();
	void updateActorSettings();
	void createRangeClippedPolyData();
	void createValueClippedPolyData();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();
	void setupScalarBarSetting();

	// Settings
	Post2dWindowContourSetting m_setting;

	// for scalar bar
	QMap<QString, QString> m_colorbarTitleMap;

	vtkSmartPointer<vtkLODActor> m_contourActor;
	vtkDataSetMapper* m_contourMapper;
	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;
	vtkSmartPointer<vtkLODActor> m_fringeActor;
	vtkDataSetMapper* m_fringeMapper;
	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	vtkSmartPointer<vtkPolyData> m_regionClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_valueClippedPolyData;
	vtkSmartPointer<vtkPolyData> m_colorContourPolyData;

	class SetSettingCommand;
	class SelectSolutionCommand;

public:
	friend class Post2dWindowContourSetProperty;
	friend class Post2dWindowContourSelectSolution;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_H
