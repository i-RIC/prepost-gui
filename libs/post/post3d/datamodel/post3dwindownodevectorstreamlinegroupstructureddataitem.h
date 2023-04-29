#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H

#include "post3dwindownodevectorstreamlinegroupdataitem.h"

class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem : public Post3dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorStreamlineGroupStructuredDataItem();

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

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
