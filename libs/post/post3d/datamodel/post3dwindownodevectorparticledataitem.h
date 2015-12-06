#ifndef POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include "../post3dwindowdataitem.h"

class QAction;
class QColor;

class Post3dWindowNodeVectorParticleDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent);
	const std::string& name() const;

	void handleStandardItemChange() override;

signals:
	void changed(Post3dWindowNodeVectorParticleDataItem*);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;

	std::string m_name;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
