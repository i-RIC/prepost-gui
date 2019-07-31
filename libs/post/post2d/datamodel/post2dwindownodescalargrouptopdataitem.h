#ifndef POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/post2dwindowcontoursetting.h>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkLODActor.h>
#include <vtkScalarBarWidget.h>

#include <QMap>
#include <QList>

class NamedGraphicWindowDataItem;
class Post2dWindowNodeScalarGroupDataItem;
class vtkLODActor;
class vtkActor;
class vtkAlgorithm;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;

class Post2dWindowNodeScalarGroupTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowNodeScalarGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeScalarGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	bool hasTransparentPart() override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	QList<QString> selectedScalars();
	bool checkKmlExportCondition(const QString& target);
	bool exportKMLHeader(QXmlStreamWriter& writer, const QString& target);
	bool exportKMLFooter(QXmlStreamWriter& writer, const QString& target);
	bool exportKMLForTimestep(QXmlStreamWriter& writer, const QString& target, int index, double time, bool oneShot);

	QList<QString> availableScalars();
	bool checkShapeExportCondition(const QString& target);
	bool exportContourFigureToShape(const QString& target, const QString& filename, double time);

	bool nextScalarBarSetting(ScalarBarSetting& scalarBarSetting);

protected:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

private:
	// for scalar bar
	QMap<std::string, QString> m_colorbarTitleMap;
	std::map<std::string, Post2dWindowNodeScalarGroupDataItem*> m_scalarmap; // only used by ctor and doLoadFromProjectMainFile

	friend class Post2dWindowNodeScalarGroupDataItem;

	class CreateCommand;
};

#endif // POST2DWINDOWNODESCALARGROUPTOPDATAITEM_H
