#include "continuoussnapshotsetting.h"

ContinuousSnapshotSetting::ContinuousSnapshotSetting() :
	fileOutputSetting {FileOutputSetting::Onefile},
	outputLayout {OutputLayout::AsIs},

	exportTargetFolder {},
	imageIsTransparent {false},
	imageExtention {".png"},
	suffixLength {4},

	startTimeStep {-1},
	stopTimeStep {-1},
	samplingRate {1},

	outputMovie {false},
	movieLengthMode {MovieLengthMode::Length},
	movieFramesPerSeconds {5},
	movieLengthSeconds {10},
	movieProfile {MovieProfile::MP4},

	outputKml {false},
	kmlFilename {"output.kml"},
	angle {0},
	north {0},
	south {0},
	east {0},
	west {0}
{}
