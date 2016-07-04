#ifndef CONTINUOUSSNAPSHOTSETTING_H
#define CONTINUOUSSNAPSHOTSETTING_H


#include <vector>

class BackgroundImageInfo;

class ContinuousSnapshotSetting
{
public:
	enum class OutputType {Onefile, Respectively};
	enum class Layout {Asis, Horizontally, Vertically};
	enum class MovieLengthMode {LengthAsSeconds, FramesPerSecond};

	ContinuousSnapshotSetting();
	~ContinuousSnapshotSetting();

	OutputType outputType;
	Layout layout;
	QSize snapshotSize;
	QPoint beginPosition;
	bool isTransparent;

	// file properties
	QString fileIODirectory;
	QStringList prefixList;
	QString extension;
	int suffixLength;

	// movie properties
	bool isMovieOutput;
	MovieLengthMode movieLengthMode;
	int framesPerSecond;
	int movieLength;
	// @todo check this and fix!
	int movieProfile;

	// timestep setting
	std::vector<double> timeSteps;
	int start;
	int stop;
	int samplingRate;

	// google earth output setting
	bool isGoogleEarthOutput;
	double leftLatitude;
	double leftLongitude;
	double rightLatitude;
	double rightLongitude;
	int coordinateSystemTargetWindowIndex;
	int targetBackgroundImageIndex;
	QString kmlFileName;

	std::vector<BackgroundImageInfo*> backgroundList;

	// google earth related parameters that are automatically calculated
	double angleDegree;
	double north;
	double south;
	double east;
	double west;

	QStringList fileList;
};

#endif // CONTINUOUSSNAPSHOTSETTING_H
