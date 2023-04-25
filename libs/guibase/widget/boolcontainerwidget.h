#ifndef BOOLCONTAINERWIDGET_H
#define BOOLCONTAINERWIDGET_H

#include "../guibase_global.h"

#include <QWidget>

namespace Ui {
class BoolContainerWidget;
}

class BoolContainer;

class GUIBASEDLL_EXPORT BoolContainerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit BoolContainerWidget(QWidget *parent = nullptr);
	~BoolContainerWidget();

	void setText(const QString& text);
	void setContainer(BoolContainer* container);

	QUndoCommand* createModifyCommand(bool apply = false);

signals:
	void toggled(bool toggled);

private slots:
	void apply();

private:
	BoolContainer* m_container;
	Ui::BoolContainerWidget *ui;
};

#endif // BOOLCONTAINERWIDGET_H
