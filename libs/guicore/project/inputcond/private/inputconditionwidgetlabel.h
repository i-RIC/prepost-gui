#ifndef INPUTCONDITIONWIDGETLABEL_H
#define INPUTCONDITIONWIDGETLABEL_H

#include "../inputconditionwidget.h"

#include <QString>

#include <map>

class InputConditionDependencyCheckSubCaptions;
class SolverDefinitionTranslator;

class QDir;
class QLabel;

class InputConditionWidgetLabel : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetLabel(QDomNode defNode, const SolverDefinitionTranslator& t);

	void addTooltip(const QString& tooltip) override;

	void setCaption(const QString& caption);
	void restoreCaption();

	InputConditionDependencyCheckSubCaptions* checkSubCaptions() const;

private:
	QLabel* m_label;
	QString m_caption;
	std::map<QString, QString> m_subCaptions;

	InputConditionDependencyCheckSubCaptions* m_checkSubCaptions;
};

#endif // INPUTCONDITIONWIDGETLABEL_H
