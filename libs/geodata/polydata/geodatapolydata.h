#ifndef GEODATAPOLYDATA_H
#define GEODATAPOLYDATA_H

#include "gd_polydata_global.h"

#include "geodatapolydatacolorsettingdialog.h"

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

	QColor color() const;

	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	const QVariant& variantValue() const;
	void setVariantValue(const QVariant& v, bool disableInform = false);

	bool getValueRange(double* min, double* max) override;
	void updateFilename() override;

	void setMapping(GeoDataPolyDataColorSettingDialog::Mapping m);
	void setOpacity(int opacity);
	void setColor(const QColor& color);

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
	GeoDataPolyDataColorSettingDialog::Setting colorSetting() const;
	void setColorSetting(GeoDataPolyDataColorSettingDialog::Setting setting);

	void setupValues();

	const std::vector<QVariant>& variantValues() const;
	std::vector<QVariant>& variantValues();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void emitNameAndValueEdited();

	virtual void updateScalarValues();
	virtual void updateActorSettings() = 0;
	virtual bool isReady() const = 0;
	virtual QString shapeName() const = 0;
	virtual QString shapeNameCamelCase() const = 0;

	class EditNameAndValueCommand;
	class SetColorSettingCommand;

	class Impl;
	Impl* impl;
};

#endif // GEODATAPOLYDATA_H
