#ifndef POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_H
#define POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QMenu>
#include <QMouseEvent>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarWidget.h>

class NamedGraphicWindowDataItem;

class Post3dWindowParticlesScalarGroupDataItem : public Post3dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post3dWindowParticlesScalarGroupDataItem(Post3dWindowDataItem* parent);
	virtual ~Post3dWindowParticlesScalarGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

protected:
	void innerUpdateZScale(double zscale) override;
	void updateVisibility(bool visible) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupActors();
	void updateActorSettings();
	void setupScalarBarSetting();

	ScalarSettingContainer m_setting;
	QMap<std::string, QString> m_scalarbarTitleMap;

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;

	class SetSettingCommand;
};

#endif // POST3DWINDOWPARTICLESSCALARGROUPDATAITEM_H
