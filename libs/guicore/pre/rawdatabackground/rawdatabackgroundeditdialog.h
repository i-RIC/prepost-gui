#ifndef RAWDATABACKGROUNDEDITDIALOG_H
#define RAWDATABACKGROUNDEDITDIALOG_H

#include <QDialog>
#include <QVariant>
#include "rawdatabackground.h"

class GridRelatedConditionEditWidget;

namespace Ui
{
	class RawDataBackgroundEditDialog;
}

class RawDataBackgroundEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataBackgroundEditDialog(QWidget* parent = nullptr);
	~RawDataBackgroundEditDialog();
	RawDataBackground::Type type() const {return m_type;}
	void setType(RawDataBackground::Type t) {m_type = t;}
	const QVariant& customValue() const {return m_customValue;}
	void setCustomValue(const QVariant& v) {
		m_customValue = v;
	}
	void setWidget(GridRelatedConditionEditWidget* w);
	void setupDialog();
	void accept();

private slots:
	void updateCustomWidgetStatus();

private:
	Ui::RawDataBackgroundEditDialog* ui;
	RawDataBackground::Type m_type;
	QVariant m_customValue;
};

#endif // RAWDATABACKGROUNDEDITDIALOG_H
