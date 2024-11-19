#ifndef __iricGeometryPartialCellFilter_h
#define __iricGeometryPartialCellFilter_h

#include "guibase_global.h"
#include "vtkPolyDataAlgorithm.h"

class vtkIncrementalPointLocator;

class GUIBASEDLL_EXPORT iricGeometryPartialCellFilter : public vtkPolyDataAlgorithm
{
public:
  static iricGeometryPartialCellFilter *New();
  vtkTypeMacro(iricGeometryPartialCellFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Turn on/off selection of geometry by point id.
   */
  vtkSetMacro(PointClipping,vtkTypeBool);
  vtkGetMacro(PointClipping,vtkTypeBool);
  vtkBooleanMacro(PointClipping,vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off selection of geometry by cell id.
   */
  vtkSetMacro(CellClipping,vtkTypeBool);
  vtkGetMacro(CellClipping,vtkTypeBool);
  vtkBooleanMacro(CellClipping,vtkTypeBool);
  //@}

  //@{
  /**
   * Turn on/off selection of geometry via bounding box.
   */
  vtkSetMacro(ExtentClipping,vtkTypeBool);
  vtkGetMacro(ExtentClipping,vtkTypeBool);
  vtkBooleanMacro(ExtentClipping,vtkTypeBool);
  //@}

  //@{
  /**
   * Specify the minimum point id for point id selection.
   */
  vtkSetClampMacro(PointMinimum,vtkIdType,0,VTK_ID_MAX);
  vtkGetMacro(PointMinimum,vtkIdType);
  //@}

  //@{
  /**
   * Specify the maximum point id for point id selection.
   */
  vtkSetClampMacro(PointMaximum,vtkIdType,0,VTK_ID_MAX);
  vtkGetMacro(PointMaximum,vtkIdType);
  //@}

  //@{
  /**
   * Specify the minimum cell id for point id selection.
   */
  vtkSetClampMacro(CellMinimum,vtkIdType,0,VTK_ID_MAX);
  vtkGetMacro(CellMinimum,vtkIdType);
  //@}

  //@{
  /**
   * Specify the maximum cell id for point id selection.
   */
  vtkSetClampMacro(CellMaximum,vtkIdType,0,VTK_ID_MAX);
  vtkGetMacro(CellMaximum,vtkIdType);
  //@}

  /**
   * Specify a (xmin,xmax, ymin,ymax, zmin,zmax) bounding box to clip data.
   */
  void SetExtent(double xMin, double xMax, double yMin, double yMax,
                 double zMin, double zMax);

  //@{
  /**
   * Set / get a (xmin,xmax, ymin,ymax, zmin,zmax) bounding box to clip data.
   */
  void SetExtent(double extent[6]);
  double *GetExtent() VTK_SIZEHINT(6) { return this->Extent;};
  //@}

  //@{
  /**
   * Turn on/off merging of coincident points. Note that is merging is
   * on, points with different point attributes (e.g., normals) are merged,
   * which may cause rendering artifacts.
   */
  vtkSetMacro(Merging,vtkTypeBool);
  vtkGetMacro(Merging,vtkTypeBool);
  vtkBooleanMacro(Merging,vtkTypeBool);
  //@}

  //@{
  /**
   * Set / get a spatial locator for merging points. By
   * default an instance of vtkMergePoints is used.
   */
  void SetLocator(vtkIncrementalPointLocator *locator);
  vtkGetObjectMacro(Locator,vtkIncrementalPointLocator);
  //@}

  /**
   * Create default locator. Used to create one when none is specified.
   */
  void CreateDefaultLocator();

  /**
   * Return the MTime also considering the locator.
   */
  vtkMTimeType GetMTime() override;

  //@{
  /**
   * Set/get the desired precision for the output types. See the documentation
   * for the vtkAlgorithm::DesiredOutputPrecision enum for an explanation of
   * the available precision settings. This only applies for data types where
   * we create points as opposed to pass them, such as rectilinear grid.
   */
  void SetOutputPointsPrecision(int precision);
  int GetOutputPointsPrecision() const;
  //@}

protected:
  iricGeometryPartialCellFilter();
  ~iricGeometryPartialCellFilter() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation(int port, vtkInformation *info) override;

  //special cases for performance
  void PolyDataExecute(vtkDataSet *, vtkPolyData *);
  void UnstructuredGridExecute(vtkDataSet *, vtkPolyData *);
  void StructuredGridExecute(vtkDataSet *, vtkPolyData *, vtkInformation *);
  int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  vtkIdType PointMaximum;
  vtkIdType PointMinimum;
  vtkIdType CellMinimum;
  vtkIdType CellMaximum;
  double Extent[6];
  vtkTypeBool PointClipping;
  vtkTypeBool CellClipping;
  vtkTypeBool ExtentClipping;
  int OutputPointsPrecision;

  vtkTypeBool Merging;
  vtkIncrementalPointLocator *Locator;
private:
  iricGeometryPartialCellFilter(const iricGeometryPartialCellFilter&) = delete;
  void operator=(const iricGeometryPartialCellFilter&) = delete;
};

#endif
