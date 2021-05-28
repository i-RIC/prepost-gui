#ifndef CONTINUOUSSNAPSHOTSETTING_H
#define CONTINUOUSSNAPSHOTSETTING_H

#include <QList>
#include <QString>

class ContinuousSnapshotSetting
{
public:
	enum class FileOutputSetting {Onefile, Respectively};
	enum class OutputLayout {AsIs, Horizontally, Vertically};
	enum class MovieLengthMode {Length, FPS};
	enum class MovieProfile {MP4 = 0};

	ContinuousSnapshotSetting();

	// window and layout
	FileOutputSetting fileOutputSetting;
	OutputLayout outputLayout;

	// file properties
	QString exportTargetFolder;
	bool imageIsTransparent;
	QString imageExtention;
	int suffixLength;

	// timestep setting
	int startTimeStep;
	int stopTimeStep;
	int samplingRate;

	// movie output
	bool outputMovie;
	MovieLengthMode movieLengthMode;
	int movieFramesPerSeconds;
	int movieLengthSeconds;
	MovieProfile movieProfile;

	// google earth
	bool outputKml;
	QString kmlFilename;
	double angle;
	double north;
	double south;
	double east;
	double west;
};

#endif // CONTINUOUSSNAPSHOTSETTING_H
