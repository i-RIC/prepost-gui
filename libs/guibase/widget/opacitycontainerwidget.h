#ifndef OPACITYCONTAINERWIDGET_H
#define OPACITYCONTAINERWIDGET_H

#include "../guibase_global.h"
#include "transparencywidget.h"

class OpacityContainer;

class GUIBASEDLL_EXPORT OpacityContainerWidget : public TransparencyWidget
{
	Q_OBJECT

public:
	OpacityContainerWidget(QWidget* parent = nullptr);

	void setContainer(OpacityContainer* container);

	QUndoCommand* createModifyCommand(bool apply = false);

private slots:
	void apply();

private:
	OpacityContainer* m_container;
};

#endif // OPACITYCONTAINERWIDGET_H
