#ifndef WATERSURFACEELEVATIONPOINTS_H
#define WATERSURFACEELEVATIONPOINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class LeftBankHWM;
class RightBankHWM;
class ArbitraryHWM;

class WaterSurfaceElevationPoints : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	WaterSurfaceElevationPoints(DataItem* parent);
	~WaterSurfaceElevationPoints();

	const LeftBankHWM& leftBankHWM() const;
	LeftBankHWM& leftBankHWM();

	const RightBankHWM& rightBankHWM() const;
	RightBankHWM& rightBankHWM();

	const ArbitraryHWM& arbitraryHWM() const;
	ArbitraryHWM& arbitraryHWM();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/watersurfaceelevationpoints_impl.h"
#endif // _DEBUG

#endif // WATERSURFACEELEVATIONPOINTS_H
