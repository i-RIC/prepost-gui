#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <guibase/contoursettingwidget.h>
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
	/// Constructor
	Post3dWindowContourGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowContourGroupDataItem();
	QString currentSolution() const {return m_currentSolution;}
	ContourSettingWidget::Contour contour() const {return m_contour;}
	int numberOfDivision() const {return m_numberOfDivision;}
	LookupTableContainer* lookupTable();
	void setSetting(const QString& sol, ContourSettingWidget::Contour c, int numOfDiv, const LookupTableContainer& lookup, bool upper, bool lower, const QString& title, const ScalarBarSetting& setting, const vtkTextPropertySettingContainer& titleC, const vtkTextPropertySettingContainer& labelC, bool draw);
	void updateChildActors();
	vtkActor* setupActorAndMapper(vtkAlgorithmOutput* algo);
	void showSettingDialog();
	QMap<QString, Post3dWindowFaceDataItem::Setting> faceMap();
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);
	void setupScalarBarActor();
	void updateScalarBarActorSetting();
	void handleStandardItemChange();
	void update();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

protected:
	void doLoadFromProjectMainFile(const QDomNode&);
	void doSaveToProjectMainFile(QXmlStreamWriter&);
	QDialog* propertyDialog(QWidget* p);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void updateVisibility(bool visible);
	void innerUpdateZScale(double scale);
private:
	QString m_currentSolution;
	int m_numberOfDivision;
	ContourSettingWidget::Contour m_contour;
	bool m_fillUpper;
	bool m_fillLower;

	// for color bar
	ScalarBarSetting m_scalarBarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	QMap<QString, QString> m_colorBarTitleMap;

	void saveContourToProjectMainFile(QXmlStreamWriter& writer);
	void loadContourFromProjectMainFile(const QDomNode& node);
	vtkActor* setupIsolinesActorAndMapper(vtkAlgorithmOutput* algo);
	vtkActor* setupContourFigureActorAndMapper(vtkAlgorithmOutput* algo);
	vtkActor* setupColorFringeActorAndMapper(vtkAlgorithmOutput* algo);

	vtkSmartPointer<vtkScalarBarWidget> m_scalarBarWidget;
	double m_zScale;
public:
	friend class Post3dWindowContourGroupSetSetting;
	friend class Post3dWindowContourGroupSetFaceMap;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_H
