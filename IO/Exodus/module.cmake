vtk_module(vtkIOExodus
  GROUPS
    StandAlone
  DEPENDS
    vtkFiltersGeneral
    vtkIOXML
    vtkexodusII
  TEST_DEPENDS
    vtkTestingRendering
    vtkInteractionStyle
    vtkRenderingOpenGL
  )