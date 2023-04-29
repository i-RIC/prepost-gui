#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"

#include <vector>

class Post2dWindowStreamlineUnstructuredSettingDialog;

class QPointF;

struct Post2dWindowUnstructuredStreamlineSetSetting {
};

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem  : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem();

	void showPropertyDialog() override;
	void assignActorZValues(const ZDepthRange& range) override;

	class SettingEditWidget;

signals:
	void point1Changed(const QPointF point);
	void point2Changed(const QPointF point);

private:
	void setupPreviewActor();
	void setupActors() override;

	QDialog* propertyDialog(QWidget* parent) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent*, VTKGraphicsView*) override;
	void hidePreviewSetting();

	void setupDefaultValues();
	void updatePreview(const QPointF p1, const QPointF p2, int numPoints);
	std::vector<QPointF> generatePoints(const QPointF p1, const QPointF p2, int numPoints);
	void disablePreviewActor();

	class Impl;
	Impl* impl;

	class Setting;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_H
