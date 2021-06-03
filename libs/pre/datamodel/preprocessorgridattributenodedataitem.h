#ifndef PREPROCESSORGRIDATTRIBUTENODEDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTENODEDATAITEM_H

#include <guibase/widget/contoursettingwidget.h>
#include <guicore/named/namedgraphicwindowdataitem.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditwidget.h>

#include <vtkIdList.h>

#include <QList>

class GridComplexConditionGroupEditDialog;
class PreProcessorGeoDataGroupDataItemInterface;
class PreProcessorGridCrosssectionWindowProjectDataItem;
class SolverDefinitionGridAttribute;
class Structured2DGridCrosssectionEditCommand;

class QAction;

class vtkMapper;
class vtkStructuredGrid;

class PreProcessorGridAttributeNodeDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~PreProcessorGridAttributeNodeDataItem();

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	ContourSettingWidget::Contour contour() {return m_contour;}
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	SolverDefinitionGridAttribute* condition() const;
	void informDataChange();
	int numberOfDivision() const;

	void updateCrossectionWindows();
	void requestCrosssectionWindowDelete(PreProcessorGridCrosssectionWindowProjectDataItem* item);
	void unregisterCrosssectionWindow(PreProcessorGridCrosssectionWindowProjectDataItem* item);
	void informSelectedVerticesChanged(const QVector<vtkIdType>& vertices);
	bool addToolBarButtons(QToolBar* toolbar) override;

public slots:
	void openCrossSectionWindow();
	void openVerticalCrossSectionWindow();

private slots:
	void editValue();
	void editDifference();
	void editRatio();
	void showDisplaySettingDialog() {showPropertyDialog();}
	void exportToFile();
	void generatePointMap();

private:
	PreProcessorGeoDataGroupDataItemInterface* geoDataGroup() const;
	void editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	int loadFromCgnsFile() override;
	void doApplyOffset(double x, double y) override;

	SolverDefinitionGridAttribute* m_condition;

	bool m_definingBoundingBox;
	/// temporarily used when loading from project file.
	bool m_isCustomModified;
	QAction* m_editValueAction;
	QAction* m_exportAction;
	QAction* m_generatePointMapAction;
	QAction* m_editDifferenceAction;
	QAction* m_editRatioAction;
	QAction* m_openXsectionWindowAction;
	QAction* m_openVXsectionWindowAction;

	int m_numberOfDivision;
	ContourSettingWidget::Contour m_contour;

	void saveContourToProjectMainFile(QXmlStreamWriter& writer);
	void loadContourFromProjectMainFile(const QDomNode& node);

	QList<PreProcessorGridCrosssectionWindowProjectDataItem*> m_crosssectionWindows;

	GridComplexConditionGroupEditDialog* m_groupEditDialog;

public:
	friend class Structured2DGridCrosssectionEditCommand;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEDATAITEM_H
