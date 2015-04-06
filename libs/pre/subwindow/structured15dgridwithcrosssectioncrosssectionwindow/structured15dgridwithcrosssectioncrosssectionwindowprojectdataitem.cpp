#include "structured15dgridwithcrosssectioncrosssectionwindowprojectdataitem.h"
#include "structured15dgridwithcrosssectioncrosssectionwindow.h"
#include "../../datamodel/preprocessornormal15dgridwithcrosssectionshapedataitem.h"

Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem(PreProcessorNormal15DGridWithCrossSectionShapeDataItem* item, QWidget* parentWindow)
	: ProjectDataItem(item)
{
	Structured15DGridWithCrossSectionCrossSectionWindow* w = new Structured15DGridWithCrossSectionCrossSectionWindow(item, this, parentWindow);
	m_window = w;
}

Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::~Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem()
{
	if (m_window != 0){
		Structured15DGridWithCrossSectionCrossSectionWindow* w = m_window;
		m_window = 0;
		delete w->parent();
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::informWindowClose()
{
	if (m_window != 0){
		m_window = 0;
		dynamic_cast<PreProcessorNormal15DGridWithCrossSectionShapeDataItem*>(parent())->requestCrossSectionWindowDelete(this);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::requestWindowClose()
{
	dynamic_cast<PreProcessorNormal15DGridWithCrossSectionShapeDataItem*>(parent())->requestCrossSectionWindowDelete(this);
}


void Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
