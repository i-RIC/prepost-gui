/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unordered_set>
#include <vector>

#include <hdf5.h>

// #include <ogr_spatialref.h>

#include <QAbstractTableModel>
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QBrush>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QCursor>
#include <QDataStream>
#include <QDate>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QDockWidget>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGridLayout>
#include <QGroupBox>
#include <QHash>
#include <QHBoxLayout>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QInputEvent>
#include <QItemDelegate>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QItemSelectionRange>
#include <QJSEngine>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QLineF>
#include <QList>
#include <QLocale>
#include <QLockFile>
#include <QMainWindow>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QPolygonF>
#include <QProcess>
#include <QProgressDialog>
#include <QPushButton>
#include <QRect>
#include <QRectF>
#include <QRegExp>
#include <QSet>
#include <QSettings>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QTableView>
#include <QtCore/qbytearray.h>
#include <QtCore/qglobal.h>
#include <QtCore/QList>
#include <QtCore/qmetatype.h>
#include <QtCore/QVariant>
#include <QtDebug>
#include <QTextCodec>
#include <QTextStream>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QTimerEvent>
#include <QTimeZone>
#include <QtPlugin>
#include <QTreeView>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QUndoCommand>
#include <QUrl>
#include <QUrlQuery>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVTKWidget.h>
#include <QWheelEvent>
#include <QWidget>
#include <QXmlStreamWriter>

//#include <qwt_legend.h>
//#include <qwt_plot.h>
//#include <qwt_plot_curve.h>
//#include <qwt_scale_engine.h>

// #include <shapefil.h>

#include <tmsloader/tmsloader.h>
#include <tmsloader/tmsrequest.h>
#include <tmsloader/tmsrequestbing.h>
#include <tmsloader/tmsrequestgooglemap.h>
#include <tmsloader/tmsrequestgsi.h>
#include <tmsloader/tmsrequestopenstreetmap.h>
#include <tmsloader/tmsrequestxyz.h>
#include <tmsloader/tmsutil.h>

//#include <triangle/triangle.h>
//#include <triangle/triangleexecutethread.h>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkAlgorithm.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCollectionIterator.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkExtractVOI.h>
#include <vtkFloatArray.h>
#include <vtkGeometryFilter.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include <vtkImageData.h>
#include <vtkImageMapper.h>
#include <vtkImageReader2.h>
#include <vtkIntArray.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleRubberBandZoom.h>
#include <vtkJPEGReader.h>
#include <vtkLightKit.h>
#include <vtkLine.h>
#include <vtkMapperCollection.h>
#include <vtkMaskPoints.h>
#include <vtkMaskPolyData.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkPlaneSource.h>
#include <vtkPNGReader.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPointSet.h>
#include <vtkPolyData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkQImageToImageSource.h>
#include <vtkQuad.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridWriter.h>
#include <vtkTextSource.h>
#include <vtkTexture.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTIFFReader.h>
#include <vtkTriangle.h>
#include <vtkTrivialProducer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkVectorText.h>
#include <vtkVertex.h>

#include <yaml-cpp/yaml.h>

#endif // __cplusplus