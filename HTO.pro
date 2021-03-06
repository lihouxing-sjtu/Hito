#-------------------------------------------------
#
# Project created by QtCreator 2018-01-07T10:45:04
#
#-------------------------------------------------
include(vtk810.pri)
include(itk413vtk810.pri)
include(occt.pri)
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hito
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp \
    DICOMBrowser.cpp \
    DICOMItem.cpp \
    itkGradientDifferenceSingleImageToImageMetric.txx \
    itkMultiImageToImageMetric.txx \
    itkMultiImageToImageRegistrationMethod.txx \
    itkMultiResolutionMultiImageToImageRegistrationMethod.txx \
    itkNormalizedGradientCorrelationImageToImageMetric.txx \
    itkPatchedRayCastInterpolateImageFunction.txx \
    itkPatternIntensityImageToImageMetric.txx \
    itkPatternIntensityMultiImageToImageMetric.txx \
    TYLineWidget.cpp \
    TYCircleWidget.cpp

HEADERS += \
        MainWindow.h \
    stdafx.h \
    DICOMBrowser.h \
    DICOMItem.h \
    itkGradientDifferenceMultiImageToImageMetric.h \
    itkGradientDifferenceSingleImageToImageMetric.h \
    itkMeanSquaresMultiImageToImageMetric.h \
    itkMultiImageRegistrationMacro.h \
    itkMultiImageToImageMetric.h \
    itkMultiImageToImageRegistrationMethod.h \
    itkMultiResolutionMultiImageToImageRegistrationMethod.h \
    itkNormalizedGradientCorrelationImageToImageMetric.h \
    itkNormalizedGradientCorrelationMultiImageToImageMetric.h \
    itkPatchedRayCastInterpolateImageFunction.h \
    itkPatternIntensityImageToImageMetric.h \
    itkPatternIntensityMultiImageToImageMetric.h \
    TYLineWidget.h \
    TYCircleWidget.h

FORMS += \
        MainWindow.ui \
    DICOMBrowser.ui \
    TYLineWidget.ui \
    TYCircleWidget.ui

RESOURCES += \
    resource.qrc

RC_ICONS =qrc/Hito.ico

DISTFILES += \
    occt.pri
