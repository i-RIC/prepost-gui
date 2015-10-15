#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"

#include <misc/compositecontainer.h>
#include <misc/qpointfcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <QPointF>

#include <vector>

class Post2dWindowStreamlineUnstructuredSettingDialog;

struct Post2dWindowUnstructuredStreamlineSetSetting {
};

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem  : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		/// Constructor
		Setting();
		/// Copy constructor
		Setting(const Setting& s);
		/// Copy operator
		Setting& operator=(const Setting& s);

		QPointFContainer point1;
		QPointFContainer point2;
		BoolContainer pointsSet;
		IntContainer numberOfPoints;
		ColorContainer color;
		IntContainer width;
	};

	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem();
	void showPropertyDialog() override;
	void assignActorZValues(const ZDepthRange& range) override;

protected:
	vtkPointSet* getSource(int i);
	void setupActors() override;

	QDialog* propertyDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent*, VTKGraphicsView*) override;
	void setSetting(const QPointF& v1, const QPointF& v2, int num);
	void hidePreviewSetting();

private slots:
	void exitDialogMode();

private:
	void setupDefaultValues();
	void setupTmpSource();
	std::vector<vtkUnstructuredGrid*> m_sourcePoints;
	vtkSmartPointer<vtkUnstructuredGrid> m_previewPoints;
	vtkSmartPointer<vtkDataSetMapper> m_previewMapper;
	vtkSmartPointer<vtkActor> m_previewActor;

	std::vector<Setting> m_unstSettings;

	Post2dWindowStreamlineUnstructuredSettingDialog* m_dialog;

	/// @name Members used for preview
	//@{
	QPointF m_point1;
	QPointF m_point2;
	int m_numberOfPoints;
	//@}

	class SetSettingCommand;

public:
	friend class Post2dWindowStreamlineUnstructuredSettingDialog;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
