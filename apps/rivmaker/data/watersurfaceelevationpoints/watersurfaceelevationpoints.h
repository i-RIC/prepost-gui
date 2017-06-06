#ifndef WATERSURFACEELEVATIONPOINTS_H
#define WATERSURFACEELEVATIONPOINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class ArbitraryHWM;
class Benchmark;
class Hub;
class LeftBankHWM;
class ReferenceMark;
class RightBankHWM;
class StreamGage;

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

	const Benchmark& benchmark() const;
	Benchmark& benchmark();

	const Hub& hub() const;
	Hub& hub();

	const ReferenceMark& referenceMark() const;
	ReferenceMark& referenceMark();

	const StreamGage& streamGage() const;
	StreamGage& streamGage();

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
