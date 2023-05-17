#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem();

	class SettingEditWidget;

public slots:
	void showPropertyDialog() override;

private:
	void setupActors() override;

	QDialog* propertyDialog(QWidget* parent) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void setupDefaultValues();

	class Impl;
	Impl* impl;

	class Setting;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
