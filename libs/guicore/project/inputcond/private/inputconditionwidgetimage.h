#ifndef INPUTCONDITIONWIDGETIMAGE_H
#define INPUTCONDITIONWIDGETIMAGE_H

#include "../inputconditionwidget.h"

#include <QPixmap>
#include <QString>

#include <map>

class InputConditionDependencyCheckSubimages;
class SolverDefinitionTranslator;

class QDir;
class QLabel;

class InputConditionWidgetImage : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetImage(QDomNode defNode, const SolverDefinitionTranslator& t, const QDir& dir);

	void addTooltip(const QString& tooltip) override;

	void activateSubImage(const QString& name);
	void inactivateSubImages();

	InputConditionDependencyCheckSubimages* checkSubImages() const;

private:
	QLabel* m_label;
	QPixmap m_image;
	std::map<QString, QPixmap> m_subImages;

	InputConditionDependencyCheckSubimages* m_checkSubImages;
};

#endif // INPUTCONDITIONWIDGETIMAGE_H
