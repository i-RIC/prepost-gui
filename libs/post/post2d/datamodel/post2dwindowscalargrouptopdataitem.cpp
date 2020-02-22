#include "post2dwindowscalargrouptopdataitem.h"

#include "../scalarbari.h"

#include <guibase/scalarbarsetting.h>

#include <QMessageBox>

Post2dWindowScalarGroupTopDataItem::Post2dWindowScalarGroupTopDataItem(GraphicsWindowDataItem* parent) :
    Post2dWindowDataItem(parent)
{}

Post2dWindowScalarGroupTopDataItem::Post2dWindowScalarGroupTopDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
    Post2dWindowDataItem(itemlabel, parent)
{}

Post2dWindowScalarGroupTopDataItem::Post2dWindowScalarGroupTopDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
    Post2dWindowDataItem(itemlabel, icon, parent)
{}

Post2dWindowScalarGroupTopDataItem::~Post2dWindowScalarGroupTopDataItem()
{}

QMap<std::string, QString>& Post2dWindowScalarGroupTopDataItem::colorbarTitleMap()
{
	return m_colorbarTitleMap;
}

bool Post2dWindowScalarGroupTopDataItem::nextScalarBarSetting(ScalarBarSetting& scalarBarSetting)
{
	std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();

	for (auto item : m_childItems) {
		if (ScalarbarI* sbi = dynamic_cast<ScalarbarI*>(item)) {
			if (sbi->quadrant() != ScalarBarSetting::Quadrant::None) {
				auto it = quads.find(sbi->quadrant());
				if (it != quads.end()) {
					quads.erase(it);
				}
			}
		}
	}

	if (quads.empty()) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return false;
	}

	scalarBarSetting.setDefaultPosition(*quads.begin());
	return true;
}
