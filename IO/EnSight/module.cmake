vtk_module(vtkIOEnSight
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkRendering${VTK_RENDERING_BACKEND}
  KIT
    vtkIO
  DEPENDS
    vtkCommonExecutionModel
  PRIVATE_DEPENDS
    vtkCommonCore
    vtkCommonDataModel
  )