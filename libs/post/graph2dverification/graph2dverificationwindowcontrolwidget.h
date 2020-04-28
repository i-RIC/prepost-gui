#ifndef GRAPH2DVERIFICATIONWINDOWCONTROLWIDGET_H
#define GRAPH2DVERIFICATIONWINDOWCONTROLWIDGET_H

#include <QWidget>
#include "graph2dverificationwindowresultsetting.h"

class QDomNode;
class QXmlStreamWriter;

namespace Ui
{
	class Graph2dVerificationWindowControlWidget;
}

class Graph2dVerificationWindowControlWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindowControlWidget(QWidget* parent = nullptr);
	~Graph2dVerificationWindowControlWidget();
	void setSetting(const Graph2dVerificationWindowResultSetting& setting);
	void setComment(const QString& comment);
	void fitSize();

signals:
	void settingButtonClicked();
	void exportButtonClicked();


private slots:
	void handleSettingButtonClicked();
	void handleExportButtonClicked();

private:
	Ui::Graph2dVerificationWindowControlWidget* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWCONTROLWIDGET_H
