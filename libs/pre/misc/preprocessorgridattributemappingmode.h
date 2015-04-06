#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGMODE_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGMODE_H

class PreProcessorGridAttributeMappingMode
{
public:
	enum Mode {
		mAuto,
		mManual
	};
	static Mode mode;

private:
	PreProcessorGridAttributeMappingMode();
};

#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGMODE_H
