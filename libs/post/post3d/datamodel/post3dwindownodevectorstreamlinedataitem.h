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
	Post3dWindowNodeVectorStreamlineDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const QString& name() const {return m_name;}

signals:
	void changed(Post3dWindowNodeVectorStreamlineDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}

private:
	QString m_name;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEDATAITEM_H
