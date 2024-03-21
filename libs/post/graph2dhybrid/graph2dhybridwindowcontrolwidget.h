#ifndef GRAPH2DHYBRIDWINDOWCONTROLWIDGET_H
#define GRAPH2DHYBRIDWINDOWCONTROLWIDGET_H

#include <QWidget>
#include "graph2dhybridwindowresultsetting.h"

class QDomNode;
class QXmlStreamWriter;

namespace Ui
{
	class Graph2dHybridWindowControlWidget;
}

class Graph2dHybridWindowControlWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindowControlWidget(QWidget* parent = nullptr);
	~Graph2dHybridWindowControlWidget();
	void setSetting(const Graph2dHybridWindowResultSetting& setting, int dim[4]);
	void setIValue(int i);
	void setJValue(int j);
	void setKValue(int k);
	void setIndexValue(int index);
	void setPolyline(GeoDataPolyLineGroupPolyLine* line);
	int iValue() const;
	int jValue() const;
	int kValue() const;
	int indexValue() const;
	GeoDataPolyLineGroupPolyLine* polyline() const;
	void fitSize();
	void loadFromProjectMainFile(const QDomNode& node);
	void saveToProjectMainFile(QXmlStreamWriter& writer) const;

signals:
	void dataSourceButtonClicked();
	void axisButtonClicked();
	void drawButtonClicked();
	void markerButtonClicked();
	void fontButtonClicked();
	void copyButtonClicked();
	void snapshotButtonClicked();
	void csvExportButtonClicked();
	void iValueChanged(int i);
	void jValueChanged(int j);
	void kValueChanged(int k);
	void indexValueChanged(int index);
	void polyLineChanged();

private:
	bool m_setupFinished;
	std::vector<GeoDataPolyLineGroupPolyLine*> m_lines;
	Ui::Graph2dHybridWindowControlWidget* ui;
};

#endif // GRAPH2DHYBRIDWINDOWCONTROLWIDGET_H
