#ifndef STDAFX_H
#define STDAFX_H
#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingOpenGL2)

// Qt include
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QVTKOpenGLWidget.h>
// vtk include

#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAppendPolyData.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCaptionActor2D.h>
#include <vtkColorTransferFunction.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkFollower.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleImage.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropAssembly.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkSphereWidget.h>
#include <vtkTextActor.h>
#include <vtkTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkVectorText.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkWorldPointPicker.h>
// itk include
#include <itkCenteredEuler3DTransform.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkRayCastInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkVTKImageToImageFilter.h>
#endif // STDAFX_H
