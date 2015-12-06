#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorStreamlineDataItem::Post3dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post3dWindowNodeVectorStreamlineDataItem::~Post3dWindowNodeVectorStreamlineDataItem()
{}

void Post3dWindowNodeVectorStreamlineDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post3dWindowNodeVectorStreamlineDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
