#ifndef GRAPH2DSCATTEREDWINDOWDATASOURCEDIALOG_H
#define GRAPH2DSCATTEREDWINDOWDATASOURCEDIALOG_H

#include "graph2dscatteredwindowresultsetting.h"
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QDialog>

class PostZoneDataContainer;

namespace Ui
{
	class Graph2dScatteredWindowDataSourceDialog;
}

class Graph2dScatteredWindowDataSourceDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dScatteredWindowDataSourceDialog(QWidget* parent = nullptr);
	~Graph2dScatteredWindowDataSourceDialog();
	void setDimension(PostSolutionInfo::Dimension dim);
	void setZoneDataContainer(PostZoneDataContainer* cont);

	void setSetting(Graph2dScatteredWindowResultSetting& setting);
	Graph2dScatteredWindowResultSetting setting() const;

private:
	void setup();

	PostSolutionInfo::Dimension m_dimension;
	PostZoneDataContainer* m_container;
	Graph2dScatteredWindowResultSetting m_setting;

	Ui::Graph2dScatteredWindowDataSourceDialog* ui;

	QList<Graph2dScatteredWindowResultSetting::Setting*> m_initialSettings;

	QList<std::string> m_xAxisValues;
	QList<std::string> m_yAxisValues;
};

#endif // GRAPH2DSCATTEREDWINDOWDATASOURCEDIALOG_H
