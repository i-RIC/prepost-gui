#ifndef ORDEREDVALUESELECTDIALOG_H
#define ORDEREDVALUESELECTDIALOG_H

#include <QDialog>
#include <QString>

#include "misc_global.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace Ui {
class OrderedValueSelectDialog;
}

class MISCDLL_EXPORT OrderedValueSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit OrderedValueSelectDialog(QWidget *parent = nullptr);
	~OrderedValueSelectDialog();

	void setValues(const std::vector<std::string>& values, const std::unordered_map<std::string, QString>& captions);
	std::string selectedValue() const;

private:
	std::vector<std::string> m_values;

	Ui::OrderedValueSelectDialog *ui;
};

#endif // ORDEREDVALUESELECTDIALOG_H
