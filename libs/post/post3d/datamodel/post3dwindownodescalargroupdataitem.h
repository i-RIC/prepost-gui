#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_H

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

#include "post3dwindownodescalargrouptopdataitem.h"  // can't forward declare nested classes

class Post3dWindowNodeScalarGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	Post3dWindowNodeScalarGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeScalarGroupDataItem();

	void updateActorSettings();
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	std::string target() const override;
	void setTarget(const std::string& target) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void addCustomMenuItems(QMenu* menu) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateVisibility() override;
	void updateVisibility(bool visible) override;
	void innerUpdateZScale(double scale) override;
	void updateColorSetting();
	void validateRange();


private:
	void setDefaultValues();
	void setupIsosurfaceSetting();

	std::string m_target;
	bool m_fullRange;
	StructuredGridRegion::Range3d m_range;
	double m_isoValue;
	QColor m_color;
	int m_opacity;

	vtkSmartPointer<vtkActor> m_isoSurfaceActor;
	vtkSmartPointer<vtkPolyDataMapper> m_isoSurfaceMapper;

	class SetSettingCommand;

public:
	friend class Post3dWindowIsosurfaceSetProperty;
	friend class Post3dWindowIsosurfaceSelectSolution;
	friend class Post3dWindowNodeScalarGroupTopDataItem::CreateCommand;
	friend class Post3dWindowNodeScalarGroupTopDataItem::DeleteCommand;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_H
