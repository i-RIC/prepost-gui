#ifndef INPUTCONDITIONWIDGET_H
#define INPUTCONDITIONWIDGET_H

#include "../../guicore_global.h"
#include "inputconditioncontainer.h"

#include <QList>
#include <QWidget>
#include <QDomNode>

#include <vector>

class InputConditionDependency;

class GUICOREDLL_EXPORT InputConditionWidget : public QWidget
{
	Q_OBJECT

public:
	InputConditionWidget(QDomNode n);
	virtual ~InputConditionWidget();

	void addDependency(InputConditionDependency* dep);
	const std::vector<InputConditionDependency*>& dependencies() const;

	virtual void addTooltip(const QString& tooltip) = 0;
	virtual bool checkImportSourceUpdate();
	virtual void toggleReadOnly(bool readonly);

	static const int margin = 0;
	static QList<QDomNode> getEnums(QDomNode defNode);
	static bool hasEnums(QDomNode defNode);

private:
	std::vector<InputConditionDependency*> m_dependencies;
};
#endif // INPUTCONDITIONWIDGET_H
