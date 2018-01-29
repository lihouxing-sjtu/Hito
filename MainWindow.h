#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "DICOMBrowser.h"
#include "stdafx.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_GenericRendWin2D;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_GenericRendWin3D;
  vtkSmartPointer<vtkRenderer> m_Render2D;
  vtkSmartPointer<vtkRenderer> m_Render3D;
  vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor2D;
  vtkSmartPointer<vtkRenderWindowInteractor> m_Interactor3D;

  vtkSmartPointer<vtkInteractorStyleImage> m_Style2D;
  vtkSmartPointer<vtkInteractorStyleTrackballCamera> m_Style3D;

  vtkSmartPointer<vtkImageData> m_imageVolume;
  vtkSmartPointer<vtkImageData> m_imageXRay;

  vtkSmartPointer<vtkEventQtSlotConnect> m_vtkqtConnector;

  double m_volumeCenter[3];
  double m_volumeOrigin[3];
  double m_volumeSpacing[3];
  int m_volumeExtend[6];
  // for volume rendering
  vtkSmartPointer<vtkPiecewiseFunction> m_opacityFunction;
  vtkSmartPointer<vtkPiecewiseFunction> m_gradientFunction;
  vtkSmartPointer<vtkColorTransferFunction> m_colortransformFunction;
  vtkSmartPointer<vtkVolumeProperty> m_volumeProperty;
  vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> m_volumeMapper;
  vtkSmartPointer<vtkVolume> m_volumeActor;

  // for drr
  vtkSmartPointer<vtkSphereWidget> m_surceSphereWidget;
  DICOMBrowser *m_dicomBrowser;
  vtkSmartPointer<vtkImageActor> m_drr3Dactor;
  vtkSmartPointer<vtkImageActor> m_drr2Dactor;

  // 2D origon
  vtkSmartPointer<vtkImageSlice> m_slice2D;
  vtkSmartPointer<vtkImageSliceMapper> m_sliceMapper2D;

  vtkSmartPointer<vtkImageActor> m_origon2D;

  vtkSmartPointer<vtkOrientationMarkerWidget> m_orientation2D;

  // for femoral
  vtkSmartPointer<vtkActor> m_moveFemoral;
  vtkSmartPointer<vtkActor> m_fixFemoral;

  double m_FemoralRadius;
  double m_FemoralCenter[3];
  double m_FemoralMoveCenter[3];
  vtkSmartPointer<vtkSphereWidget> m_FemoralSide1;
  vtkSmartPointer<vtkSphereWidget> m_FemoralSide2;
  vtkSmartPointer<vtkSphereWidget> m_FemoralSide3;

  vtkSmartPointer<vtkSphereWidget> m_FemoralOrigion;

protected:
  virtual void closeEvent(QCloseEvent *event);
  void SetUpSphereWidet();
  void SetUpOrientationWidget();
  void SetUpCamera(int i, int j);
  void UpDateFemoralCircle();
  void GenerateDRR();
  void CalculateCircle(double p1[], double p2[], double p3[],
                       double outcenter[], double &outradius);
protected slots:
  void OnDICOMBrowser();
  void OnImportVolume(int index);
  void OnImportXRay(int index);
  void OnSetVolumeRep();
  void OnCloseWindow();
  void OnSetSurceWidget();
  void OnSurceWidgetInteraction();
  void OnGenerateDRR();

  void OnFemoralCenterButton();
  void OnFemoralMove();
  void OnFemoralCircleChanged();
  void OnFemoralSet();
  void OnFemoralSideChange();
  void OnFemoralOrigionChange();
};

#endif // MAINWINDOW_H
