#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/widget/contoursettingwidget.h>
#include <guibase/structuredgridregion.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/opacitycontainer.h>

#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QColor>

#include <memory>

class NamedGraphicWindowDataItem;
class Post3dWindowNodeScalarGroupTopDataItem;

class vtkActor;
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
	Post3dWindowNodeScalarGroupDataItem(const std::string& target, Post3dWindowDataItem* parent);
	~Post3dWindowNodeScalarGroupDataItem();

	void informDataChange(const QString& name);
	void updateZDepthRangeItemCount() override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	std::string target() const override;
	void setTarget(const std::string& target) override;

public slots:
	void showPropertyDialog() override;
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();
	void updateActorSetting() override;

	void setDefaultValues();
	void setupIsosurfaceSetting();
	Post3dWindowNodeScalarGroupTopDataItem* topDataItem() const;

	class Impl;
	std::unique_ptr<Impl> impl;

	class SetSettingCommand;
	class Setting;
	class SettingEditWidget;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_H
