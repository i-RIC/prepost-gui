#ifndef POST3DWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST3DWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/widget/contoursettingwidget.h>
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>

#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QColor>

class NamedGraphicWindowDataItem;

class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;

class Post3dWindowIsosurfaceSetProperty;

class Post3dWindowNodeScalarGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	Post3dWindowNodeScalarGroupTopDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeScalarGroupTopDataItem();

	void updateActorSettings();
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;
	void undoableDeleteItem(GraphicsWindowDataItem* item, bool noDraw) override;

protected:
	void addCustomMenuItems(QMenu* menu) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

private:
	void setDefaultValues();
	void setupIsosurfaceSetting();

	std::string m_target;
	bool m_fullRange;
	StructuredGridRegion::Range3d m_range;
	double m_isoValue;
	double m_zScale;
	QColor m_color;
	vtkSmartPointer<vtkActor> m_isoSurfaceActor;
	vtkSmartPointer<vtkPolyDataMapper> m_isoSurfaceMapper;

public:
	class CreateCommand;
	class DeleteCommand;
	friend class Post3dWindowIsosurfaceSetProperty;
	friend class Post3dWindowIsosurfaceSelectSolution;
};

#endif // POST3DWINDOWNODESCALARGROUPTOPDATAITEM_H
