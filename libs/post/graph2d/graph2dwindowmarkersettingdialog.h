#ifndef GRAPH2DWINDOWMARKERSETTINGDIALOG_H
#define GRAPH2DWINDOWMARKERSETTINGDIALOG_H

#include "graph2dwindowmarkersetting.h"
#include <QDialog>

namespace Ui
{
	class Graph2dWindowMarkerSettingDialog;
}

class ColorSource;

class Graph2dWindowMarkerSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dWindowMarkerSettingDialog(QWidget* parent = nullptr);
	~Graph2dWindowMarkerSettingDialog();
	void setSetting(const Graph2dWindowMarkerSetting& setting);
	const Graph2dWindowMarkerSetting& setting() {return m_setting;}
	void setXAxisLabel(const QString& l) {m_xAxisLabel = l;}

private slots:
	void itemChanged(int index);
	void oriantationChanged();
	void valueChanged(double val);
	void labelChanged();
	void axisChanged();
	void styleChanged(int index);
	void colorChanged(const QColor& col);
	void showLineChanged(bool show);
	void alignCenterChanged(bool center);
	void addItem();
	void removeItem();

private:
	void setupList();
	void updateLabel();
	QString label(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item) const;
	QString autoLabel(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item) const;
	QString autoLabel() const;

	Graph2dWindowMarkerSetting m_setting;
	Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem* m_item;
	Ui::Graph2dWindowMarkerSettingDialog* ui;
	QString m_xAxisLabel;

	ColorSource* m_colorSource;
};

#endif // GRAPH2DWINDOWMARKERSETTINGDIALOG_H
