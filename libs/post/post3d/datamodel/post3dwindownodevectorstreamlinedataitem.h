#ifndef POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H

#include "../post3dwindowdataitem.h"

class QAction;
class QColor;

class Post3dWindowNodeVectorStreamlineDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent);
	const std::string& name() const;
	void handleStandardItemChange() override;

signals:
	void changed(Post3dWindowNodeVectorStreamlineDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
