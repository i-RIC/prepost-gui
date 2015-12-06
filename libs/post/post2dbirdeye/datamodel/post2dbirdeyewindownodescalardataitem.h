#ifndef POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post2dBirdEyeWindowNodeScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowNodeScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarDataItem();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARDATAITEM_H
