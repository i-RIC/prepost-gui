#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <guibase/scalarsettingcontainer.h>
#include <guibase/widget/contoursettingwidget.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "post3dwindowfacedataitem.h"

#include <vtkScalarBarWidget.h>

#include <QMap>
#include <QString>

class vtkActor;
class vtkAlgorithmOutput;
class LookupTableContainer;


class Post3dWindowContourGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	Post3dWindowContourGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowContourGroupDataItem();

	const ScalarSettingContainer& scalarSetting() const;
	LookupTableContainer* lookupTable();

	QMap<QString, Post3dWindowFaceDataItem::Setting> faceMap() const;
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map, QString target);

	void updateChildActors();
	vtkActor* setupActorAndMapper(vtkPolyData* data);

	void setupScalarBarActor();
	void updateScalarBarActorSetting();
	void update();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void updateVisibility(bool visible) override;
	void innerUpdateZScale(double scale) override;
	void undoCommands(QDialog* propDialog, QUndoCommand* parent);

private:
	ScalarSettingContainer m_scalarSetting;

	void saveContourToProjectMainFile(QXmlStreamWriter& writer);
	void loadContourFromProjectMainFile(const QDomNode& node);
	vtkActor* setupIsolinesActorAndMapper(vtkPolyData* data);
	vtkActor* setupContourFigureActorAndMapper(vtkPolyData* data);
	vtkActor* setupColorFringeActorAndMapper(vtkPolyData* data);

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	double m_zScale;

	class SetSettingCommand;

public:
	friend class Post3dWindowContourGroupSetFaceMap;
	friend class Post3dWindowContourGroupTopDataItem;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_H
