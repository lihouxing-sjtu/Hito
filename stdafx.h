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
#include <vtkTextRepresentation.h>
#include <vtkTextWidget.h>
#include <vtkTransform.h>
#include <vtkVectorText.h>
#include <vtkVolume.h>
#include <vtkVolumePicker.h>
#include <vtkVolumeProperty.h>
#include <vtkWorldPointPicker.h>

// itk include
#include "itkFRPROptimizer.h"
#include "itkGradientDifferenceMultiImageToImageMetric.h"
#include "itkMultiResolutionMultiImageToImageRegistrationMethod.h"
#include "itkNormalizedGradientCorrelationMultiImageToImageMetric.h"
#include "itkPatchedRayCastInterpolateImageFunction.h"
#include <itkCenteredEuler3DTransform.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkEuler3DTransform.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkGradientDifferenceImageToImageMetric.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegistrationMethod.h>
#include <itkImageRegistrationMethodv4.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkMeanSquaresImageToImageMetricv4.h>
#include <itkMultiResolutionImageRegistrationMethod.h>
#include <itkMultiResolutionPyramidImageFilter.h>
#include <itkNormalizedCorrelationImageToImageMetric.h>
#include <itkPowellOptimizerv4.h>
#include <itkRayCastInterpolateImageFunction.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkRegularStepGradientDescentOptimizerv4.h>
#include <itkResampleImageFilter.h>
#include <itkTranslationTransform.h>

#include <itkVTKImageToImageFilter.h>
#include <itkVersorRigid3DTransform.h>
#endif // STDAFX_H
