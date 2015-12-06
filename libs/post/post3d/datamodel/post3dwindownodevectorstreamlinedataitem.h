#ifndef POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class QAction;
class QColor;

class Post3dWindowNodeVectorStreamlineDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
	~Post3dWindowNodeVectorStreamlineDataItem();

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
