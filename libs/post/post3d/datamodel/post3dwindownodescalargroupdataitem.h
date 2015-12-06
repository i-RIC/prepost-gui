#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>
#include <guibase/structuredgridregion.h>

#include <QMap>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <QColor>

class Post3dWindowNodeScalarDataItem;
class vtkLODActor;
class vtkActor;
class vtkDataSetMapper;
class vtkPolyDataMapper;
class vtkContourFilter;

class Post3dWindowIsosurfaceSetProperty;
class Post3dWindowIsosurfaceSelectSolution;

class Post3dWindowNodeScalarGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

private:
	static const int DEFAULT_NUMOFDIV = 15;

public:
	/// Constructor
	Post3dWindowNodeScalarGroupDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeScalarGroupDataItem();
	void updateActorSettings();
	const std::string& currentCondition() const {return m_currentSolution;}
	void informDataChange(const QString& name);
	void setupActors();
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void setCurrentSolution(const std::string& currentSol);

public slots:
	void exclusivelyCheck(Post3dWindowNodeScalarDataItem* item);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateVisibility() override;
	void updateVisibility(bool visible) override;
	void innerUpdateZScale(double scale) override;
	void updateColorSetting();

private:
	void setDefaultValues();
	void setupIsosurfaceSetting();

	std::string m_currentSolution;
	bool m_fullRange;
	StructuredGridRegion::Range3d m_range;
	double m_isoValue;
	QColor m_color;

	vtkSmartPointer<vtkActor> m_isoSurfaceActor;
	vtkSmartPointer<vtkPolyDataMapper> m_isoSurfaceMapper;

public:
	friend class Post3dWindowIsosurfaceSetProperty;
	friend class Post3dWindowIsosurfaceSelectSolution;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_H
