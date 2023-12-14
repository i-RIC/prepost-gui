#ifndef INPUTCONDITIONWIDGETSET_H
#define INPUTCONDITIONWIDGETSET_H

#include "../../guicore_global.h"
#include <QMap>
#include <QObject>
#include <QVector>

#include <string>
#include <unordered_map>

class QWidget;
class QDomNode;
class InputConditionCgnsFile;
class InputConditionContainerSet;
class InputConditionWidget;
class SolverDefinition;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetSet : public QObject
{
	Q_OBJECT

public:
	InputConditionWidgetSet();
	~InputConditionWidgetSet();

	void setup(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC = false, bool noStretch = false);
	InputConditionWidget* widget(const std::string& name) const;

	bool checkImportSourceUpdate();
	void clear();
	void toggleReadOnly(bool readonly);
	void clearLabelAndImageCount();

	QString labelName(const QDomNode& itemNode);
	QString imageName(const QDomNode& itemNode);

private:
	void buildWidgets(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC, bool noStretch);
	void buildWidgetsCustom(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool noStretch);
	void buildWidgetsCustomRec(const QDomNode& node, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool noStretch);
	void buildWidget(QDomNode& itemNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool noStretch);
	void buildLabel(QDomNode& itemNode, const SolverDefinitionTranslator& t);
	void buildImage(QDomNode& itemNode, const SolverDefinition& def, const SolverDefinitionTranslator& t);
	void buildDeps(const QDomNode&, InputConditionContainerSet& cset, bool forBC);
	void buildDepsSimple(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustom(const QDomNode& tabNode, InputConditionContainerSet& cset);
	void buildDepsCustomRec(const QDomNode& node, InputConditionContainerSet& cset);
	void buildDepsItem(const QDomNode& itemNode, InputConditionContainerSet& cset);
	void buildDepsLabel(const QDomNode& labelNode, InputConditionContainerSet& cset);
	void buildDepsImage(const QDomNode& imageNode, InputConditionContainerSet& cset);
	void buildDepsOption(const QDomNode& imageNode, InputConditionContainerSet& cset);
	void buildDep(const QDomNode&, InputConditionContainerSet& cset, InputConditionWidget* w);
	void addTooltip(InputConditionWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t);

	InputConditionCgnsFile* getCgnsFile(const std::string& name);

	QMap<std::string, InputConditionCgnsFile*> m_cgnsFiles;
	QMap<std::string, InputConditionWidget*> m_widgets;

	std::unordered_map<QString, int> m_labelCountByCaption;
	std::unordered_map<QString, int> m_imageCountBySrc;

public:
	friend class InputConditionDialog;
	friend class InputConditionCgnsFileSelectDialog;
};
#endif // INPUTCONDITIONWIDGETSET_H
