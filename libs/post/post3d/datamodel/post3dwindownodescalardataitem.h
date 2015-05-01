#ifndef POST3DWINDOWNODESCALARDATAITEM_H
#define POST3DWINDOWNODESCALARDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridRelatedCondition;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post3dWindowNodeScalarDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowNodeScalarDataItem(const QString& name, const QString& caption, Post3dWindowDataItem* parent);
	void handleStandardItemChange();
	const QString& name() const {return m_name;}
signals:
	void changed(Post3dWindowNodeScalarDataItem*);
protected:
	void doLoadFromProjectMainFile(const QDomNode&) {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) {}
private:
	QString m_name;
};

#endif // POST3DWINDOWNODESCALARDATAITEM_H
