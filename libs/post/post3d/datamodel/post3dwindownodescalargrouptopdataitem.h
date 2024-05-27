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
class Post3dWindowIsosurfaceSetProperty;
class Post3dWindowZoneDataItem;

class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;


class Post3dWindowNodeScalarGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	Post3dWindowNodeScalarGroupTopDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeScalarGroupTopDataItem();

	Post3dWindowZoneDataItem* zoneDataItem() const;
	void informDataChange(const QString& name);
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

private:
	void addCustomMenuItems(QMenu* menu) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

	double m_zScale;
};

#endif // POST3DWINDOWNODESCALARGROUPTOPDATAITEM_H
