#ifndef GEODATAPOLYDATA_H
#define GEODATAPOLYDATA_H

#include "gd_polydata_global.h"

#include <guicore/pre/geodata/geodatawithsinglemapper.h>

class QAction;
class QMenu;
class QToolBar;

class GD_POLYDATA_EXPORT GeoDataPolyData : public GeoDataWithSingleMapper
{
	Q_OBJECT

public:
	GeoDataPolyData(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);
	~GeoDataPolyData() override;

	void setCaptionAndEmitEdited(const QString& caption);
	void setVariantValueAndEmitEdited(const QVariant& value);

	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v, bool disableInform = false);

	bool getValueRange(double* min, double* max) override;
	void applyColorMapSetting() override;
	void updateFilename() override;

	QAction* editNameAction() const;
	QAction* editValueAction() const;
	QAction* editNameAndValueAction() const;
	QAction* editColorSettingAction() const;

	virtual void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) = 0;
	virtual bool inNormalMode() const = 0;
	virtual bool isDefined() const = 0;

signals:
	void nameAndValueEdited();

public slots:
	void editName();
	void editValue();
	void editNameAndValue();
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex) override;
	void handleDimensionValuesChange(const std::vector<QVariant>& before, const std::vector<QVariant>& after) override;

private slots:
	void editColorSetting();

protected:
	void setupValues();

	const std::vector<QVariant>& variantValues() const;
	std::vector<QVariant>& variantValues();

private:
	void emitNameAndValueEdited();

	virtual void updateScalarValues();
	virtual bool isReady() const = 0;
	virtual QString shapeName() const = 0;
	virtual QString shapeNameCamelCase() const = 0;

	class EditNameAndValueCommand;
	class SetColorSettingCommand;

	class Impl;
	Impl* impl;
};

#endif // GEODATAPOLYDATA_H
