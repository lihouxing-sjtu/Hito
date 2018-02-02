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

  // for knee
  vtkSmartPointer<vtkActor> m_moveKnee;
  vtkSmartPointer<vtkActor> m_fixKnee;
  vtkSmartPointer<vtkSphereWidget> m_KneeP1;
  vtkSmartPointer<vtkSphereWidget> m_KneeP2;
  vtkSmartPointer<vtkSphereWidget> m_KneeOrigion;

  double m_KneeLength;
  double m_KneeCenter[3];
  double m_KneeMoveCenter[3];
  bool m_KneeisFlip;

  // for ankle
  vtkSmartPointer<vtkActor> m_moveAnkle;
  vtkSmartPointer<vtkActor> m_fixAnkle;
  vtkSmartPointer<vtkSphereWidget> m_AnkleP1;
  vtkSmartPointer<vtkSphereWidget> m_AnkleP2;
  vtkSmartPointer<vtkSphereWidget> m_AnkleOrigion;
  double m_AnkleMoveCenter[3];
  double m_AnkleLength;
  double m_AnkleCenter[3];

  // for force line
  vtkSmartPointer<vtkActor> m_ForceLineActor;
  vtkSmartPointer<vtkActor> m_ExtensionLineActor;

  vtkSmartPointer<vtkFollower> m_AngleFollower;

  // for registration
  vtkSmartPointer<vtkActor> m_moveXrayActor;
  vtkSmartPointer<vtkActor> m_fixXrayActor;
  double m_XRayRegionMoveCenter[3];
  double m_XRayRegionFixCenter[3];
  double m_XRayLength;
  double m_XRayRegionPts[5][3];
  vtkSmartPointer<vtkSphereWidget> m_XrayRegionWidgetP1;
  vtkSmartPointer<vtkSphereWidget> m_XrayRegionWidgetP2;
  vtkSmartPointer<vtkSphereWidget> m_XrayRegionWidgetP3;
  vtkSmartPointer<vtkSphereWidget> m_XrayRegionWidgetP4;
  vtkSmartPointer<vtkSphereWidget> m_XRayRegionWidgetOrigon;

  vtkSmartPointer<vtkActor> m_moveCTActor;
  vtkSmartPointer<vtkActor> m_fixCTActor;
  double m_CTRegionMoveCenter[3];
  double m_CTRegionFixCenter[3];
  double m_CTLength;

  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP1;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP2;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP3;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP4;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP5;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP6;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP7;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetP8;
  vtkSmartPointer<vtkSphereWidget> m_CTRegionWidgetOrigion;
  QList<vtkSmartPointer<vtkSphereWidget>> m_CTSphereList;
  double m_CTRegionPts[9][3];

protected:
  virtual void closeEvent(QCloseEvent *event);
  void SetUpSphereWidet();
  void SetUpOrientationWidget();
  void SetUpCamera(int i, int j);
  void UpDateFemoralCircle();
  void GenerateDRR();
  void CalculateCircle(double p1[], double p2[], double p3[],
                       double outcenter[], double &outradius);
  void UpDateForceLine();
  bool isDataNull(double data[]);
  bool isEqual(double p1[], double p2[]);
  void BuildLine(double p1[], double p2[], vtkPolyData *out);
  void BuildLine(double p1[], double p2[], double p3[], vtkPolyData *out);
  void BuildExtensionLine(double p1[], double p2[], double p3[],
                          vtkPolyData *out);
  void BuildFrame(double p1[], double p2[], double p3[], double p4[],
                  vtkPolyData *out);
  void BuildDottedLine(double p1[], double p2[], double segment,
                       vtkPolyData *out);
  void BuildBox(double p1[], double p2[], double p3[], double p4[], double p5[],
                double p6[], double p7[], double p8[], vtkPolyData *out);
protected slots:
  void OnDICOMBrowser();
  void OnImportVolume(int index);
  void OnImportXRay(int index);
  void OnSetVolumeRep();
  void OnCloseWindow();
  void OnSetSurceWidget();
  void OnSurceWidgetInteraction();
  void OnGenerateDRR();

  // for femoral
  void OnFemoralCenterButton();
  void OnFemoralMove();
  void OnFemoralCircleChanged();
  void OnFemoralSet();
  void OnFemoralSideChange();
  void OnFemoralOrigionChange();

  // for kness
  void OnKneeCenterButton();
  void OnKneeMove();
  void OnKneeChanged();
  void OnKneeSet();
  void OnKneeSideChange();
  void OnKneeOrigionChange();

  // for ankle
  void OnAnkleCenterButton();
  void OnAnkleMove();
  void OnAnkleChanged();
  void OnAnkleSet();
  void OnAnkleSideChanged();
  void OnAnkleOrigionChange();

  // for x-ray region
  void OnXRayRegionButton();
  void OnXRayRegionMove();
  void OnXRayRegionLengthChanged();
  void OnXRayRegionSet();
  void OnXRayRegionSideChanged();

  // for ct region
  void OnCTRegionButton();
  void OnCTRegionMove();
  void OnCTRegionSet();
  void OnCTRegionChanged();
};

#endif // MAINWINDOW_H
