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
	void setIValue(int i);
	void setJValue(int j);
	void setKValue(int k);
	void setIndexValue(int index);
	int iValue() const;
	int jValue() const;
	int kValue() const;
	int indexValue() const;
	void fitSize();
	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer) const;

signals:
	void dataSourceButtonClicked();
	void axisButtonClicked();
	void drawButtonClicked();
	void markerButtonClicked();
	void copyButtonClicked();
	void snapshotButtonClicked();
	void csvExportButtonClicked();
	void iValueChanged(int i);
	void jValueChanged(int j);
	void kValueChanged(int k);
	void indexValueChanged(int index);

	void settingButtonClicked();
	void exportButtonClicked();


private slots:
	void handleSettingButtonClicked();
	void handleExportButtonClicked();

private:
	Ui::Graph2dVerificationWindowControlWidget* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWCONTROLWIDGET_H
