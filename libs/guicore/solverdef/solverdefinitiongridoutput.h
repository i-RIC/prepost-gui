#ifndef SOLVERDEFINITIONGRIDOUTPUT_H
#define SOLVERDEFINITIONGRIDOUTPUT_H

#include "../guicore_global.h"
#include "solverdefinitionoutput.h"

class ColorMapFactoryI;
class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class ColorMapSettingEditDialog;
class ColorMapSettingEditWidgetI;
class ColorMapSettingToolBarWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridOutput : public SolverDefinitionOutput
{
public:
	enum Position {
		Node,           ///< Node (vertex)
		CellCenter,      ///< Cell center
		EdgeI,
		EdgeJ,
		EdgeK,
		Particle,
		PolyData
	};

	SolverDefinitionGridOutput(const std::string& name);
	SolverDefinitionGridOutput(const QDomElement& elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator);
	~SolverDefinitionGridOutput();

	Position position() const;

	ColorMapSettingContainerI* createColorMapSettingContainer() const;
	ColorMapLegendSettingContainerI* createColorMapLegendSettingContainer() const;
	ColorMapSettingEditWidgetI* createColorMapSettingEditWidget(QWidget* parent) const;
	ColorMapSettingEditDialog* createColorMapSettingEditDialog(QWidget* parent) const;
	ColorMapSettingToolBarWidget* createColorMapSettingToolbarWidget(QWidget* parent) const;

protected:
	void setColorMapFactory(ColorMapFactoryI* factory);
	void setDefaultColorMapSetting(const QDomElement& elem);

private:
	virtual void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const;

	class Impl;
	Impl* impl;
};

#endif // SOLVERDEFINITIONGRIDOUTPUT_H
