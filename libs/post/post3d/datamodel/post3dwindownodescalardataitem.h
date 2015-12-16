#ifndef POST3DWINDOWNODESCALARDATAITEM_H
#define POST3DWINDOWNODESCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowNodeScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWNODESCALARDATAITEM_H
