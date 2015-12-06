#ifndef POST3DWINDOWNODESCALARDATAITEM_H
#define POST3DWINDOWNODESCALARDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <guibase/contoursettingwidget.h>

class SolverDefinitionGridAttribute;
class PostZoneDataContainer;
class QAction;
class vtkMapper;
class vtkStructuredGrid;

class Post3dWindowNodeScalarDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent);
	void handleStandardItemChange() override;
	const std::string& name() const {return m_name;}

signals:
	void changed(Post3dWindowNodeScalarDataItem*);

protected:
	void doLoadFromProjectMainFile(const QDomNode&) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter&) override {}

private:
	std::string m_name;
};

#endif // POST3DWINDOWNODESCALARDATAITEM_H
