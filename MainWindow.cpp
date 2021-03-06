#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  // QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
  ui->setupUi(this);

  m_vtkqtConnector = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  m_GenericRendWin2D = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
  m_GenericRendWin3D = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

  ui->Widget2D->SetRenderWindow(m_GenericRendWin2D);
  ui->Widget3D->SetRenderWindow(m_GenericRendWin3D);

  m_Render2D = vtkSmartPointer<vtkRenderer>::New();
  m_GenericRendWin2D->AddRenderer(m_Render2D);
  m_Interactor2D = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_GenericRendWin2D->SetInteractor(m_Interactor2D);
  m_Style2D = vtkSmartPointer<vtkInteractorStyleImage>::New();
  m_Interactor2D->SetInteractorStyle(m_Style2D);

  m_Render3D = vtkSmartPointer<vtkRenderer>::New();
  m_Render3D->GradientBackgroundOn();
  m_Render3D->SetBackground(1, 1, 1);
  m_Render3D->SetBackground2(0, 1, 1);

  m_GenericRendWin3D->AddRenderer(m_Render3D);
  m_Interactor3D = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  m_GenericRendWin3D->SetInteractor(m_Interactor3D);
  m_Style3D = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
  m_Interactor3D->SetInteractorStyle(m_Style3D);
  m_GenericRendWin3D->Render();
  m_Interactor3D->Initialize();
  m_Interactor2D->Initialize();

  m_imageVolume = vtkSmartPointer<vtkImageData>::New();
  m_imageXRay = vtkSmartPointer<vtkImageData>::New();

  // for volume rendering
  m_opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
  m_gradientFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
  m_colortransformFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
  m_volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
  m_volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
  m_volumeActor = vtkSmartPointer<vtkVolume>::New();
  m_volumeProperty->SetColor(m_colortransformFunction);
  m_volumeProperty->SetScalarOpacity(m_opacityFunction);
  m_volumeProperty->SetGradientOpacity(m_gradientFunction);
  m_volumeProperty->ShadeOn();
  m_volumeProperty->SetInterpolationTypeToLinear();
  m_volumeMapper->SetSampleDistance(.5);
  m_volumeMapper->SetAutoAdjustSampleDistances(1);
  m_volumeMapper->SetBlendModeToComposite();
  m_volumeActor->SetProperty(m_volumeProperty);
  m_volumeActor->SetMapper(m_volumeMapper);
  m_dicomBrowser = new DICOMBrowser();

  // for drr
  m_surceSphereWidget = vtkSmartPointer<vtkSphereWidget>::New();
  m_drr2Dactor = vtkSmartPointer<vtkImageActor>::New();
  m_drr3Dactor = vtkSmartPointer<vtkImageActor>::New();

  // 2D origon
  m_origon2D = vtkSmartPointer<vtkImageActor>::New();
  m_sliceMapper2D = vtkSmartPointer<vtkImageSliceMapper>::New();
  m_slice2D = vtkSmartPointer<vtkImageSlice>::New();

  m_orientation2D = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

  // for femoral
  m_moveFemoral = vtkSmartPointer<vtkActor>::New();
  m_moveFemoral->GetProperty()->SetColor(1, 0, 0);
  m_moveFemoral->GetProperty()->SetLineWidth(5);
  m_Render2D->AddActor(m_moveFemoral);
  m_fixFemoral = vtkSmartPointer<vtkActor>::New();
  m_fixFemoral->GetProperty()->SetColor(1, 0, 0);
  m_fixFemoral->GetProperty()->SetLineWidth(5);
  m_Render2D->AddActor(m_fixFemoral);
  m_FemoralRadius = 50;

  // for knee
  m_moveKnee = vtkSmartPointer<vtkActor>::New();
  m_moveKnee->GetProperty()->SetColor(1, 0, 0);
  m_moveKnee->GetProperty()->SetLineWidth(5);
  m_fixKnee = vtkSmartPointer<vtkActor>::New();
  m_fixKnee->GetProperty()->SetColor(1, 0, 0);
  m_fixKnee->GetProperty()->SetLineWidth(5);

  m_Render2D->AddActor(m_moveKnee);
  m_Render2D->AddActor(m_fixKnee);
  m_KneeLength = 100;
  m_KneeisFlip = false;

  // for ankle
  m_moveAnkle = vtkSmartPointer<vtkActor>::New();
  m_moveAnkle->GetProperty()->SetColor(1, 0, 0);
  m_moveAnkle->GetProperty()->SetLineWidth(5);

  m_fixAnkle = vtkSmartPointer<vtkActor>::New();
  m_fixAnkle->GetProperty()->SetColor(1, 0, 0);
  m_fixAnkle->GetProperty()->SetLineWidth(5);

  m_Render2D->AddActor(m_moveAnkle);
  m_Render2D->AddActor(m_fixAnkle);
  m_AnkleLength = 100;

  // for force line
  m_ForceLineActor = vtkSmartPointer<vtkActor>::New();
  m_ForceLineActor->GetProperty()->SetColor(1, 1, 0);
  m_ForceLineActor->GetProperty()->SetLineWidth(5);
  m_Render2D->AddActor(m_ForceLineActor);

  m_ExtensionLineActor = vtkSmartPointer<vtkActor>::New();
  m_ExtensionLineActor->GetProperty()->SetColor(1, 1, 0);
  m_ExtensionLineActor->GetProperty()->SetLineWidth(5);
  m_Render2D->AddActor(m_ExtensionLineActor);

  m_AngleFollower = vtkSmartPointer<vtkFollower>::New();
  m_AngleFollower->SetCamera(m_Render2D->GetActiveCamera());
  m_AngleFollower->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_AngleFollower);

  for (int i = 0; i < 3; i++) {
    m_FemoralCenter[i] = 0;
    m_KneeCenter[i] = 0;
    m_AnkleCenter[i] = 0;
    m_XRayRegionFixCenter[i] = 0;
  }

  // for registration
  m_moveXrayActor = vtkSmartPointer<vtkActor>::New();
  m_moveXrayActor->GetProperty()->SetColor(1, 0, 0);
  m_moveXrayActor->GetProperty()->SetLineWidth(5);
  m_Render2D->AddActor(m_moveXrayActor);

  m_fixXrayActor = vtkSmartPointer<vtkActor>::New();
  m_fixXrayActor->GetProperty()->SetColor(0, 0, 1);
  m_fixXrayActor->GetProperty()->SetLineWidth(5);
  m_fixXrayActor->GetProperty()->SetOpacity(0.75);
  m_Render2D->AddActor(m_fixXrayActor);

  m_XRayLength = 400;

  m_moveCTActor = vtkSmartPointer<vtkActor>::New();
  m_moveCTActor->GetProperty()->SetColor(1, 0, 0);
  m_moveCTActor->GetProperty()->SetLineWidth(5);
  m_Render3D->AddActor(m_moveCTActor);

  m_fixCTActor = vtkSmartPointer<vtkActor>::New();
  m_fixCTActor->GetProperty()->SetColor(1, 0, 0);
  m_fixCTActor->GetProperty()->SetLineWidth(5);
  m_Render3D->AddActor(m_fixCTActor);

  m_xrayIn3D = vtkSmartPointer<vtkImageActor>::New();
  m_CTRegionVolume = vtkSmartPointer<vtkVolume>::New();

  m_ExtractedCTImage = ImageType::New();
  m_ExtractedXRayImage = ImageType::New();

  m_CTLength = 100;

  this->SetUpSphereWidet();
  this->SetUpOrientationWidget();
  connect(ui->DICOMBrowserButton, SIGNAL(clicked(bool)), this,
          SLOT(OnDICOMBrowser()));
  connect(m_dicomBrowser, SIGNAL(importVolume(int)), this,
          SLOT(OnImportVolume(int)));
  connect(m_dicomBrowser, SIGNAL(importXRay(int)), this,
          SLOT(OnImportXRay(int)));
  connect(ui->SetSurceWidgetButton, SIGNAL(clicked(bool)), this,
          SLOT(OnSetSurceWidget()));
  connect(ui->GenerateDRRButton, SIGNAL(clicked(bool)), this,
          SLOT(OnGenerateDRR()));
  //  m_vtkqtConnector->Connect(m_surceSphereWidget,
  //                            vtkCommand::EndInteractionEvent, this,
  //                            SLOT(OnSurceWidgetInteraction()));
  connect(ui->XOutOffSetSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnSurceWidgetInteraction()));
  connect(ui->YOutOffSetSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnSurceWidgetInteraction()));
  connect(ui->ZOutOffSetSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(OnSurceWidgetInteraction()));
  connect(ui->FemoralCenterButton, SIGNAL(clicked(bool)), this,
          SLOT(OnFemoralCenterButton()));
  connect(ui->KneeCenterButton, SIGNAL(clicked(bool)), this,
          SLOT(OnKneeCenterButton()));

  connect(ui->AnkleCenterButton, SIGNAL(clicked(bool)), this,
          SLOT(OnAnkleCenterButton()));

  connect(ui->XRayRegionButton, SIGNAL(clicked(bool)), this,
          SLOT(OnXRayRegionButton()));
  connect(ui->CTRegionButton, SIGNAL(clicked(bool)), this,
          SLOT(OnCTRegionButton()));

  connect(ui->XRayIn3DVisibilityButton, SIGNAL(clicked(bool)), this,
          SLOT(OnXrayIn3DVisibilityButton()));
  connect(ui->StartRegistrationButton, SIGNAL(clicked(bool)), this,
          SLOT(OnStartRegestration()));

  connect(ui->CTVolumeVisibilityButton, SIGNAL(clicked(bool)), this,
          SLOT(OnCTVolumeVisibility()));
  connect(ui->CTRegionVolumeVisibilityButton, SIGNAL(clicked(bool)), this,
          SLOT(OnCTRegionVisibilityButton()));
  connect(ui->RemoveCTNoiseButton, SIGNAL(clicked(bool)), this,
          SLOT(OnRemoveCTNoiseButton()));

  connect(ui->testButton, SIGNAL(clicked(bool)), this, SLOT(OnTestButton()));

  this->InitializePlanningWidget();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
  int choose = QMessageBox::information(
      this, tr("Quit"), tr("Unsaved change will be lost! Continue to quit?"),
      QMessageBox::Ok | QMessageBox::Cancel);
  if (choose == QMessageBox::Ok) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::SetUpSphereWidet() {
  // for femoral
  m_FemoralSide1 = vtkSmartPointer<vtkSphereWidget>::New();
  m_FemoralSide1->SetInteractor(m_Interactor2D);
  m_FemoralSide1->SetRepresentationToSurface();
  m_FemoralSide1->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_FemoralSide1, vtkCommand::InteractionEvent, this,
                            SLOT(OnFemoralSideChange()));
  m_FemoralSide2 = vtkSmartPointer<vtkSphereWidget>::New();
  m_FemoralSide2->SetInteractor(m_Interactor2D);
  m_FemoralSide2->SetRepresentationToSurface();
  m_FemoralSide2->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_FemoralSide2, vtkCommand::InteractionEvent, this,
                            SLOT(OnFemoralSideChange()));
  m_FemoralSide3 = vtkSmartPointer<vtkSphereWidget>::New();
  m_FemoralSide3->SetInteractor(m_Interactor2D);
  m_FemoralSide3->SetRepresentationToSurface();
  m_FemoralSide3->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_FemoralSide3, vtkCommand::InteractionEvent, this,
                            SLOT(OnFemoralSideChange()));

  m_FemoralOrigion = vtkSmartPointer<vtkSphereWidget>::New();
  m_FemoralOrigion->SetInteractor(m_Interactor2D);
  m_FemoralOrigion->SetRepresentationToSurface();
  m_FemoralOrigion->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_FemoralOrigion, vtkCommand::InteractionEvent,
                            this, SLOT(OnFemoralOrigionChange()));
  // for knee
  m_KneeP1 = vtkSmartPointer<vtkSphereWidget>::New();
  m_KneeP1->SetInteractor(m_Interactor2D);
  m_KneeP1->SetRepresentationToSurface();
  m_KneeP1->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_KneeP1, vtkCommand::InteractionEvent, this,
                            SLOT(OnKneeSideChange()));
  m_KneeP2 = vtkSmartPointer<vtkSphereWidget>::New();
  m_KneeP2->SetInteractor(m_Interactor2D);
  m_KneeP2->SetRepresentationToSurface();
  m_KneeP2->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_KneeP2, vtkCommand::InteractionEvent, this,
                            SLOT(OnKneeSideChange()));
  m_KneeOrigion = vtkSmartPointer<vtkSphereWidget>::New();
  m_KneeOrigion->SetInteractor(m_Interactor2D);
  m_KneeOrigion->SetRepresentationToSurface();
  m_KneeOrigion->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_KneeOrigion, vtkCommand::InteractionEvent, this,
                            SLOT(OnKneeOrigionChange()));

  // for  ankle
  m_AnkleP1 = vtkSmartPointer<vtkSphereWidget>::New();
  m_AnkleP1->SetInteractor(m_Interactor2D);
  m_AnkleP1->SetRepresentationToSurface();
  m_AnkleP1->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_AnkleP1, vtkCommand::InteractionEvent, this,
                            SLOT(OnAnkleSideChanged()));
  m_AnkleP2 = vtkSmartPointer<vtkSphereWidget>::New();
  m_AnkleP2->SetInteractor(m_Interactor2D);
  m_AnkleP2->SetRepresentationToSurface();
  m_AnkleP2->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_AnkleP2, vtkCommand::InteractionEvent, this,
                            SLOT(OnAnkleSideChanged()));
  m_AnkleOrigion = vtkSmartPointer<vtkSphereWidget>::New();
  m_AnkleOrigion->SetInteractor(m_Interactor2D);
  m_AnkleOrigion->SetRepresentationToSurface();
  m_AnkleOrigion->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_AnkleOrigion, vtkCommand::InteractionEvent, this,
                            SLOT(OnAnkleOrigionChange()));

  // for xray region
  m_XrayRegionWidgetP1 = vtkSmartPointer<vtkSphereWidget>::New();
  m_XrayRegionWidgetP1->SetInteractor(m_Interactor2D);
  m_XrayRegionWidgetP1->SetRepresentationToSurface();
  m_XrayRegionWidgetP1->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_XrayRegionWidgetP1, vtkCommand::InteractionEvent,
                            this, SLOT(OnXRayRegionSideChanged()));
  m_XrayRegionWidgetP2 = vtkSmartPointer<vtkSphereWidget>::New();
  m_XrayRegionWidgetP2->SetInteractor(m_Interactor2D);
  m_XrayRegionWidgetP2->SetRepresentationToSurface();
  m_XrayRegionWidgetP2->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_XrayRegionWidgetP2, vtkCommand::InteractionEvent,
                            this, SLOT(OnXRayRegionSideChanged()));
  m_XrayRegionWidgetP3 = vtkSmartPointer<vtkSphereWidget>::New();
  m_XrayRegionWidgetP3->SetInteractor(m_Interactor2D);
  m_XrayRegionWidgetP3->SetRepresentationToSurface();
  m_XrayRegionWidgetP3->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_XrayRegionWidgetP3, vtkCommand::InteractionEvent,
                            this, SLOT(OnXRayRegionSideChanged()));
  m_XrayRegionWidgetP4 = vtkSmartPointer<vtkSphereWidget>::New();
  m_XrayRegionWidgetP4->SetInteractor(m_Interactor2D);
  m_XrayRegionWidgetP4->SetRepresentationToSurface();
  m_XrayRegionWidgetP4->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_XrayRegionWidgetP4, vtkCommand::InteractionEvent,
                            this, SLOT(OnXRayRegionSideChanged()));
  m_XRayRegionWidgetOrigon = vtkSmartPointer<vtkSphereWidget>::New();
  m_XRayRegionWidgetOrigon->SetInteractor(m_Interactor2D);
  m_XRayRegionWidgetOrigon->SetRepresentationToSurface();
  m_XRayRegionWidgetOrigon->GetSphereProperty()->SetColor(1, 1, 0);
  m_vtkqtConnector->Connect(m_XRayRegionWidgetOrigon,
                            vtkCommand::InteractionEvent, this,
                            SLOT(OnXRayRegionSideChanged()));

  // for ct region
  m_CTRegionWidgetP1 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP1);
  m_CTRegionWidgetP2 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP2);
  m_CTRegionWidgetP3 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP3);
  m_CTRegionWidgetP4 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP4);
  m_CTRegionWidgetP5 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP5);
  m_CTRegionWidgetP6 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP6);
  m_CTRegionWidgetP7 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP7);
  m_CTRegionWidgetP8 = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetP8);
  m_CTRegionWidgetOrigion = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTSphereList.append(m_CTRegionWidgetOrigion);
  foreach (vtkSmartPointer<vtkSphereWidget> var, m_CTSphereList) {
    var->SetInteractor(m_Interactor3D);
    var->SetRepresentationToSurface();
    var->GetSphereProperty()->SetColor(1, 1, 0);
    m_vtkqtConnector->Connect(var, vtkCommand::InteractionEvent, this,
                              SLOT(OnCTRegionChanged()));
  }

  // for ct remove noise
  m_CTRemoveWidget = vtkSmartPointer<vtkSphereWidget>::New();
  m_CTRemoveWidget->SetInteractor(m_Interactor3D);
  m_CTRemoveWidget->SetRepresentationToSurface();
  m_CTRemoveWidget->GetSphereProperty()->SetColor(0, 1, 0);
  m_CTRemoveWidget->SetRadius(10);
}

void MainWindow::SetUpOrientationWidget() {
  auto cube = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
  cube->SetFaceTextScale(0.65);
  vtkProperty *prop = cube->GetCubeProperty();
  prop->SetColor(0.75, 0.75, 1);

  prop = cube->GetTextEdgesProperty();
  prop->SetLineWidth(1);
  prop->SetDiffuse(0);
  prop->SetAmbient(1);
  prop->SetColor(0, 0, 0);

  // this static function improves the appearance of the text edges
  // since they are overlaid on a surface rendering of the cube's faces
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();

  // anatomic labelling
  cube->SetXPlusFaceText("L");
  cube->SetXMinusFaceText("R");
  cube->SetYPlusFaceText("P");
  cube->SetYMinusFaceText("A");
  cube->SetZPlusFaceText("S");
  cube->SetZMinusFaceText("I");

  // change the vector text colors
  prop = cube->GetXPlusFaceProperty();
  prop->SetColor(1, 0, 0);
  prop->SetInterpolationToFlat();
  prop = cube->GetXMinusFaceProperty();
  prop->SetColor(1, 0, 0);
  prop->SetInterpolationToFlat();
  prop = cube->GetYPlusFaceProperty();
  prop->SetColor(0, 1, 0);
  prop->SetInterpolationToFlat();
  prop = cube->GetYMinusFaceProperty();
  prop->SetColor(0, 1, 0);
  prop->SetInterpolationToFlat();
  prop = cube->GetZPlusFaceProperty();
  prop->SetColor(0, 0, 1);
  prop->SetInterpolationToFlat();
  prop = cube->GetZMinusFaceProperty();
  prop->SetColor(0, 0, 1);
  prop->SetInterpolationToFlat();

  auto axes = vtkSmartPointer<vtkAxesActor>::New();

  axes->SetShaftTypeToCylinder();
  axes->SetXAxisLabelText("X");
  axes->SetYAxisLabelText("Y");
  axes->SetZAxisLabelText("Z");

  axes->SetTotalLength(1.5, 1.5, 1.5);
  axes->SetCylinderRadius(0.500 * axes->GetCylinderRadius());
  axes->SetConeRadius(1.025 * axes->GetConeRadius());
  axes->SetSphereRadius(1.500 * axes->GetSphereRadius());

  vtkTextProperty *tprop =
      axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty();
  tprop->BoldOn();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToArial();
  tprop->SetColor(1, 0, 0);

  tprop = axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty();
  tprop->BoldOn();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToArial();
  tprop->SetColor(0, 1, 0);

  tprop = axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty();
  tprop->BoldOn();
  tprop->ItalicOn();
  tprop->ShadowOn();
  tprop->SetFontFamilyToArial();
  tprop->SetColor(0, 0, 1);

  auto assembly = vtkSmartPointer<vtkPropAssembly>::New();
  assembly->AddPart(axes);
  assembly->AddPart(cube);

  m_orientation2D->SetOrientationMarker(assembly);
  m_orientation2D->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_orientation2D->SetInteractor(m_Interactor2D);
  m_orientation2D->EnabledOn();
  m_orientation2D->InteractiveOff();
  m_orientation2D->On();
  this->SetUpCamera(1, 2);
}

void MainWindow::SetUpCamera(int i, int j) {
  if (i > 2)
    return;
  if (j > 3)
    return;
  double viewRight[4][3] = {{0, -1, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}};
  double viewUp[4][3] = {{0, 0, 1}, {0, 0, 1}, {0, -1, 0}, {0, 0, 1}};
  vtkCamera *camera;
  switch (i) {
  case 1:
    camera = m_Render2D->GetActiveCamera();
    camera->ParallelProjectionOn();
    break;
  case 2:
    camera = m_Render3D->GetActiveCamera();
    camera->ParallelProjectionOn();
    break;
  default:
    break;
  }
  double focus[3];
  camera->GetFocalPoint(focus);
  double d = camera->GetDistance();

  double cross[3];
  vtkMath::Cross(viewRight[j], viewUp[j], cross);

  camera->SetPosition(focus[0] + d * cross[0], focus[1] + d * cross[1],
                      focus[2] + d * cross[2]);
  camera->SetFocalPoint(focus);
  camera->SetViewUp(viewUp[j]);
}

void MainWindow::UpDateFemoralCircle() {
  //计算圆
  double zdirection[3] = {0, 0, 1};
  double crossDirection[3];
  auto circlepoints = vtkSmartPointer<vtkPoints>::New();
  circlepoints->Initialize();
  for (int i = 0; i < 360; i++) {
    vtkMath::Perpendiculars(zdirection, nullptr, crossDirection,
                            i / 180.0 * vtkMath::Pi());

    double p[3];
    for (int j = 0; j < 3; j++) {
      p[j] = m_FemoralCenter[j] + crossDirection[j] * m_FemoralRadius;
    }
    circlepoints->InsertNextPoint(p);
  }
  auto lines = vtkSmartPointer<vtkCellArray>::New();
  lines->Initialize();
  for (int i = 0; i < 360; i++) {
    auto idlist = vtkSmartPointer<vtkIdList>::New();
    idlist->Initialize();
    if (i == 359) {
      idlist->InsertNextId(359);
      idlist->InsertNextId(0);
    } else {
      idlist->InsertNextId(i);
      idlist->InsertNextId(i + 1);
    }
    lines->InsertNextCell(idlist);
  }
  auto circlePD = vtkSmartPointer<vtkPolyData>::New();
  circlePD->SetPoints(circlepoints);
  circlePD->SetLines(lines);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(circlePD);
  m_fixFemoral->SetMapper(mapper);
  m_fixFemoral->VisibilityOn();
  m_Render2D->GetRenderWindow()->Render();

  ui->FemoralCenterButton->setChecked(false);
  this->OnFemoralCenterButton();

  this->UpDateForceLine();
}

void MainWindow::GenerateDRR() {
  qDebug() << " 0";
  const unsigned int Dimension = 3;
  typedef short InputPixelType;
  typedef short OutputPixleType;
  typedef itk::Image<InputPixelType, Dimension> InputImageType;

  typedef itk::VTKImageToImageFilter<InputImageType> VTKImageToImageType;
  VTKImageToImageType::Pointer m_fromvtk = VTKImageToImageType::New();
  m_fromvtk->SetInput(m_imageVolume);
  m_fromvtk->Update();

  typedef unsigned char OutputPixelType;

  typedef itk::Image<InputPixelType, Dimension> InputImageType;
  typedef itk::Image<OutputPixelType, Dimension> OutputImageType;

  InputImageType::Pointer image;
  image = m_fromvtk->GetOutput();

  typedef itk::ResampleImageFilter<InputImageType, InputImageType> FilterType;

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(image);
  filter->SetDefaultPixelValue(-1024);

  typedef itk::CenteredEuler3DTransform<double> TransformType;

  TransformType::Pointer transform = TransformType::New();

  transform->SetComputeZYX(true);

  TransformType::OutputVectorType translation;

  translation[0] = 0;
  translation[1] = 0;
  translation[2] = 0;

  // constant for converting degrees into radians
  const double dtr = (std::atan(1.0) * 4.0) / 180.0;

  transform->SetTranslation(translation);
  transform->SetRotation(dtr * ui->XOutOffSetSpinBox->value(),
                         dtr * ui->YOutOffSetSpinBox->value(),
                         dtr * ui->ZOutOffSetSpinBox->value());

  InputImageType::PointType imOrigin = image->GetOrigin();
  InputImageType::SpacingType imRes = image->GetSpacing();

  typedef InputImageType::RegionType InputImageRegionType;
  typedef InputImageRegionType::SizeType InputImageSizeType;

  InputImageRegionType imRegion = image->GetBufferedRegion();
  InputImageSizeType imSize = imRegion.GetSize();

  imOrigin[0] += imRes[0] * static_cast<double>(imSize[0]) / 2.0;
  imOrigin[1] += imRes[1] * static_cast<double>(imSize[1]) / 2.0;
  imOrigin[2] += imRes[2] * static_cast<double>(imSize[2]) / 2.0;

  TransformType::InputPointType center;
  center[0] = 0 + imOrigin[0];
  center[1] = 0 + imOrigin[1];
  center[2] = 0 + imOrigin[2];

  transform->SetCenter(center);

  typedef itk::RayCastInterpolateImageFunction<InputImageType, double>
      InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  interpolator->SetTransform(transform);

  //  interpolator->SetThreshold(-1024);

  InterpolatorType::InputPointType focalpoint;

  focalpoint[0] = imOrigin[0];
  focalpoint[1] = imOrigin[1];
  focalpoint[2] = imOrigin[2] - m_surceSphereWidget->GetCenter()[2];
  interpolator->SetFocalPoint(focalpoint);

  filter->SetInterpolator(interpolator);
  filter->SetTransform(transform);
  filter->SetOutputDirection(m_fromvtk->GetOutput()->GetDirection());
  InputImageType::SizeType size;

  size[0] =
      ui->SizeXSpinBox->value(); // number of pixels along X of the 2D DRR image
  size[1] =
      ui->SizeYSpinBox->value(); // number of pixels along Y of the 2D DRR image
  size[2] = 1;                   // only one slice

  filter->SetSize(size);

  InputImageType::SpacingType spacing;

  spacing[0] = 1;   // pixel spacing along X of the 2D DRR image [mm]
  spacing[1] = 1;   // pixel spacing along Y of the 2D DRR image [mm]
  spacing[2] = 1.0; // slice thickness of the 2D DRR image [mm]

  filter->SetOutputSpacing(spacing);

  double origin[Dimension];

  origin[0] = imOrigin[0] -
              (m_volumeExtend[1] - m_volumeExtend[0]) / 2 * m_volumeSpacing[0] -
              300;
  origin[1] = imOrigin[1] -
              (m_volumeExtend[3] - m_volumeExtend[2]) / 2 * m_volumeSpacing[1] +
              200;
  origin[2] = imOrigin[2] + m_surceSphereWidget->GetCenter()[2] - 100;

  filter->SetOutputOrigin(origin);
  filter->Update();

  auto tovtk = itk::ImageToVTKImageFilter<InputImageType>::New();
  tovtk->SetInput(filter->GetOutput());
  tovtk->Update();
  m_imageXRay->DeepCopy(tovtk->GetOutput());
  double scaleRange[2];
  m_imageXRay->GetScalarRange(scaleRange);
  qDebug() << "origin " << m_imageXRay->GetOrigin()[0] << "  "
           << m_imageXRay->GetOrigin()[1] << "  "
           << m_imageXRay->GetOrigin()[22];
  m_drr3Dactor->GetMapper()->SetInputData(m_imageXRay);
  m_drr3Dactor->SetZSlice(0);
  m_drr3Dactor->GetProperty()->SetColorLevel(
      (scaleRange[1] - scaleRange[0]) / 2 + scaleRange[0]);
  m_drr3Dactor->GetProperty()->SetColorWindow(scaleRange[1] - scaleRange[0]);
  auto vtk0 = itk::ImageToVTKImageFilter<InputImageType>::New();
  vtk0->SetInput(m_fromvtk->GetOutput());
  vtk0->Update();

  auto im = vtkSmartPointer<vtkImageData>::New();
  im->DeepCopy(vtk0->GetOutput());
  qDebug() << im->GetDimensions()[0];
  auto stransform = vtkSmartPointer<vtkTransform>::New();
  stransform->RotateZ(ui->ZOutOffSetSpinBox->value());
  stransform->RotateY(ui->YOutOffSetSpinBox->value());
  stransform->RotateX(ui->XOutOffSetSpinBox->value());

  stransform->Update();
  m_drr2Dactor->GetMapper()->SetInputData(m_imageXRay);
  m_drr2Dactor->SetZSlice(100);
  m_drr2Dactor->GetProperty()->SetColorLevel(
      (scaleRange[1] - scaleRange[0]) / 2 + scaleRange[0]);
  m_drr2Dactor->GetProperty()->SetColorWindow(scaleRange[1] - scaleRange[0]);
  m_drr2Dactor->SetUserTransform(stransform);
  if (!m_Render2D->GetActors()->IsItemPresent(m_drr3Dactor)) {
    m_Render2D->AddActor(m_drr3Dactor);
    m_Render3D->AddActor(m_drr2Dactor);
  }
  m_GenericRendWin2D->Render();
  m_GenericRendWin3D->Render();
  qDebug() << " 4";
}

void MainWindow::CalculateCircle(double p1[], double p2[], double p3[],
                                 double outcenter[], double &outradius) {
  double a1, b1, c1, d1;
  double a2, b2, c2, d2;
  double a3, b3, c3, d3;

  double x1 = p1[0], y1 = p1[1], z1 = p1[2];
  double x2 = p2[0], y2 = p2[1], z2 = p2[2];
  double x3 = p3[0], y3 = p3[1], z3 = p3[2];

  a1 = (y1 * z2 - y2 * z1 - y1 * z3 + y3 * z1 + y2 * z3 - y3 * z2);
  b1 = -(x1 * z2 - x2 * z1 - x1 * z3 + x3 * z1 + x2 * z3 - x3 * z2);
  c1 = (x1 * y2 - x2 * y1 - x1 * y3 + x3 * y1 + x2 * y3 - x3 * y2);
  d1 = -(x1 * y2 * z3 - x1 * y3 * z2 - x2 * y1 * z3 + x2 * y3 * z1 +
         x3 * y1 * z2 - x3 * y2 * z1);

  a2 = 2 * (x2 - x1);
  b2 = 2 * (y2 - y1);
  c2 = 2 * (z2 - z1);
  d2 = x1 * x1 + y1 * y1 + z1 * z1 - x2 * x2 - y2 * y2 - z2 * z2;

  a3 = 2 * (x3 - x1);
  b3 = 2 * (y3 - y1);
  c3 = 2 * (z3 - z1);
  d3 = x1 * x1 + y1 * y1 + z1 * z1 - x3 * x3 - y3 * y3 - z3 * z3;

  outcenter[0] = -(b1 * c2 * d3 - b1 * c3 * d2 - b2 * c1 * d3 + b2 * c3 * d1 +
                   b3 * c1 * d2 - b3 * c2 * d1) /
                 (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 +
                  a3 * b1 * c2 - a3 * b2 * c1);
  outcenter[1] = (a1 * c2 * d3 - a1 * c3 * d2 - a2 * c1 * d3 + a2 * c3 * d1 +
                  a3 * c1 * d2 - a3 * c2 * d1) /
                 (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 +
                  a3 * b1 * c2 - a3 * b2 * c1);
  outcenter[2] = -(a1 * b2 * d3 - a1 * b3 * d2 - a2 * b1 * d3 + a2 * b3 * d1 +
                   a3 * b1 * d2 - a3 * b2 * d1) /
                 (a1 * b2 * c3 - a1 * b3 * c2 - a2 * b1 * c3 + a2 * b3 * c1 +
                  a3 * b1 * c2 - a3 * b2 * c1);
  double distance[3];
  for (int i = 0; i < 3; i++) {
    distance[i] = outcenter[i] - p1[i];
  }
  outradius =
      sqrt(pow(distance[0], 2) + pow(distance[1], 2) + pow(distance[2], 2));
}

void MainWindow::UpDateForceLine() {

  bool isfemoralDefined = isDataNull(m_FemoralCenter);
  bool iskneeDefined = isDataNull(m_KneeCenter);
  bool isankleDefined = isDataNull(m_AnkleCenter);
  qDebug() << isfemoralDefined << " " << iskneeDefined << "  "
           << isankleDefined;

  auto Forcelinepd = vtkSmartPointer<vtkPolyData>::New();
  auto Extensionlinepd = vtkSmartPointer<vtkPolyData>::New();
  if (ui->FemoralCenterButton->isChecked()) {
    if (iskneeDefined) {
      if (isankleDefined) {
        BuildLine(m_FemoralMoveCenter, m_KneeCenter, m_AnkleCenter,
                  Forcelinepd);
        BuildExtensionLine(m_FemoralMoveCenter, m_KneeCenter, m_AnkleCenter,
                           Extensionlinepd);
      } else {
        BuildLine(m_FemoralMoveCenter, m_KneeCenter, Forcelinepd);
      }
    }
  } else if (ui->KneeCenterButton->isChecked()) {
    if (isfemoralDefined) {
      if (isankleDefined) {
        BuildLine(m_FemoralCenter, m_KneeMoveCenter, m_AnkleCenter,
                  Forcelinepd);
        BuildExtensionLine(m_FemoralCenter, m_KneeMoveCenter, m_AnkleCenter,
                           Extensionlinepd);
      } else {
        BuildLine(m_FemoralCenter, m_KneeMoveCenter, Forcelinepd);
      }
    }
  } else if (ui->AnkleCenterButton->isChecked()) {
    if (iskneeDefined) {
      if (isfemoralDefined) {
        BuildLine(m_FemoralCenter, m_KneeCenter, m_AnkleMoveCenter,
                  Forcelinepd);
        BuildExtensionLine(m_FemoralCenter, m_KneeCenter, m_AnkleMoveCenter,
                           Extensionlinepd);
      } else {
        BuildLine(m_KneeCenter, m_AnkleMoveCenter, Forcelinepd);
      }
    }
  } else {
    if (isfemoralDefined && iskneeDefined && isankleDefined) {
      BuildLine(m_FemoralCenter, m_KneeCenter, m_AnkleCenter, Forcelinepd);
      BuildExtensionLine(m_FemoralCenter, m_KneeCenter, m_AnkleCenter,
                         Extensionlinepd);
    } else if (isfemoralDefined && iskneeDefined) {
      BuildLine(m_FemoralCenter, m_KneeCenter, Forcelinepd);
    } else if (iskneeDefined && isankleDefined) {
      BuildLine(m_KneeCenter, m_AnkleCenter, Forcelinepd);
    } else
      return;
  }

  auto Forcelinemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  Forcelinemapper->SetInputData(Forcelinepd);
  m_ForceLineActor->SetMapper(Forcelinemapper);

  auto Extensionmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  Extensionmapper->SetInputData(Extensionlinepd);
  m_ExtensionLineActor->SetMapper(Extensionmapper);
  m_Render2D->GetRenderWindow()->Render();
}

bool MainWindow::isDataNull(double data[]) {
  if (data[0] == 0 && data[1] == 0 && data[2] == 0)
    return 0;
  else
    return 1;
}

bool MainWindow::isEqual(double p1[], double p2[]) {
  for (int i = 0; i < 3; i++) {
    if (p1[i] != p2[i])
      return true;
  }
  return false;
}

void MainWindow::BuildLine(double p1[], double p2[], vtkPolyData *out) {
  auto points = vtkSmartPointer<vtkPoints>::New();
  points->Initialize();
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();

  points->InsertNextPoint(p1);
  points->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  cells->InsertNextCell(idlist);

  out->SetPoints(points);
  out->SetLines(cells);
}

void MainWindow::BuildLine(double p1[], double p2[], double p3[],
                           vtkPolyData *out) {
  auto points = vtkSmartPointer<vtkPoints>::New();
  points->Initialize();
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();

  points->InsertNextPoint(p1);
  points->InsertNextPoint(p2);
  points->InsertNextPoint(p3);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  idlist->InsertNextId(2);
  cells->InsertNextCell(idlist);

  out->SetPoints(points);
  out->SetLines(cells);
}

void MainWindow::BuildExtensionLine(double p1[], double p2[], double p3[],
                                    vtkPolyData *out) {
  double distance = sqrt(vtkMath::Distance2BetweenPoints(p2, p3));
  double extensionvec[3];
  for (int i = 0; i < 3; i++)
    extensionvec[i] = p2[i] - p1[i];
  vtkMath::Normalize(extensionvec);
  double extendpt[3];
  for (int i = 0; i < 3; i++)
    extendpt[i] = p2[i] + distance * extensionvec[i];

  auto points = vtkSmartPointer<vtkPoints>::New();
  points->Initialize();
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  int segment = 20;
  int numOfSegment = distance / segment;
  for (int i = 0; i < numOfSegment; i++) {
    double pt[3];
    for (int j = 0; j < 3; j++) {
      pt[j] = p2[j] + segment * i * extensionvec[j];
    }
    points->InsertNextPoint(pt);
    if (i % 2 == 0 && i < numOfSegment - 1) {
      auto idlist = vtkSmartPointer<vtkIdList>::New();
      idlist->Initialize();
      idlist->InsertNextId(i);
      idlist->InsertNextId(i + 1);
      cells->InsertNextCell(idlist);
    }
  }
  out->SetPoints(points);
  out->SetLines(cells);

  // for angle
  double planeOrigion[3];
  m_imageXRay->GetOrigin(planeOrigion);
  planeOrigion[2] += 10;
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetNormal(0, 0, 1);
  plane->SetOrigin(planeOrigion);

  double knee2angleVec[3];
  for (int i = 0; i < 3; i++)
    knee2angleVec[i] = p3[i] - p2[i];
  vtkMath::Normalize(knee2angleVec);

  plane->ProjectVector(extensionvec, extensionvec);
  plane->ProjectVector(knee2angleVec, knee2angleVec);
  double angle = vtkMath::DegreesFromRadians(
      vtkMath::AngleBetweenVectors(knee2angleVec, extensionvec));
  QString angleString = QString::number(angle);
  char *ch;
  QByteArray ba = angleString.toLocal8Bit();
  ch = ba.data();
  double anglePos[3];
  double midvec[3];
  for (int i = 0; i < 3; i++) {
    midvec[i] = knee2angleVec[i] + extensionvec[i];
  }
  vtkMath::Normalize(midvec);

  auto transform = vtkSmartPointer<vtkTransform>::New();
  transform->RotateWXYZ(-angle, 0, 0, 1);
  transform->Update();
  double transvec[3];
  transvec[0] = transform->TransformDoubleVector(midvec)[0];
  transvec[1] = transform->TransformDoubleVector(midvec)[1];
  transvec[2] = transform->TransformDoubleVector(midvec)[2];

  for (int i = 0; i < 3; i++) {
    anglePos[i] = transvec[i] * distance / 2 + p2[i];
  }
  auto vectext = vtkSmartPointer<vtkVectorText>::New();
  vectext->SetText(ch);
  vectext->Update();
  auto textmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  textmapper->SetInputData(vectext->GetOutput());
  m_AngleFollower->SetMapper(textmapper);
  m_AngleFollower->SetPosition(anglePos);
  m_AngleFollower->SetScale(50);
}

void MainWindow::BuildFrame(double p1[], double p2[], double p3[], double p4[],
                            vtkPolyData *out) {
  double sp1[3], sp2[3], sp3[3], sp4[3];
  for (int i = 0; i < 3; i++) {
    sp1[i] = p1[i];
    sp2[i] = p2[i];
    sp3[i] = p3[i];
    sp4[i] = p4[i];
  }
  double segment = 30;
  auto line12 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(sp1, sp2, segment, line12);

  auto line23 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(sp2, sp3, segment, line23);

  auto line34 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(sp3, sp4, segment, line34);

  auto line41 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(sp4, sp1, segment, line41);

  auto appender = vtkSmartPointer<vtkAppendPolyData>::New();
  appender->AddInputData(line12);
  appender->AddInputData(line23);
  appender->AddInputData(line34);
  appender->AddInputData(line41);
  appender->Update();

  out->DeepCopy(appender->GetOutput());
}

void MainWindow::BuildDottedLine(double p1[], double p2[], double segment,
                                 vtkPolyData *out) {
  double sp1[3], sp2[3];
  double p2top1[3];
  for (int i = 0; i < 3; i++) {
    sp1[i] = p1[i];
    sp2[i] = p2[i];
    p2top1[i] = sp2[i] - sp1[i];
  }
  vtkMath::Normalize(p2top1);

  double p1distancep2 = sqrt(vtkMath::Distance2BetweenPoints(sp1, sp2));

  int numOfSegment1 = p1distancep2 / segment;
  auto points12 = vtkSmartPointer<vtkPoints>::New();
  points12->Initialize();
  auto cells12 = vtkSmartPointer<vtkCellArray>::New();
  cells12->Initialize();
  for (int i = 0; i < numOfSegment1; i++) {
    double pt[3];
    for (int j = 0; j < 3; j++) {
      pt[j] = sp1[j] + p2top1[j] * segment * i;
    }
    points12->InsertNextPoint(pt);
    if (i % 2 == 0 && i < numOfSegment1 - 1) {
      auto idlist = vtkSmartPointer<vtkIdList>::New();
      idlist->Initialize();
      idlist->InsertNextId(i);
      idlist->InsertNextId(i + 1);
      cells12->InsertNextCell(idlist);
    }
  }
  out->SetPoints(points12);
  out->SetLines(cells12);
}

void MainWindow::BuildBox(double p1[], double p2[], double p3[], double p4[],
                          double p5[], double p6[], double p7[], double p8[],
                          vtkPolyData *out) {

  double segment = 20;

  auto line12 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p1, p2, segment, line12);

  auto line23 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p2, p3, segment, line23);

  auto line34 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p3, p4, segment, line34);

  auto line41 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p4, p1, segment, line41);

  auto line56 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p5, p6, segment, line56);

  auto line67 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p6, p7, segment, line67);

  auto line78 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p7, p8, segment, line78);

  auto line85 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p8, p5, segment, line85);

  auto line15 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p1, p5, segment, line15);

  auto line26 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p2, p6, segment, line26);

  auto line37 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p3, p7, segment, line37);

  auto line48 = vtkSmartPointer<vtkPolyData>::New();
  this->BuildDottedLine(p4, p8, segment, line48);

  auto appender = vtkSmartPointer<vtkAppendPolyData>::New();
  appender->AddInputData(line12);
  appender->AddInputData(line23);
  appender->AddInputData(line34);
  appender->AddInputData(line41);
  appender->AddInputData(line56);
  appender->AddInputData(line67);
  appender->AddInputData(line78);
  appender->AddInputData(line85);
  appender->AddInputData(line15);
  appender->AddInputData(line26);
  appender->AddInputData(line37);
  appender->AddInputData(line48);
  appender->Update();

  out->DeepCopy(appender->GetOutput());
}

void MainWindow::ExtractXRayRegion() {
  // generate origion xray
  const unsigned int Dimension = 3;
  typedef double InputPixelType;
  typedef double OutputPixleType;
  typedef itk::Image<InputPixelType, Dimension> InputImageType;

  typedef itk::VTKImageToImageFilter<InputImageType> VTKImageToImageType;
  VTKImageToImageType::Pointer m_xrayfromvtk = VTKImageToImageType::New();
  m_xrayfromvtk->SetInput(m_imageXRay);
  m_xrayfromvtk->Update();

  typedef double OutputPixelType;

  typedef itk::Image<InputPixelType, Dimension> InputImageType;
  typedef itk::Image<OutputPixelType, Dimension> OutputImageType;

  InputImageType::Pointer xrayimage;
  xrayimage = m_xrayfromvtk->GetOutput();

  // projection plane
  double xrayPlaneCenter[3];
  m_imageXRay->GetOrigin(xrayPlaneCenter);
  auto xrayPlane = vtkSmartPointer<vtkPlane>::New();
  xrayPlane->SetOrigin(xrayPlaneCenter);
  xrayPlane->SetNormal(0, 0, 1);

  // get the region of xray
  double projectedXrayNodes[4][3];
  for (int i = 0; i < 4; i++) {

    xrayPlane->ProjectPoint(m_XRayRegionPts[i], projectedXrayNodes[i]);
    for (int j = 0; j < 3; j++) {
      qDebug() << projectedXrayNodes[i][j] << " -re1";
    }
  }
  double xraySpacing[3];
  double xrayOrigion[3];

  m_imageXRay->GetSpacing(xraySpacing);
  m_imageXRay->GetOrigin(xrayOrigion);

  typedef double PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  ImageType::RegionType inputRegion;
  ImageType::RegionType::IndexType inputStart;
  ImageType::RegionType::SizeType size;

  int xrayStart[3], xraySize[3];

  ImageType::RegionType::SizeType maxSize;
  maxSize = xrayimage->GetLargestPossibleRegion().GetSize();

  int minRegionIndex[3], maxRegionIndex[3];
  for (int i = 0; i < 3; i++) {
    minRegionIndex[i] =
        (projectedXrayNodes[3][i] - xrayOrigion[i]) / xraySpacing[i];
    maxRegionIndex[i] =
        (projectedXrayNodes[1][i] - xrayOrigion[i]) / xraySpacing[i];

    if (minRegionIndex[i] < 0)
      minRegionIndex[i] = 0;
    if (minRegionIndex[i] > maxSize[i])
      minRegionIndex[i] = maxSize[i];

    if (maxRegionIndex[i] < 0)
      maxRegionIndex[i] = 0;
    if (maxRegionIndex[i] > maxSize[i])
      maxRegionIndex[i] = maxSize[i];

    xrayStart[i] = minRegionIndex[i];
    xraySize[i] = maxRegionIndex[i] - minRegionIndex[i];
  }

  for (int i = 0; i < 3; i++) {
    inputStart[i] = xrayStart[i];
    size[i] = xraySize[i];
    qDebug() << size[i] << " " << inputStart[i] << " " << xraySpacing[i];
  }
  size[2] = 1;
  inputRegion.SetSize(size);
  inputRegion.SetIndex(inputStart);

  ImageType::RegionType outputRegion;
  ImageType::RegionType::IndexType outputStart;
  outputStart[0] = 0;
  outputStart[1] = 0;
  outputStart[2] = 0;
  outputRegion.SetSize(size);
  outputRegion.SetIndex(outputStart);

  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetRegions(outputRegion);

  double outputOrgion[3];
  for (int i = 0; i < 3; i++) {
    outputOrgion[i] = xrayStart[i] * xraySpacing[i] + xrayOrigion[i];
  }
  outputImage->SetSpacing(xraySpacing);
  outputImage->SetOrigin(outputOrgion);
  outputImage->Allocate();
  ConstIteratorType inputIt(xrayimage, inputRegion);
  IteratorType outputIt(outputImage, outputRegion);
  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while (!inputIt.IsAtEnd()) {
    outputIt.Set(inputIt.Get());
    ++inputIt;
    ++outputIt;
  }
  m_ExtractedXRayImage = outputImage;
  auto tovtk = itk::ImageToVTKImageFilter<ImageType>::New();
  tovtk->SetInput(m_ExtractedXRayImage);
  tovtk->Update();
  qDebug() << "xray region";
  auto imagedata = vtkSmartPointer<vtkImageData>::New();
  imagedata->DeepCopy(tovtk->GetOutput());
  double scalarRange[2];
  imagedata->GetScalarRange(scalarRange);

  m_xrayIn3D->GetMapper()->SetInputData(imagedata);
  m_xrayIn3D->SetInputData(imagedata);
  m_xrayIn3D->GetProperty()->SetColorLevel(
      (scalarRange[1] - scalarRange[0]) / 2 + scalarRange[0]);
  m_xrayIn3D->GetProperty()->SetColorWindow(scalarRange[1] - scalarRange[0]);
  if (!m_Render3D->GetViewProps()->IsItemPresent(m_xrayIn3D))
    m_Render3D->AddActor(m_xrayIn3D);
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::ExtractCTRegion() {

  // generate origion xray
  const unsigned int Dimension = 3;
  typedef double InputPixelType;
  typedef double OutputPixleType;
  typedef itk::Image<InputPixelType, Dimension> InputImageType;

  typedef itk::VTKImageToImageFilter<InputImageType> VTKImageToImageType;
  VTKImageToImageType::Pointer m_ctfromvtk = VTKImageToImageType::New();
  m_ctfromvtk->SetInput(m_imageVolume);
  m_ctfromvtk->Update();

  typedef unsigned char OutputPixelType;

  typedef itk::Image<InputPixelType, Dimension> InputImageType;
  typedef itk::Image<OutputPixelType, Dimension> OutputImageType;

  InputImageType::Pointer ctimage;
  ctimage = m_ctfromvtk->GetOutput();

  // get the region of ct

  double ctSpacing[3];
  double ctOrigion[3];

  m_imageVolume->GetSpacing(ctSpacing);
  m_imageVolume->GetOrigin(ctOrigion);

  typedef double PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
  typedef itk::ImageRegionIterator<ImageType> IteratorType;
  ImageType::RegionType inputRegion;
  ImageType::RegionType::IndexType inputStart;
  ImageType::RegionType::SizeType size;

  int ctStart[3], ctSize[3];
  int minusSize[3] = {0, 0, 0};
  ImageType::RegionType::SizeType maxSize;
  maxSize = ctimage->GetLargestPossibleRegion().GetSize();
  int minRegionIndex[3], maxRegionIndex[3];
  for (int i = 0; i < 3; i++) {
    minRegionIndex[i] = (m_CTRegionPts[4][i] - ctOrigion[i]) / ctSpacing[i];
    maxRegionIndex[i] = (m_CTRegionPts[2][i] - ctOrigion[i]) / ctSpacing[i];

    if (minRegionIndex[i] < 0)
      minRegionIndex[i] = 0;
    if (minRegionIndex[i] > maxSize[i])
      minRegionIndex[i] = maxSize[i];

    if (maxRegionIndex[i] < 0)
      maxRegionIndex[i] = 0;
    if (maxRegionIndex[i] > maxSize[i])
      maxRegionIndex[i] = maxSize[i];

    ctStart[i] = minRegionIndex[i];
    ctSize[i] = maxRegionIndex[i] - minRegionIndex[i];
  }

  for (int i = 0; i < 3; i++) {
    inputStart[i] = ctStart[i];
    size[i] = ctSize[i];
    qDebug() << size[i] << " " << inputStart[i] << " " << ctSpacing[i];
  }

  inputRegion.SetSize(size);
  inputRegion.SetIndex(inputStart);

  ImageType::RegionType outputRegion;
  ImageType::RegionType::IndexType outputStart;
  outputStart[0] = 0;
  outputStart[1] = 0;
  outputStart[2] = 0;
  outputRegion.SetSize(size);
  outputRegion.SetIndex(outputStart);

  ImageType::Pointer outputImage = ImageType::New();
  outputImage->SetRegions(outputRegion);

  double outputOrgion[3];
  for (int i = 0; i < 3; i++) {
    outputOrgion[i] = ctStart[i] * ctSpacing[i] + ctOrigion[i];
  }
  outputImage->SetSpacing(ctSpacing);
  outputImage->SetOrigin(outputOrgion);
  outputImage->Allocate();
  ConstIteratorType inputIt(ctimage, inputRegion);
  IteratorType outputIt(outputImage, outputRegion);
  inputIt.GoToBegin();
  outputIt.GoToBegin();

  while (!inputIt.IsAtEnd()) {
    outputIt.Set(inputIt.Get());
    ++inputIt;
    ++outputIt;
  }

  m_ExtractedCTImage = outputImage;
  this->VisualizeSelectedCTRegion();
}

void MainWindow::VisualizeSelectedCTRegion() {
  auto tovtk = itk::ImageToVTKImageFilter<ImageType>::New();
  tovtk->SetInput(m_ExtractedCTImage);
  tovtk->Update();
  qDebug() << "xray region";
  auto imagedata = vtkSmartPointer<vtkImageData>::New();
  imagedata->DeepCopy(tovtk->GetOutput());

  auto colortransformFunction =
      vtkSmartPointer<vtkColorTransferFunction>::New();
  colortransformFunction->AddRGBPoint(-3096, 0, 0.0, 0.0);
  colortransformFunction->AddRGBPoint(199, 0, 0, 0);
  colortransformFunction->AddRGBPoint(200.0, 1, 0.2, 0.6);
  colortransformFunction->AddRGBPoint(500, 1, 0.2, 0.6);
  colortransformFunction->AddRGBPoint(501, 0, 0, 0);
  colortransformFunction->AddRGBPoint(1024, 0, 0, 0);
  auto opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
  opacityFunction->AddPoint(-3096, 0);
  opacityFunction->AddPoint(199, 0);
  opacityFunction->AddPoint(200, 1);
  opacityFunction->AddPoint(500, 1);
  opacityFunction->AddPoint(501, 0);
  opacityFunction->AddPoint(1024, 0);

  auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
  volumeProperty->SetColor(colortransformFunction);
  volumeProperty->SetScalarOpacity(opacityFunction);
  volumeProperty->ShadeOn();
  volumeProperty->SetInterpolationTypeToLinear();

  auto volumeMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
  volumeMapper->SetInputData(imagedata);
  volumeMapper->SetSampleDistance(0.5);
  volumeMapper->SetAutoAdjustSampleDistances(1);
  volumeMapper->SetBlendModeToComposite();

  m_CTRegionVolume->SetMapper(volumeMapper);
  m_CTRegionVolume->SetProperty(volumeProperty);

  if (!m_Render3D->GetVolumes()->IsItemPresent(m_CTRegionVolume))
    m_Render3D->AddVolume(m_CTRegionVolume);

  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::TryRegV4() {
  typedef itk::Euler3DTransform<double> TransformType;
  typedef itk::FRPROptimizer OptimizerType;
  typedef itk::NormalizedGradientCorrelationMultiImageToImageMetric<ImageType,
                                                                    ImageType>
      MetricType;
  typedef itk::RayCastInterpolateImageFunction<ImageType, double>
      InterpolatorType;

  typedef itk::MultiResolutionMultiImageToImageRegistrationMethod<ImageType,
                                                                  ImageType>
      RegistrationType;

  MetricType::Pointer metric = MetricType::New();
  TransformType::Pointer transform = TransformType::New();
  OptimizerType::Pointer optimizer = OptimizerType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  RegistrationType::Pointer registration = RegistrationType::New();

  registration->SetMultiMetric(metric);
  transform->SetIdentity();
  unsigned int parTotal = transform->GetNumberOfParameters();
  OptimizerType::ScalesType scales(parTotal);
  scales.Fill(itk::NumericTraits<OptimizerType::ScalesType::ValueType>::One);
  scales[0] = 25.0;
  scales[1] = 25.0;
  scales[2] = 25.0;

  optimizer->SetScales(scales);
  optimizer->SetMaximize(true);
  optimizer->SetMaximumIteration(200);
  optimizer->SetMaximumLineIteration(10);
  optimizer->SetValueTolerance(1e-3);
  optimizer->SetUseUnitLengthGradient(true);
  optimizer->SetToPolakRibiere();
  optimizer->SetCatchGetValueException(true);
  optimizer->SetMetricWorstPossibleValue(
      -itk::NumericTraits<MetricType::MeasureType>::infinity());

  optimizer->SetStepTolerance(0.08);
  optimizer->SetStepLength(8);

  registration->SetOptimizer(optimizer);
  // define schedules
  const unsigned int ResolutionLevels = 3;
  RegistrationType::ScheduleType fixedSchedule(ResolutionLevels, 3);
  fixedSchedule[0][0] = 4;
  fixedSchedule[0][1] = 4;
  fixedSchedule[0][2] = 1;
  fixedSchedule[1][0] = 2;
  fixedSchedule[1][1] = 2;
  fixedSchedule[1][2] = 1;
  fixedSchedule[2][0] = 1;
  fixedSchedule[2][1] = 1;
  fixedSchedule[2][2] = 1;

  RegistrationType::ScheduleType movingSchedule(ResolutionLevels, 3);
  movingSchedule[0][0] = 4;
  movingSchedule[0][1] = 4;
  movingSchedule[0][2] = 4;
  movingSchedule[1][0] = 2;
  movingSchedule[1][1] = 2;
  movingSchedule[1][2] = 2;
  movingSchedule[2][0] = 1;
  movingSchedule[2][1] = 1;
  movingSchedule[2][2] = 1;

  registration->SetSchedules(fixedSchedule, movingSchedule);

  typedef itk::MultiResolutionPyramidImageFilter<ImageType, ImageType>
      FixedImagePyramidType;
  FixedImagePyramidType::Pointer fixedPyramidFilter =
      FixedImagePyramidType::New();
  registration->AddFixedImage(m_ExtractedXRayImage);
  registration->AddFixedImagePyramid(fixedPyramidFilter);

  typedef itk::MultiResolutionPyramidImageFilter<ImageType, ImageType>
      MovingImagePyramidType;
  MovingImagePyramidType::Pointer movingPyramidFilter =
      MovingImagePyramidType::New();
  registration->SetMovingImagePyramid(movingPyramidFilter);
  registration->SetMovingImage(m_ExtractedCTImage);

  ImageType::PointType origion;
  origion = m_ExtractedXRayImage->GetOrigin();

  ImageType::PointType spacing;
  spacing = m_ExtractedXRayImage->GetSpacing();

  ImageType::RegionType::SizeType largeSize;
  largeSize = m_ExtractedXRayImage->GetLargestPossibleRegion().GetSize();

  InterpolatorType::InputPointType focalPoint;
  focalPoint[0] = origion[0] + spacing[0] * largeSize[0] / 2.0;
  focalPoint[1] = origion[1] + spacing[1] * largeSize[1] / 2.0;
  focalPoint[2] = 200;
  interpolator->SetFocalPoint(focalPoint);
  interpolator->SetTransform(transform);
  interpolator->SetThreshold(0.0);

  ImageType::PointType origionCT;
  origionCT = m_ExtractedCTImage->GetOrigin();
  ImageType::PointType spacingCT;
  spacingCT = m_ExtractedCTImage->GetSpacing();
  ImageType::SizeType sizeCT;
  sizeCT = m_ExtractedCTImage->GetLargestPossibleRegion().GetSize();

  registration->AddInterpolator(interpolator);
  registration->SetTransform(transform);
  registration->SetInitialTransformParameters(transform->GetParameters());

  try {
    registration->Update();
  } catch (itk::ExceptionObject &e) {
    std::cout << e.GetDescription() << std::endl;
    return;
  }

  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;
  ResampleType::Pointer resampler = ResampleType::New();
  resampler->SetInput(m_ExtractedCTImage);
  resampler->SetInterpolator(interpolator);
  resampler->SetTransform(transform);
  resampler->SetUseReferenceImage(true);
  resampler->SetReferenceImage(m_ExtractedXRayImage);
  resampler->Update();

  m_ExtractedXRayImage = resampler->GetOutput();
  auto tovtk = itk::ImageToVTKImageFilter<ImageType>::New();
  tovtk->SetInput(m_ExtractedXRayImage);
  tovtk->Update();
  qDebug() << "xray region registered";
  auto imagedata = vtkSmartPointer<vtkImageData>::New();
  imagedata->DeepCopy(tovtk->GetOutput());
  double scalarRange[2];
  imagedata->GetScalarRange(scalarRange);

  m_xrayIn3D->GetMapper()->SetInputData(imagedata);
  m_xrayIn3D->SetInputData(imagedata);
  m_xrayIn3D->GetProperty()->SetColorLevel(
      (scalarRange[1] - scalarRange[0]) / 2 + scalarRange[0]);
  m_xrayIn3D->GetProperty()->SetColorWindow(scalarRange[1] - scalarRange[0]);
  if (!m_Render3D->GetViewProps()->IsItemPresent(m_xrayIn3D))
    m_Render3D->AddActor(m_xrayIn3D);
  m_Render3D->GetRenderWindow()->Render();
  qDebug() << "xray region registered";
}

void MainWindow::InitializePlanningWidget() {
  double origion[3] = {0, 0, 0};
  double directionY[3] = {0, 1, 0};
  double directionX[3] = {1, 0, 0};

  double kneeUp[3], kneeDown[3], ankle[3];
  for (int i = 0; i < 3; i++) {
    kneeUp[i] = origion[i] + directionY[i] * 400 - directionX[i] * 20;
    kneeDown[i] = kneeUp[i] + directionY[i] * 20;
    ankle[i] = kneeDown[i] + directionY[i] * 400 + directionX[i] * 20;
  }

  m_femurWidget =
      new TYCircleWidget(0, m_Render2D, m_Interactor2D, origion, 70);
  connect(m_femurWidget, SIGNAL(circleChanged()), this,
          SLOT(OnChangeFemurWidget()));
  m_kneeUpWidget =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, kneeUp, 0.5, 200);
  connect(m_kneeUpWidget, SIGNAL(lineChanged()), this,
          SLOT(OnChangeKneeUpWidget()));
  m_kneeDownWidget =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, kneeDown, 0.5, 200);
  double colorBlue[3] = {0, 0, 1};
  m_kneeDownWidget->SetColor(colorBlue);

  connect(m_kneeDownWidget, SIGNAL(lineChanged()), this,
          SLOT(OnChangeKneeDownWidget()));
  m_ankleWidget =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, ankle, 0.5, 200);
  connect(m_ankleWidget, SIGNAL(lineChanged()), this,
          SLOT(OnChangeAnkleWidget()));

  m_femur2kneeupLine = new TYLineWidget(0, m_Render2D, m_Interactor2D, origion);
  m_femur2kneeupLine->SetWidgetOff(true);
  m_femur2kneeupLine->SetPoint1and2(origion, kneeUp);

  m_kneedown2ankleLine =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, origion);
  m_kneedown2ankleLine->SetWidgetOff(true);
  m_kneedown2ankleLine->SetPoint1and2(kneeDown, ankle);

  m_femur2ankleLine = new TYLineWidget(0, m_Render2D, m_Interactor2D, origion);
  m_femur2ankleLine->SetWidgetOff(true);
  m_femur2ankleLine->SetPoint1and2(origion, ankle);
  m_femur2ankleLine->SetDottedLine(true);

  m_femurKneeDownLine =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, origion);
  m_femurKneeDownLine->SetWidgetOff(true);
  m_femurKneeDownLine->SetDottedLine(true);
  m_femurKneeDownLine->SetColor(colorBlue);

  double kneeDownDirection[3];
  double kneeDownp1[3], kneeDownp2[3];
  auto kneeDownPoints = vtkSmartPointer<vtkPoints>::New();
  m_kneeDownWidget->GetPoints(kneeDownPoints);
  kneeDownPoints->GetPoint(0, kneeDownp1);
  kneeDownPoints->GetPoint(2, kneeDownp2);
  double kneeDownReferencePoint[3];
  for (int i = 0; i < 3; i++) {
    kneeDownDirection[i] = kneeDownp1[i] - kneeDownp2[i];
    kneeDownReferencePoint[i] = origion[i] + kneeDownDirection[i];
  }
  m_femurKneeDownLine->SetPoint1and2(kneeDownReferencePoint, origion);

  // angle 1
  m_TextActorFemurKneeDown = vtkSmartPointer<vtkFollower>::New();
  m_TextActorFemurKneeDown->SetCamera(m_Render2D->GetActiveCamera());
  m_TextActorFemurKneeDown->SetScale(30);
  m_TextActorFemurKneeDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_TextActorFemurKneeDown);
  m_GuideActorFemurKneeDown = vtkSmartPointer<vtkActor>::New();
  m_GuideActorFemurKneeDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_GuideActorFemurKneeDown);

  this->UpDateVectorActor(m_TextActorFemurKneeDown, m_GuideActorFemurKneeDown,
                          kneeDownReferencePoint, origion, kneeDown, origion);
  // angle2
  m_TextActorFemurKneeUp = vtkSmartPointer<vtkFollower>::New();
  m_TextActorFemurKneeUp->SetCamera(m_Render2D->GetActiveCamera());
  m_TextActorFemurKneeUp->SetScale(30);
  m_TextActorFemurKneeUp->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_TextActorFemurKneeUp);
  double kneeUpPoint1[3];
  m_kneeUpWidget->GetPoint1(kneeUpPoint1);

  m_GuideActorFemurKneeUp = vtkSmartPointer<vtkActor>::New();
  m_GuideActorFemurKneeUp->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_GuideActorFemurKneeUp);

  this->UpDateVectorActor(m_TextActorFemurKneeUp, m_GuideActorFemurKneeUp,
                          kneeUpPoint1, kneeUp, origion, kneeUp);

  // angle 3
  m_TextActorAnkleKneeDown = vtkSmartPointer<vtkFollower>::New();
  m_TextActorAnkleKneeDown->SetCamera(m_Render2D->GetActiveCamera());
  m_TextActorAnkleKneeDown->SetScale(30);
  m_TextActorAnkleKneeDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_TextActorAnkleKneeDown);

  m_GuideActorAnkleKneeDown = vtkSmartPointer<vtkActor>::New();
  m_GuideActorAnkleKneeDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_GuideActorAnkleKneeDown);

  double kneeDownPoint2[3];
  m_kneeDownWidget->GetPoint2(kneeDownPoint2);
  this->UpDateVectorActor(m_TextActorAnkleKneeDown, m_GuideActorAnkleKneeDown,
                          kneeDownPoint2, kneeDown, ankle, kneeDown);
  // angle 4
  m_TextActorAnkleAnkle = vtkSmartPointer<vtkFollower>::New();
  m_TextActorAnkleAnkle->SetCamera(m_Render2D->GetActiveCamera());
  m_TextActorAnkleAnkle->SetScale(30);
  m_TextActorAnkleAnkle->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_TextActorAnkleAnkle);
  m_GuideActorAnkleAnkle = vtkSmartPointer<vtkActor>::New();
  m_GuideActorAnkleAnkle->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_GuideActorAnkleAnkle);

  double anklePoint1[3];
  m_ankleWidget->GetPoint1(anklePoint1);
  this->UpDateVectorActor(m_TextActorAnkleAnkle, m_GuideActorAnkleAnkle,
                          anklePoint1, ankle, kneeDown, ankle);

  // angle 5
  m_TextActorKneeUpDown = vtkSmartPointer<vtkFollower>::New();
  m_TextActorKneeUpDown->SetCamera(m_Render2D->GetActiveCamera());
  m_TextActorKneeUpDown->SetScale(30);
  m_TextActorKneeUpDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_TextActorKneeUpDown);

  m_GuideActorKneeUpDown = vtkSmartPointer<vtkActor>::New();
  m_GuideActorKneeUpDown->GetProperty()->SetColor(1, 1, 0);
  m_Render2D->AddActor(m_GuideActorKneeUpDown);

  this->UpDateVectorActor(m_TextActorKneeUpDown, m_GuideActorKneeUpDown,
                          kneeUpPoint1, kneeUp, kneeDownp1, kneeDown);

  // fujisawa line
  double colorRed[3] = {1, 0, 0};
  m_fujisawaLine =
      new TYLineWidget(0, m_Render2D, m_Interactor2D, kneeUp, 0.375);
  m_fujisawaLine->SetColor(colorRed);
}

double MainWindow::CalculateAngle(double *p1, double *p2, double *pp1,
                                  double *pp2, double *position,
                                  vtkPolyData *guideData) {
  double length1 = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  double length2 = sqrt(vtkMath::Distance2BetweenPoints(pp1, pp2));
  double Length = length1 > length2 ? length2 : length1;
  double direction[3];
  double p12[3], pp12[3];
  for (int i = 0; i < 3; i++) {
    p12[i] = p1[i] - p2[i];
    pp12[i] = pp1[i] - pp2[i];
    direction[i] = p12[i] + pp12[i];
  }
  vtkMath::Normalize(p12);
  vtkMath::Normalize(pp12);
  vtkMath::Normalize(direction);
  double cameraDirection[3];
  m_Render2D->GetActiveCamera()->GetDirectionOfProjection(cameraDirection);

  auto points = vtkSmartPointer<vtkPoints>::New();
  points->Initialize();
  double guidePoints[4][3];
  for (int i = 0; i < 3; i++) {
    guidePoints[0][i] = p2[i] + length1 / 4.0 * p12[i];
    guidePoints[2][i] = pp2[i] + length2 / 4.0 * pp12[i];
  }
  double guideDirection1[3], guideDirection2[3];
  for (int i = 0; i < 3; i++) {
    position[i] = direction[i] * Length + p1[i] - cameraDirection[i] * 2;
    guideDirection1[i] = position[i] - guidePoints[0][i];
    guideDirection2[i] = position[i] - guidePoints[2][i];
  }
  vtkMath::Normalize(guideDirection1);
  vtkMath::Normalize(guideDirection2);
  for (int i = 0; i < 3; i++) {
    guidePoints[1][i] = position[i] - 60 * guideDirection1[i];
    guidePoints[3][i] = position[i] - guideDirection2[i] * 60;
  }

  for (int i = 0; i < 4; i++) {
    points->InsertNextPoint(guidePoints[i]);
  }
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  auto id1 = vtkSmartPointer<vtkIdList>::New();
  id1->Initialize();
  id1->InsertNextId(0);
  id1->InsertNextId(1);
  cells->InsertNextCell(id1);

  auto id2 = vtkSmartPointer<vtkIdList>::New();
  id2->Initialize();
  id2->InsertNextId(2);
  id2->InsertNextId(3);
  cells->InsertNextCell(id2);
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  pd->SetPoints(points);
  pd->SetLines(cells);
  guideData->DeepCopy(pd);
  double angle = vtkMath::AngleBetweenVectors(p12, pp12);
  return vtkMath::DegreesFromRadians(angle);
}

void MainWindow::UpDateVectorActor(vtkFollower *actor, vtkActor *guideActor,
                                   double p1[], double p2[], double pp1[],
                                   double pp2[]) {
  double textActorPosition[3];
  auto guideData = vtkSmartPointer<vtkPolyData>::New();

  double angle = CalculateAngle(p1, p2, pp1, pp2, textActorPosition, guideData);
  auto vectorText = vtkSmartPointer<vtkVectorText>::New();
  vectorText->SetText(QString::number(angle, 'g', 4).toStdString().c_str());
  vectorText->Update();

  auto guideMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  guideMapper->SetInputData(guideData);
  guideActor->SetMapper(guideMapper);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(vectorText->GetOutput());

  actor->SetMapper(mapper);
  actor->SetPosition(textActorPosition);
}

void MainWindow::OnDICOMBrowser() { m_dicomBrowser->show(); }

void MainWindow::OnImportVolume(int index) {

  m_imageVolume = m_dicomBrowser->GetItemImageData(index);

  m_imageVolume->GetOrigin(m_volumeOrigin);
  m_imageVolume->GetSpacing(m_volumeSpacing);
  m_imageVolume->GetExtent(m_volumeExtend);

  for (int i = 0; i < 3; i++) {
    m_volumeCenter[i] = m_volumeOrigin[i] +
                        (m_volumeExtend[2 * i + 1] - m_volumeExtend[2 * i]) /
                            2.0 * m_volumeSpacing[i];
  }
  if (ui->BoneRadioButton->isChecked()) {
    m_colortransformFunction->RemoveAllPoints();

    m_colortransformFunction->AddRGBPoint(0.0, 0.5, 0.0, 0.0);
    m_colortransformFunction->AddRGBPoint(60.0, 1.0, 0.5, 0.5);
    m_colortransformFunction->AddRGBPoint(120.0, 0.5, 0.5, 0.3);
    m_colortransformFunction->AddRGBPoint(170.0, 0.5, 0.77, 0.6);
    m_colortransformFunction->AddRGBPoint(405.0, 0.5, 0.9, 0.9);

    m_opacityFunction->RemoveAllPoints();
    m_opacityFunction->AddPoint(7.0, 0.0);
    m_opacityFunction->AddPoint(59.0, 0);
    m_opacityFunction->AddPoint(60.0, 0);
    m_opacityFunction->AddPoint(195.0, 0);
    m_opacityFunction->AddPoint(100, .2);
    m_opacityFunction->AddPoint(100, .3);
    m_opacityFunction->AddPoint(200, .3);
    m_opacityFunction->AddPoint(495.0, 1.0);

    m_gradientFunction->RemoveAllPoints();
    m_gradientFunction->AddPoint(50, .2);
    m_gradientFunction->AddPoint(1500, .7);
    m_gradientFunction->AddPoint(2000, .1);
  }

  m_volumeMapper->SetInputData(m_imageVolume);

  bool isContained = m_Render3D->GetVolumes()->IsItemPresent(m_volumeActor);
  if (!isContained) {
    m_Render3D->AddVolume(m_volumeActor);
    m_Render3D->ResetCamera();
  }

  m_GenericRendWin3D->Render();
}

void MainWindow::OnImportXRay(int index) {
  qDebug() << index;
  m_imageXRay = m_dicomBrowser->GetItemImageData(index);
  double scaleRange[2];
  m_imageXRay->GetScalarRange(scaleRange);
  double origion[3], spacing[3];
  int extend[6];
  m_imageXRay->GetOrigin(origion);
  m_imageXRay->GetSpacing(spacing);
  m_imageXRay->GetExtent(extend);
  qDebug() << "origion:" << origion[0] << " " << origion[1] << " "
           << origion[2];
  qDebug() << "spacing:" << spacing[0] << " " << spacing[1] << " "
           << spacing[2];
  qDebug() << "extend:" << extend[0] << " " << extend[1] << " " << extend[2]
           << " " << extend[3] << " " << extend[4] << " " << extend[5];

  m_sliceMapper2D->SetInputData(m_imageXRay);
  m_sliceMapper2D->SetOrientationToZ();

  if (!m_Render2D->GetViewProps()->IsItemPresent(m_slice2D)) {
    m_slice2D->GetProperty()->SetColorLevel(
        (scaleRange[1] - scaleRange[0]) / 2 + scaleRange[0]);
    m_slice2D->GetProperty()->SetColorWindow(scaleRange[1] - scaleRange[0]);
    m_slice2D->SetMapper(m_sliceMapper2D);
    m_Render2D->AddViewProp(m_slice2D);
  }
  m_Render2D->ResetCamera();
  m_GenericRendWin2D->Render();
}

void MainWindow::OnSetVolumeRep() {
  //  if (ui->VolumeRepRadioButton->isChecked()) {
  //    bool isContained =
  //    m_Render3D->GetVolumes()->IsItemPresent(m_volumeActor);
  //    if (!isContained) {
  //      m_Render3D->AddVolume(m_volumeActor);
  //      m_Render3D->ResetCamera();
  //    }
  //  }
  m_GenericRendWin3D->Render();
}

void MainWindow::OnCloseWindow() {}

void MainWindow::OnSetSurceWidget() {
  if (m_imageVolume->GetDimensions()[2] < 2)
    return;
  double _sphereCenter[3];
  _sphereCenter[0] = m_volumeCenter[0] + ui->XOffSetSpinBox->value();
  _sphereCenter[1] = m_volumeCenter[1] + ui->YOffSetSpinBox->value();
  _sphereCenter[2] = m_volumeCenter[2] + ui->ZOffSetSpinBox->value();

  m_surceSphereWidget->SetCenter(_sphereCenter);
  m_surceSphereWidget->SetRadius(50);
  m_surceSphereWidget->SetInteractor(m_Interactor3D);
  m_surceSphereWidget->On();

  m_GenericRendWin3D->Render();
  //  this->GenerateDRR();
}

void MainWindow::OnSurceWidgetInteraction() {
  // this->GenerateDRR();
  qDebug() << " inter  " << m_Interactor2D->GetEventPosition()[0] << " "
           << m_Interactor2D->GetEventPosition()[0];
}

void MainWindow::OnGenerateDRR() {}

void MainWindow::OnCTVolumeVisibility() {
  if (m_volumeActor->GetVisibility())
    m_volumeActor->VisibilityOff();
  else
    m_volumeActor->VisibilityOn();
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::OnFemoralCenterButton() {
  if (ui->FemoralCenterButton->isChecked()) {
    m_moveFemoral->VisibilityOn();
    m_fixFemoral->VisibilityOff();
    m_FemoralSide1->Off();
    m_FemoralSide2->Off();
    m_FemoralSide3->Off();
    m_FemoralOrigion->Off();

    ui->KneeCenterButton->setDisabled(true);
    ui->AnkleCenterButton->setDisabled(true);
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnFemoralMove()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::KeyPressEvent, this,
                              SLOT(OnFemoralCircleChanged()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::LeftButtonPressEvent,
                              this, SLOT(OnFemoralSet()));
    qDebug() << "femove connected";
  } else {
    m_moveFemoral->VisibilityOff();
    m_fixFemoral->VisibilityOn();
    this->UpDateForceLine();
    double side1[3];
    m_FemoralSide1->GetCenter(side1);
    if (side1[0] == 0 && side1[1] == 0 && side1[2] == 0) {
    } else {
      m_FemoralSide1->On();
      m_FemoralSide2->On();
      m_FemoralSide3->On();
      m_FemoralOrigion->On();
    }
    ui->KneeCenterButton->setDisabled(false);
    ui->AnkleCenterButton->setDisabled(false);
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnFemoralMove()));
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::KeyPressEvent,
                                 this, SLOT(OnFemoralCircleChanged()));
    m_vtkqtConnector->Disconnect(m_Interactor2D,
                                 vtkCommand::LeftButtonPressEvent, this,
                                 SLOT(OnFemoralSet()));
  }
  m_Render2D->GetRenderWindow()->Render();
}

void MainWindow::OnFemoralMove() {

  //投影到世界坐标系
  ui->Widget2D->setFocus();
  QPoint mouse = ui->Widget2D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget2D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render2D, x, y, 0, worldpos);
  qDebug() << "world:" << worldpos[0] << " " << worldpos[1] << " "
           << worldpos[2];

  for (int i = 0; i < 3; i++)
    m_FemoralMoveCenter[i] = worldpos[i];
  //计算圆
  double zdirection[3] = {0, 0, 1};
  double crossDirection[3];

  auto circlepoints = vtkSmartPointer<vtkPoints>::New();
  circlepoints->Initialize();
  for (int i = 0; i < 360; i++) {
    vtkMath::Perpendiculars(zdirection, nullptr, crossDirection,
                            i / 180.0 * vtkMath::Pi());
    double p[3];
    for (int j = 0; j < 3; j++)
      p[j] = worldpos[j] + crossDirection[j] * m_FemoralRadius;
    circlepoints->InsertNextPoint(p);
  }
  auto lines = vtkSmartPointer<vtkCellArray>::New();
  lines->Initialize();
  for (int i = 0; i < 360; i++) {
    auto idlist = vtkSmartPointer<vtkIdList>::New();
    idlist->Initialize();
    if (i == 359) {
      idlist->InsertNextId(359);
      idlist->InsertNextId(0);
    } else {
      idlist->InsertNextId(i);
      idlist->InsertNextId(i + 1);
    }
    lines->InsertNextCell(idlist);
  }
  auto circlePD = vtkSmartPointer<vtkPolyData>::New();
  circlePD->SetPoints(circlepoints);
  circlePD->SetLines(lines);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(circlePD);
  m_moveFemoral->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();
  this->UpDateForceLine();
}

void MainWindow::OnFemoralCircleChanged() {
  char *key = m_Interactor2D->GetKeySym();
  QString str = QString(QLatin1String(key));
  qDebug() << key;
  if (str == "equal") {
    m_FemoralRadius += 1;
  }
  if (str == "minus") {
    m_FemoralRadius -= 1;
  }
  OnFemoralMove();
}

void MainWindow::OnFemoralSet() {
  for (int i = 0; i < 3; i++) {
    m_FemoralCenter[i] = m_FemoralMoveCenter[i];
  }

  //计算圆
  double zdirection[3] = {0, 0, 1};
  double crossDirection[3];
  double side1[3], side2[3], side3[3];
  auto circlepoints = vtkSmartPointer<vtkPoints>::New();
  circlepoints->Initialize();
  for (int i = 0; i < 360; i++) {
    vtkMath::Perpendiculars(zdirection, nullptr, crossDirection,
                            i / 180.0 * vtkMath::Pi());

    double p[3];
    for (int j = 0; j < 3; j++) {
      p[j] = m_FemoralCenter[j] + crossDirection[j] * m_FemoralRadius;
      if (i == 0)
        side1[j] = p[j];
      if (i == 119)
        side2[j] = p[j];
      if (i == 239)
        side3[j] = p[j];
    }
    circlepoints->InsertNextPoint(p);
  }
  auto lines = vtkSmartPointer<vtkCellArray>::New();
  lines->Initialize();
  for (int i = 0; i < 360; i++) {
    auto idlist = vtkSmartPointer<vtkIdList>::New();
    idlist->Initialize();
    if (i == 359) {
      idlist->InsertNextId(359);
      idlist->InsertNextId(0);
    } else {
      idlist->InsertNextId(i);
      idlist->InsertNextId(i + 1);
    }
    lines->InsertNextCell(idlist);
  }
  auto circlePD = vtkSmartPointer<vtkPolyData>::New();
  circlePD->SetPoints(circlepoints);
  circlePD->SetLines(lines);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(circlePD);
  m_fixFemoral->SetMapper(mapper);
  m_fixFemoral->VisibilityOn();
  m_Render2D->GetRenderWindow()->Render();
  side1[2] += 20;
  m_FemoralSide1->SetCenter(side1);
  m_FemoralSide1->SetRadius(10);
  m_FemoralSide1->On();
  side2[2] += 20;
  m_FemoralSide2->SetCenter(side2);
  m_FemoralSide2->SetRadius(10);
  m_FemoralSide2->On();
  side3[2] += 20;
  m_FemoralSide3->SetCenter(side3);
  m_FemoralSide3->SetRadius(10);

  m_FemoralSide3->On();
  m_FemoralOrigion->SetCenter(m_FemoralCenter[0], m_FemoralCenter[1],
                              m_FemoralCenter[2] + 10);
  m_FemoralOrigion->SetRadius(10);
  m_FemoralOrigion->On();

  ui->FemoralCenterButton->setChecked(false);
  this->OnFemoralCenterButton();

  this->OnFemoralSideChange();

  this->UpDateForceLine();
}

void MainWindow::OnFemoralSideChange() {
  double side1[3], side2[3], side3[3];
  m_FemoralSide1->GetCenter(side1);
  m_FemoralSide2->GetCenter(side2);
  m_FemoralSide3->GetCenter(side3);

  this->CalculateCircle(side1, side2, side3, m_FemoralCenter, m_FemoralRadius);
  m_FemoralOrigion->SetCenter(m_FemoralCenter);
  this->UpDateFemoralCircle();
}

void MainWindow::OnFemoralOrigionChange() {
  double changedCenter[3];
  m_FemoralOrigion->GetCenter(changedCenter);
  double side1[3], side2[3], side3[3];
  m_FemoralSide1->GetCenter(side1);
  m_FemoralSide2->GetCenter(side2);
  m_FemoralSide3->GetCenter(side3);
  double distance[3];
  for (int i = 0; i < 3; i++) {
    distance[i] = changedCenter[i] - m_FemoralCenter[i];
    m_FemoralCenter[i] = changedCenter[i];
    side1[i] = side1[i] + distance[i];
    side2[i] = side2[i] + distance[i];
    side3[i] = side3[i] + distance[i];
  }
  m_FemoralSide1->SetCenter(side1);
  m_FemoralSide2->SetCenter(side2);
  m_FemoralSide3->SetCenter(side3);

  this->UpDateFemoralCircle();
}

void MainWindow::OnKneeCenterButton() {
  if (ui->KneeCenterButton->isChecked()) {
    ui->FemoralCenterButton->setDisabled(true);
    ui->AnkleCenterButton->setDisabled(true);

    m_KneeP1->Off();
    m_KneeP2->Off();
    m_KneeOrigion->Off();
    m_moveKnee->VisibilityOn();
    m_fixKnee->VisibilityOff();
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnKneeMove()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::KeyPressEvent, this,
                              SLOT(OnKneeChanged()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::LeftButtonPressEvent,
                              this, SLOT(OnKneeSet()));
  } else {
    ui->FemoralCenterButton->setDisabled(false);
    ui->AnkleCenterButton->setDisabled(false);

    double p1[3];
    m_KneeP1->GetCenter(p1);
    if (p1[0] == 0 && p1[1] == 0 && p1[2] == 0) {
    } else {
      m_KneeP1->On();
      m_KneeP2->On();
      m_KneeOrigion->On();
    }

    m_moveKnee->VisibilityOff();
    m_fixKnee->VisibilityOn();
    this->UpDateForceLine();

    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnKneeMove()));
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::KeyPressEvent,
                                 this, SLOT(OnKneeChanged()));
    m_vtkqtConnector->Disconnect(m_Interactor2D,
                                 vtkCommand::LeftButtonPressEvent, this,
                                 SLOT(OnKneeSet()));
  }
}

void MainWindow::OnKneeMove() {
  ui->Widget2D->setFocus();
  QPoint mouse = ui->Widget2D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget2D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render2D, x, y, 0, worldpos);
  qDebug() << "world:" << worldpos[0] << " " << worldpos[1] << " "
           << worldpos[2];

  for (int i = 0; i < 3; i++)
    m_KneeMoveCenter[i] = worldpos[i];

  //计算直线
  double zdireciton[3] = {0, 0, 1};
  double crossDirection[3];
  vtkMath::Perpendiculars(zdireciton, nullptr, crossDirection, 0);
  double p1[3], p2[3];
  for (int i = 0; i < 3; i++) {
    if (m_KneeisFlip) {
      p1[i] = m_KneeMoveCenter[i] - 0.375 * m_KneeLength * crossDirection[i];
      p2[i] = m_KneeMoveCenter[i] + 0.625 * m_KneeLength * crossDirection[i];
    } else {
      p1[i] = m_KneeMoveCenter[i] - 0.625 * m_KneeLength * crossDirection[i];
      p2[i] = m_KneeMoveCenter[i] + 0.375 * m_KneeLength * crossDirection[i];
    }
  }
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_moveKnee->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnKneeChanged() {
  char *key = m_Interactor2D->GetKeySym();
  QString str = QString(QLatin1String(key));
  qDebug() << key;
  if (str == "equal") {
    m_KneeLength += 5;
  }
  if (str == "minus") {
    m_KneeLength -= 5;
  }
  if (str == "space") {
    m_KneeisFlip = !m_KneeisFlip;
  }
  OnKneeMove();
}

void MainWindow::OnKneeSet() {
  for (int i = 0; i < 3; i++) {
    m_KneeCenter[i] = m_KneeMoveCenter[i];
  }
  //计算直线
  double zdireciton[3] = {0, 0, 1};
  double crossDirection[3];
  vtkMath::Perpendiculars(zdireciton, nullptr, crossDirection, 0);
  double p1[3], p2[3];
  for (int i = 0; i < 3; i++) {
    if (m_KneeisFlip) {
      p1[i] = m_KneeMoveCenter[i] - 0.375 * m_KneeLength * crossDirection[i];
      p2[i] = m_KneeMoveCenter[i] + 0.625 * m_KneeLength * crossDirection[i];
    } else {
      p1[i] = m_KneeMoveCenter[i] - 0.625 * m_KneeLength * crossDirection[i];
      p2[i] = m_KneeMoveCenter[i] + 0.375 * m_KneeLength * crossDirection[i];
    }
  }
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixKnee->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  m_KneeP1->SetCenter(p1[0], p1[1], p1[2] + 10);
  m_KneeP1->SetRadius(10);
  m_KneeP1->On();

  m_KneeP2->SetCenter(p2[0], p2[1], p2[2] + 10);
  m_KneeP2->SetRadius(10);
  m_KneeP2->On();

  m_KneeOrigion->SetCenter(m_KneeCenter[0], m_KneeCenter[1],
                           m_KneeCenter[2] + 10);
  m_KneeOrigion->SetRadius(10);
  m_KneeOrigion->On();

  ui->KneeCenterButton->setChecked(false);
  this->OnKneeCenterButton();
  this->OnKneeSideChange();
  this->UpDateForceLine();
}

void MainWindow::OnKneeSideChange() {
  double p1[3], p2[3];
  m_KneeP1->GetCenter(p1);
  m_KneeP2->GetCenter(p2);
  double direction[3];
  for (int i = 0; i < 3; i++)
    direction[i] = p2[i] - p1[i];
  vtkMath::Normalize(direction);
  m_KneeLength = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
  for (int i = 0; i < 3; i++) {
    if (m_KneeisFlip)
      m_KneeCenter[i] = p1[i] + 0.357 * direction[i] * m_KneeLength;
    else
      m_KneeCenter[i] = p1[i] + 0.625 * direction[i] * m_KneeLength;
  }
  m_KneeOrigion->SetCenter(m_KneeCenter);

  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixKnee->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnKneeOrigionChange() {
  double origion[3];
  m_KneeOrigion->GetCenter(origion);
  double changeDistance[3];
  for (int i = 0; i < 3; i++) {
    changeDistance[i] = origion[i] - m_KneeCenter[i];
    m_KneeCenter[i] = origion[i];
  }
  double p1[3], p2[3];
  m_KneeP1->GetCenter(p1);
  m_KneeP2->GetCenter(p2);
  for (int i = 0; i < 3; i++) {
    p1[i] += changeDistance[i];
    p2[i] += changeDistance[i];
  }
  m_KneeP1->SetCenter(p1);
  m_KneeP2->SetCenter(p2);

  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixKnee->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnAnkleCenterButton() {
  if (ui->AnkleCenterButton->isChecked()) {
    m_fixAnkle->VisibilityOff();
    m_moveAnkle->VisibilityOn();
    m_AnkleP1->Off();
    m_AnkleP2->Off();
    m_AnkleOrigion->Off();
    ui->FemoralCenterButton->setDisabled(true);
    ui->KneeCenterButton->setDisabled(true);
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnAnkleMove()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::KeyPressEvent, this,
                              SLOT(OnAnkleChanged()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::LeftButtonPressEvent,
                              this, SLOT(OnAnkleSet()));
  } else {
    double p1[3];
    if (p1[0] == 0 && p1[1] == 0 && p1[2] == 0) {
    } else {
      m_AnkleP1->On();
      m_AnkleP2->On();
      m_AnkleOrigion->On();
    }
    m_fixAnkle->VisibilityOn();
    m_moveAnkle->VisibilityOff();
    this->UpDateForceLine();

    ui->FemoralCenterButton->setDisabled(false);
    ui->KneeCenterButton->setDisabled(false);
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnAnkleMove()));
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::KeyPressEvent,
                                 this, SLOT(OnAnkleChanged()));
    m_vtkqtConnector->Disconnect(m_Interactor2D,
                                 vtkCommand::LeftButtonPressEvent, this,
                                 SLOT(OnAnkleSet()));
  }
}

void MainWindow::OnAnkleMove() {
  ui->Widget2D->setFocus();
  QPoint mouse = ui->Widget2D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget2D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render2D, x, y, 0, worldpos);
  qDebug() << "world:" << worldpos[0] << " " << worldpos[1] << " "
           << worldpos[2];

  for (int i = 0; i < 3; i++)
    m_AnkleMoveCenter[i] = worldpos[i];

  //计算直线
  double zdireciton[3] = {0, 0, 1};
  double crossDirection[3];
  vtkMath::Perpendiculars(zdireciton, nullptr, crossDirection, 0);
  double p1[3], p2[3];
  for (int i = 0; i < 3; i++) {

    p1[i] = m_AnkleMoveCenter[i] - 0.5 * m_AnkleLength * crossDirection[i];
    p2[i] = m_AnkleMoveCenter[i] + 0.5 * m_AnkleLength * crossDirection[i];
  }
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_moveAnkle->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnAnkleChanged() {
  char *key = m_Interactor2D->GetKeySym();
  QString str = QString(QLatin1String(key));
  qDebug() << key;
  if (str == "equal") {
    m_AnkleLength += 5;
  }
  if (str == "minus") {
    m_AnkleLength -= 5;
  }
  OnAnkleMove();
}

void MainWindow::OnAnkleSet() {
  for (int i = 0; i < 3; i++)
    m_AnkleCenter[i] = m_AnkleMoveCenter[i];

  //计算直线
  double zdireciton[3] = {0, 0, 1};
  double crossDirection[3];
  vtkMath::Perpendiculars(zdireciton, nullptr, crossDirection, 0);
  double p1[3], p2[3];
  for (int i = 0; i < 3; i++) {

    p1[i] = m_AnkleCenter[i] - 0.5 * m_AnkleLength * crossDirection[i];
    p2[i] = m_AnkleCenter[i] + 0.5 * m_AnkleLength * crossDirection[i];
  }
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixAnkle->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  m_AnkleP1->SetCenter(p1[0], p1[1], p1[2] + 10);
  m_AnkleP1->SetRadius(10);
  m_AnkleP1->On();

  m_AnkleP2->SetCenter(p2[0], p2[1], p2[2] + 10);
  m_AnkleP2->SetRadius(10);
  m_AnkleP2->On();

  m_AnkleOrigion->SetCenter(m_AnkleCenter[0], m_AnkleCenter[1],
                            m_AnkleCenter[2] + 10);
  m_AnkleOrigion->SetRadius(10);
  m_AnkleOrigion->On();

  ui->AnkleCenterButton->setChecked(false);
  this->OnAnkleCenterButton();
  this->OnAnkleSideChanged();

  this->UpDateForceLine();
}

void MainWindow::OnAnkleSideChanged() {
  double p1[3], p2[3];
  m_AnkleP1->GetCenter(p1);
  m_AnkleP2->GetCenter(p2);

  for (int i = 0; i < 3; i++) {
    m_AnkleCenter[i] = (p1[i] + p2[i]) / 2;
  }
  m_AnkleOrigion->SetCenter(m_AnkleCenter);
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixAnkle->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnAnkleOrigionChange() {
  double origion[3];
  m_AnkleOrigion->GetCenter(origion);
  m_AnkleOrigion->GetCenter(origion);
  double changeDistance[3];
  for (int i = 0; i < 3; i++) {
    changeDistance[i] = origion[i] - m_AnkleCenter[i];
    m_AnkleCenter[i] = origion[i];
  }
  double p1[3], p2[3];
  m_AnkleP1->GetCenter(p1);
  m_AnkleP2->GetCenter(p2);
  for (int i = 0; i < 3; i++) {
    p1[i] += changeDistance[i];
    p2[i] += changeDistance[i];
  }
  m_AnkleP1->SetCenter(p1);
  m_AnkleP2->SetCenter(p2);
  auto linepoints = vtkSmartPointer<vtkPoints>::New();
  linepoints->Initialize();
  linepoints->InsertNextPoint(p1);
  linepoints->InsertNextPoint(p2);
  auto idlist = vtkSmartPointer<vtkIdList>::New();
  idlist->Initialize();
  idlist->InsertNextId(0);
  idlist->InsertNextId(1);
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  cells->InsertNextCell(idlist);
  auto linePD = vtkSmartPointer<vtkPolyData>::New();
  linePD->SetPoints(linepoints);
  linePD->SetLines(cells);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(linePD);
  m_fixAnkle->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  this->UpDateForceLine();
}

void MainWindow::OnXRayRegionButton() {
  if (ui->XRayRegionButton->isChecked()) {
    m_moveXrayActor->VisibilityOn();
    m_fixXrayActor->VisibilityOff();
    m_XrayRegionWidgetP1->Off();
    m_XrayRegionWidgetP2->Off();
    m_XrayRegionWidgetP3->Off();
    m_XrayRegionWidgetP4->Off();
    m_XRayRegionWidgetOrigon->Off();
    ui->CTRegionButton->setDisabled(true);
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnXRayRegionMove()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::LeftButtonPressEvent,
                              this, SLOT(OnXRayRegionSet()));
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::KeyPressEvent, this,
                              SLOT(OnXRayRegionLengthChanged()));
  } else {
    m_moveXrayActor->VisibilityOff();
    m_fixXrayActor->VisibilityOn();
    if (m_XRayRegionFixCenter[0] == 0 && m_XRayRegionFixCenter[1] == 0 &&
        m_XRayRegionFixCenter[2] == 0) {
    } else {
      m_XrayRegionWidgetP1->On();
      m_XrayRegionWidgetP1->On();
      m_XrayRegionWidgetP1->On();
      m_XrayRegionWidgetP1->On();
      m_XRayRegionWidgetOrigon->On();
    }
    ui->CTRegionButton->setDisabled(false);
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnXRayRegionMove()));
    m_vtkqtConnector->Disconnect(m_Interactor2D,
                                 vtkCommand::LeftButtonPressEvent, this,
                                 SLOT(OnXRayRegionSet()));
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::KeyPressEvent,
                                 this, SLOT(OnXRayRegionLengthChanged()));
  }
}

void MainWindow::OnXRayRegionMove() {
  ui->Widget2D->setFocus();
  QPoint mouse = ui->Widget2D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget2D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render2D, x, y, 0, worldpos);
  qDebug() << "world:" << worldpos[0] << " " << worldpos[1] << " "
           << worldpos[2];
  for (int i = 0; i < 3; i++)
    m_XRayRegionMoveCenter[i] = worldpos[i];

  //计算方框
  double p1[3], p2[3], p3[3], p4[4];
  double direct1[3] = {-1 / 3.0, 1, 0};
  double direct2[3] = {1 / 3.0, 1, 0};
  double direct3[3] = {1 / 3.0, -1, 0};
  double direct4[3] = {-1 / 3.0, -1, 0};
  for (int i = 0; i < 3; i++) {
    p1[i] = m_XRayRegionMoveCenter[i] + m_XRayLength * direct1[i];
    p2[i] = m_XRayRegionMoveCenter[i] + m_XRayLength * direct2[i];
    p3[i] = m_XRayRegionMoveCenter[i] + m_XRayLength * direct3[i];
    p4[i] = m_XRayRegionMoveCenter[i] + m_XRayLength * direct4[i];
  }
  auto framePd = vtkSmartPointer<vtkPolyData>::New();
  this->BuildFrame(p1, p2, p3, p4, framePd);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(framePd);
  m_moveXrayActor->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();
}

void MainWindow::OnXRayRegionLengthChanged() {
  char *key = m_Interactor2D->GetKeySym();
  QString str = QString(QLatin1String(key));
  qDebug() << key;
  if (str == "equal") {
    m_XRayLength += 50;
  }
  if (str == "minus") {
    m_XRayLength -= 50;
  }
  OnXRayRegionMove();
}

void MainWindow::OnXRayRegionSet() {
  for (int i = 0; i < 3; i++)
    m_XRayRegionFixCenter[i] = m_XRayRegionMoveCenter[i];
  //计算方框
  double direct1[3] = {-1 / 3.0, 1, 0};
  double direct2[3] = {1 / 3.0, 1, 0};
  double direct3[3] = {1 / 3.0, -1, 0};
  double direct4[3] = {-1 / 3.0, -1, 0};
  for (int i = 0; i < 3; i++) {
    m_XRayRegionPts[0][i] =
        m_XRayRegionFixCenter[i] + m_XRayLength * direct1[i];
    m_XRayRegionPts[1][i] =
        m_XRayRegionFixCenter[i] + m_XRayLength * direct2[i];
    m_XRayRegionPts[2][i] =
        m_XRayRegionFixCenter[i] + m_XRayLength * direct3[i];
    m_XRayRegionPts[3][i] =
        m_XRayRegionFixCenter[i] + m_XRayLength * direct4[i];
    m_XRayRegionPts[4][i] = m_XRayRegionMoveCenter[i];
  }

  auto framePd = vtkSmartPointer<vtkPolyData>::New();
  this->BuildFrame(m_XRayRegionPts[0], m_XRayRegionPts[1], m_XRayRegionPts[2],
                   m_XRayRegionPts[3], framePd);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(framePd);
  m_fixXrayActor->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();

  ui->XRayRegionButton->setChecked(false);
  this->OnXRayRegionButton();
  for (int i = 0; i < 5; i++) {
    m_XRayRegionPts[i][2] += 10;
  }
  m_XrayRegionWidgetP1->SetCenter(m_XRayRegionPts[0]);
  m_XrayRegionWidgetP1->SetRadius(10);
  m_XrayRegionWidgetP1->On();

  m_XrayRegionWidgetP2->SetCenter(m_XRayRegionPts[1]);
  m_XrayRegionWidgetP2->SetRadius(10);
  m_XrayRegionWidgetP2->On();

  m_XrayRegionWidgetP3->SetCenter(m_XRayRegionPts[2]);
  m_XrayRegionWidgetP3->SetRadius(10);
  m_XrayRegionWidgetP3->On();

  m_XrayRegionWidgetP4->SetCenter(m_XRayRegionPts[3]);
  m_XrayRegionWidgetP4->SetRadius(10);
  m_XrayRegionWidgetP4->On();

  m_XRayRegionWidgetOrigon->SetCenter(m_XRayRegionPts[4]);
  m_XRayRegionWidgetOrigon->SetRadius(10);
  m_XRayRegionWidgetOrigon->On();
  this->OnXRayRegionSideChanged();
}

void MainWindow::OnXRayRegionSideChanged() {
  double p1[3], p2[3], p3[3], p4[3], p5[3];
  m_XrayRegionWidgetP1->GetCenter(p1);
  m_XrayRegionWidgetP2->GetCenter(p2);
  m_XrayRegionWidgetP3->GetCenter(p3);
  m_XrayRegionWidgetP4->GetCenter(p4);
  m_XRayRegionWidgetOrigon->GetCenter(p5);
  bool isp1changed = isEqual(p1, m_XRayRegionPts[0]);
  bool isp2changed = isEqual(p2, m_XRayRegionPts[1]);
  bool isp3changed = isEqual(p3, m_XRayRegionPts[2]);
  bool isp4changed = isEqual(p4, m_XRayRegionPts[3]);
  bool isp5changed = isEqual(p5, m_XRayRegionPts[4]);
  qDebug() << isp1changed << "  " << isp2changed << "  " << isp3changed << "  "
           << isp4changed << "  " << isp5changed << "  ";
  double distance[3];

  if (isp1changed) {
    for (int i = 0; i < 3; i++) {
      distance[i] = p1[i] - m_XRayRegionPts[0][i];
      m_XRayRegionPts[0][i] = p1[i];
      m_XRayRegionPts[4][i] += distance[i] / 2;
    }
    m_XRayRegionPts[1][1] += distance[1];
    m_XRayRegionPts[3][0] += distance[0];
  }
  if (isp2changed) {
    for (int i = 0; i < 3; i++) {
      distance[i] = p2[i] - m_XRayRegionPts[1][i];
      m_XRayRegionPts[1][i] = p2[i];
      m_XRayRegionPts[4][i] += distance[i] / 2;
    }
    m_XRayRegionPts[0][1] += distance[1];
    m_XRayRegionPts[2][0] += distance[0];
  }
  if (isp3changed) {
    for (int i = 0; i < 3; i++) {
      distance[i] = p3[i] - m_XRayRegionPts[2][i];
      m_XRayRegionPts[2][i] = p3[i];
      m_XRayRegionPts[4][i] += distance[i] / 2;
    }
    m_XRayRegionPts[3][1] += distance[1];
    m_XRayRegionPts[1][0] += distance[0];
  }
  if (isp4changed) {
    for (int i = 0; i < 3; i++) {
      distance[i] = p4[i] - m_XRayRegionPts[3][i];
      m_XRayRegionPts[3][i] = p4[i];
      m_XRayRegionPts[4][i] += distance[i] / 2;
    }
    m_XRayRegionPts[2][1] += distance[1];
    m_XRayRegionPts[0][0] += distance[0];
  }
  if (isp5changed) {
    for (int i = 0; i < 3; i++) {
      distance[i] = p5[i] - m_XRayRegionPts[4][i];
      m_XRayRegionPts[4][i] = p5[i];
    }
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++)
        m_XRayRegionPts[i][j] += distance[j];
    }
  }
  m_XrayRegionWidgetP1->SetCenter(m_XRayRegionPts[0]);

  m_XrayRegionWidgetP2->SetCenter(m_XRayRegionPts[1]);

  m_XrayRegionWidgetP3->SetCenter(m_XRayRegionPts[2]);

  m_XrayRegionWidgetP4->SetCenter(m_XRayRegionPts[3]);

  m_XRayRegionWidgetOrigon->SetCenter(m_XRayRegionPts[4]);

  auto framePd = vtkSmartPointer<vtkPolyData>::New();
  this->BuildFrame(m_XRayRegionPts[0], m_XRayRegionPts[1], m_XRayRegionPts[2],
                   m_XRayRegionPts[3], framePd);

  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(framePd);
  m_fixXrayActor->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();
}

void MainWindow::OnCTRegionButton() {
  if (ui->CTRegionButton->isChecked()) {
    m_moveCTActor->VisibilityOn();
    m_fixCTActor->VisibilityOff();
    ui->XRayRegionButton->setDisabled(true);
    m_vtkqtConnector->Connect(m_Interactor3D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnCTRegionMove()));
    m_vtkqtConnector->Connect(m_Interactor3D, vtkCommand::RightButtonPressEvent,
                              this, SLOT(OnCTRegionSet()));
  } else {
    m_moveCTActor->VisibilityOff();
    m_fixCTActor->VisibilityOn();
    ui->XRayRegionButton->setDisabled(false);
    m_vtkqtConnector->Disconnect(m_Interactor3D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnCTRegionMove()));
    m_vtkqtConnector->Disconnect(m_Interactor3D,
                                 vtkCommand::RightButtonPressEvent, this,
                                 SLOT(OnCTRegionSet()));
  }
}

void MainWindow::OnCTRegionMove() {
  ui->Widget3D->setFocus();
  QPoint mouse = ui->Widget3D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget3D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render3D, x, y, 0, worldpos);
  qDebug() << "world:" << worldpos[0] << " " << worldpos[1] << " "
           << worldpos[2];
  for (int i = 0; i < 3; i++)
    m_CTRegionMoveCenter[i] = worldpos[i];
  double cameradirection[3];
  double planecenter[3];
  for (int i = 0; i < 3; i++) {
    cameradirection[i] =
        m_Render3D->GetActiveCamera()->GetDirectionOfProjection()[i];
    planecenter[i] = m_imageVolume->GetCenter()[i];
  }
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetOrigin(planecenter);
  plane->SetNormal(cameradirection);

  plane->ProjectPoint(m_CTRegionMoveCenter, m_CTRegionMoveCenter);
  double direction[8][3] = {{-1, -1, 1.5}, {1, -1, 1.5},   {1, 1, 1.5},
                            {-1, 1, 1.5},  {-1, -1, -1.5}, {1, -1, -1.5},
                            {1, 1, -1.5},  {-1, 1, -1.5}};
  double p[8][3];
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 3; j++) {
      p[i][j] = m_CTRegionMoveCenter[j] + m_CTLength * direction[i][j];
    }
  }
  auto box = vtkSmartPointer<vtkPolyData>::New();
  this->BuildBox(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], box);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(box);
  m_moveCTActor->SetMapper(mapper);
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::OnCTRegionSet() {
  for (int i = 0; i < 3; i++) {
    m_CTRegionFixCenter[i] = m_CTRegionMoveCenter[i];
  }
  double direction[8][3] = {{-1, -1, 1.5}, {1, -1, 1.5},   {1, 1, 1.5},
                            {-1, 1, 1.5},  {-1, -1, -1.5}, {1, -1, -1.5},
                            {1, 1, -1.5},  {-1, 1, -1.5}};
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 3; j++) {
      if (i == 8)
        m_CTRegionPts[i][j] = m_CTRegionFixCenter[j];
      else
        m_CTRegionPts[i][j] =
            m_CTRegionFixCenter[j] + m_CTLength * direction[i][j];
    }
  }

  auto box = vtkSmartPointer<vtkPolyData>::New();
  this->BuildBox(m_CTRegionPts[0], m_CTRegionPts[1], m_CTRegionPts[2],
                 m_CTRegionPts[3], m_CTRegionPts[4], m_CTRegionPts[5],
                 m_CTRegionPts[6], m_CTRegionPts[7], box);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(box);
  m_fixCTActor->SetMapper(mapper);
  m_Render3D->GetRenderWindow()->Render();
  ui->CTRegionButton->setChecked(false);
  this->OnCTRegionButton();

  for (int i = 0; i < 9; i++) {
    m_CTSphereList.at(i)->SetCenter(m_CTRegionPts[i]);
    m_CTSphereList.at(i)->SetRadius(10);
    m_CTSphereList.at(i)->On();
  }
}

void MainWindow::OnCTRegionChanged() {
  double p[3];
  int changedIndex = -1;
  double distance[3];
  for (int m = 0; m < 9; m++) {
    m_CTSphereList.at(m)->GetCenter(p);
    if (isEqual(p, m_CTRegionPts[m])) {
      changedIndex = m;
      for (int i = 0; i < 3; i++) {
        distance[i] = p[i] - m_CTRegionPts[m][i];
        m_CTRegionPts[m][i] = p[i];
      }
      break;
    }
  }
  if (changedIndex == -1)
    return;
  if (changedIndex != 8) {
    for (int i = 0; i < 3; i++)
      m_CTRegionPts[8][i] += distance[i] * 0.5;
  }
  qDebug() << changedIndex;

  if (changedIndex == 0) {
    m_CTRegionPts[4][0] += distance[0];
    m_CTRegionPts[4][1] += distance[1];

    m_CTRegionPts[1][2] += distance[2];
    m_CTRegionPts[1][1] += distance[1];

    m_CTRegionPts[3][2] += distance[2];
    m_CTRegionPts[3][0] += distance[0];

    m_CTRegionPts[2][2] += distance[2];
    m_CTRegionPts[5][1] += distance[1];
    m_CTRegionPts[7][0] += distance[0];
  }
  if (changedIndex == 1) {
    m_CTRegionPts[0][2] += distance[2];
    m_CTRegionPts[0][1] += distance[1];

    m_CTRegionPts[5][0] += distance[0];
    m_CTRegionPts[5][1] += distance[1];

    m_CTRegionPts[2][2] += distance[2];
    m_CTRegionPts[2][0] += distance[0];

    m_CTRegionPts[6][0] += distance[0];
    m_CTRegionPts[4][1] += distance[1];
    m_CTRegionPts[3][2] += distance[2];
  }
  if (changedIndex == 2) {
    m_CTRegionPts[1][2] += distance[2];
    m_CTRegionPts[1][0] += distance[0];

    m_CTRegionPts[3][2] += distance[2];
    m_CTRegionPts[3][1] += distance[1];

    m_CTRegionPts[6][1] += distance[1];
    m_CTRegionPts[6][0] += distance[0];

    m_CTRegionPts[0][2] += distance[2];
    m_CTRegionPts[7][1] += distance[1];
    m_CTRegionPts[5][0] += distance[0];
  }
  if (changedIndex == 3) {
    m_CTRegionPts[0][2] += distance[2];
    m_CTRegionPts[0][0] += distance[0];

    m_CTRegionPts[2][2] += distance[2];
    m_CTRegionPts[2][1] += distance[1];

    m_CTRegionPts[7][1] += distance[1];
    m_CTRegionPts[7][0] += distance[0];

    m_CTRegionPts[4][0] += distance[0];
    m_CTRegionPts[6][1] += distance[1];
    m_CTRegionPts[1][2] += distance[2];
  }
  if (changedIndex == 4) {
    m_CTRegionPts[0][1] += distance[1];
    m_CTRegionPts[0][0] += distance[0];

    m_CTRegionPts[5][2] += distance[2];
    m_CTRegionPts[5][1] += distance[1];

    m_CTRegionPts[7][2] += distance[2];
    m_CTRegionPts[7][0] += distance[0];

    m_CTRegionPts[3][0] += distance[0];
    m_CTRegionPts[1][1] += distance[1];
    m_CTRegionPts[6][2] += distance[2];
  }
  if (changedIndex == 5) {
    m_CTRegionPts[1][1] += distance[1];
    m_CTRegionPts[1][0] += distance[0];

    m_CTRegionPts[4][2] += distance[2];
    m_CTRegionPts[4][1] += distance[1];

    m_CTRegionPts[6][2] += distance[2];
    m_CTRegionPts[6][0] += distance[0];

    m_CTRegionPts[0][1] += distance[1];
    m_CTRegionPts[2][0] += distance[0];
    m_CTRegionPts[7][2] += distance[2];
  }
  if (changedIndex == 6) {
    m_CTRegionPts[2][1] += distance[1];
    m_CTRegionPts[2][0] += distance[0];

    m_CTRegionPts[5][2] += distance[2];
    m_CTRegionPts[5][0] += distance[0];

    m_CTRegionPts[7][2] += distance[2];
    m_CTRegionPts[7][1] += distance[1];

    m_CTRegionPts[1][0] += distance[0];
    m_CTRegionPts[3][1] += distance[1];
    m_CTRegionPts[4][2] += distance[2];
  }

  if (changedIndex == 7) {
    m_CTRegionPts[3][1] += distance[1];
    m_CTRegionPts[3][0] += distance[0];

    m_CTRegionPts[4][2] += distance[2];
    m_CTRegionPts[4][0] += distance[0];

    m_CTRegionPts[6][2] += distance[2];
    m_CTRegionPts[6][1] += distance[1];

    m_CTRegionPts[0][0] += distance[0];
    m_CTRegionPts[2][1] += distance[1];
    m_CTRegionPts[5][2] += distance[2];
  }
  if (changedIndex == 8) {

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 3; j++) {
        m_CTRegionPts[i][j] += distance[j];
      }
    }
  }

  for (int i = 0; i < 9; i++) {
    m_CTSphereList.at(i)->SetCenter(m_CTRegionPts[i]);
  }
  auto box = vtkSmartPointer<vtkPolyData>::New();
  this->BuildBox(m_CTRegionPts[0], m_CTRegionPts[1], m_CTRegionPts[2],
                 m_CTRegionPts[3], m_CTRegionPts[4], m_CTRegionPts[5],
                 m_CTRegionPts[6], m_CTRegionPts[7], box);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(box);
  m_fixCTActor->SetMapper(mapper);
  m_Render3D->GetRenderWindow()->Render();
  ui->CTRegionButton->setChecked(false);
  this->OnCTRegionButton();
}

void MainWindow::OnXrayIn3DVisibilityButton() {
  if (m_xrayIn3D->GetVisibility())
    m_xrayIn3D->VisibilityOff();
  else
    m_xrayIn3D->VisibilityOn();
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::OnCTRegionVisibilityButton() {
  if (m_CTRegionVolume->GetVisibility())
    m_CTRegionVolume->VisibilityOff();
  else
    m_CTRegionVolume->VisibilityOn();
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::OnRemoveCTNoiseButton() {
  if (ui->RemoveCTNoiseButton->isChecked()) {
    ui->XRayRegionButton->setDisabled(true);
    ui->CTRegionButton->setDisabled(true);
    this->setCursor(Qt::CrossCursor);
    m_vtkqtConnector->Connect(m_Interactor3D, vtkCommand::RightButtonPressEvent,
                              this, SLOT(OnRemoveCTNoiseSet()));
  } else {
    ui->XRayRegionButton->setDisabled(false);
    ui->CTRegionButton->setDisabled(false);
    this->setCursor(Qt::ArrowCursor);
    m_vtkqtConnector->Disconnect(m_Interactor3D,
                                 vtkCommand::RightButtonPressEvent, this,
                                 SLOT(OnRemoveCTNoiseSet()));
  }
}

void MainWindow::OnRemoveCTNoiseSet() {
  ui->Widget3D->setFocus();
  QPoint mouse = ui->Widget3D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget3D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];

  vtkInteractorObserver::ComputeDisplayToWorld(m_Render3D, x, y, 0, worldpos);

  ui->RemoveCTNoiseButton->setChecked(false);
  this->OnRemoveCTNoiseButton();
  auto volumePicker = vtkSmartPointer<vtkVolumePicker>::New();
  volumePicker->AddPickList(m_CTRegionVolume);
  volumePicker->PickFromListOn();

  volumePicker->SetVolumeOpacityIsovalue(1);
  volumePicker->Pick(x, y, 0, m_Render3D);

  volumePicker->GetPickPosition(worldpos);
  m_CTRemoveWidget->SetRadius(12);
  m_CTRemoveWidget->SetCenter(worldpos[0], worldpos[1], worldpos[2]);
  m_CTRemoveWidget->On();
  // calculate the selected point index
  ImageType::RegionType::SizeType regionsize;
  regionsize = m_ExtractedCTImage->GetLargestPossibleRegion().GetSize();
  ImageType::SpacingType ctspacing;
  ctspacing = m_ExtractedCTImage->GetSpacing();
  ImageType::PointType ctorigion;
  ctorigion = m_ExtractedCTImage->GetOrigin();

  ImageType::IndexType selectedIndex;
  for (int i = 0; i < 3; i++) {
    selectedIndex[i] = (worldpos[i] - ctorigion[i]) / ctspacing[i];
    if ((selectedIndex[i] < 0) | (selectedIndex[i] > regionsize[i])) {
      return; // if out the range,return}
    }
  }

  // use connect threshold to extract target region
  ImageType::RegionType largetestRegion;
  largetestRegion = m_ExtractedCTImage->GetLargestPossibleRegion();

  ImageType::Pointer backupImage = ImageType::New();
  //  backupImage->SetRegions(largetestRegion);
  //  backupImage->Allocate();

  typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType>
      ConnectedFillterType;
  ConnectedFillterType::Pointer connectedThreshold =
      ConnectedFillterType::New();
  connectedThreshold->SetInput(m_ExtractedCTImage);
  connectedThreshold->SetLower(190);
  connectedThreshold->SetUpper(510);
  connectedThreshold->SetReplaceValue(-4000);
  connectedThreshold->SetSeed(selectedIndex);
  connectedThreshold->SetCoordinateTolerance(1);
  connectedThreshold->Update();

  backupImage = connectedThreshold->GetOutput();

  itk::ImageRegionIterator<ImageType> origionIterator(m_ExtractedCTImage,
                                                      largetestRegion);
  itk::ImageRegionConstIterator<ImageType> backupIterator(backupImage,
                                                          largetestRegion);
  origionIterator.GoToBegin();
  backupIterator.GoToBegin();

  while (!origionIterator.IsAtEnd()) {
    if (backupIterator.Get() == -4000) {
      origionIterator.Set(0);
    }
    ++backupIterator;
    ++origionIterator;
  }
  this->VisualizeSelectedCTRegion();
}

void MainWindow::OnStartRegestration() {
  // check
  if (m_fixCTActor->GetMapper()->GetInput()->GetNumberOfPoints() == 0)
    return;
  if (m_fixXrayActor->GetMapper()->GetInput()->GetNumberOfPoints() == 0)
    return;
  this->ExtractXRayRegion();
  this->ExtractCTRegion();

  TryRegV4();
  return;
  //  typedef itk::Image<double, 3> ImageType;

  //  typedef itk::Euler3DTransform<double> TransformType;
  //  typedef itk::ImageRegistrationMethodv4<ImageType, ImageType,
  //  TransformType>
  //      RegistrationType;

  //  typedef itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType>
  //  MetricType;
  //  typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;

  //  typedef itk::LinearInterpolateImageFunction<ImageType, double>
  //      FixedInterpolatorType;
  //  typedef itk::RayCastInterpolateImageFunction<ImageType, double>
  //      MovingInterpolatorType;

  //  RegistrationType::Pointer registration = RegistrationType::New();
  //  TransformType::Pointer transform = TransformType::New();
  //  OptimizerType::Pointer optimizer = OptimizerType::New();
  //  MetricType::Pointer metric = MetricType::New();
  //  FixedInterpolatorType::Pointer fixedInterpolator =
  //      FixedInterpolatorType::New();
  //  MovingInterpolatorType::Pointer movingInterpolator =
  //      MovingInterpolatorType::New();

  //  ImageType::PointType origion = m_ExtractedCTImage->GetOrigin();
  //  ImageType::SpacingType spacing = m_ExtractedCTImage->GetSpacing();
  //  ImageType::RegionType::SizeType size =
  //      m_ExtractedCTImage->GetLargestPossibleRegion().GetSize();

  //  MovingInterpolatorType::InputPointType movingFocalPoint;
  //  movingFocalPoint[0] = origion[0] + 200;
  //  movingFocalPoint[1] = origion[1] + size[1] / 2 * spacing[1];
  //  movingFocalPoint[2] = origion[2] + size[2] / 2 * spacing[2];

  //  movingInterpolator->SetFocalPoint(movingFocalPoint);
  //  movingInterpolator->SetThreshold(0);
  //  metric->SetFixedInterpolator(fixedInterpolator);
  //  metric->SetMovingInterpolator(movingInterpolator);

  //  registration->SetMetric(metric);
  //  registration->SetFixedImage(m_ExtractedXRayImage);
  //  registration->SetMovingImage(m_ExtractedCTImage);

  //  optimizer->SetLearningRate(0.2);
  //  optimizer->SetMinimumStepLength(0.001);
  //  optimizer->SetRelaxationFactor(0.5);
  //  optimizer->SetNumberOfIterations(200);
  //  optimizer->SetReturnBestParametersAndValue(true);

  //  registration->SetOptimizer(optimizer);
  //  try {
  //    registration->Update();
  //  } catch (itk::ExceptionObject &e) {
  //    std::cout << e.GetDescription() << std::endl;
  //    return;
  //  }

  // define registration type
  typedef itk::MultiResolutionMultiImageToImageRegistrationMethod<ImageType,
                                                                  ImageType>
      RegistrationType;
  RegistrationType::Pointer registration = RegistrationType::New();

  // define transform
  typedef itk::Euler3DTransform<double> TransformType;
  TransformType::Pointer transfom = TransformType::New();
  transfom->SetIdentity();
  registration->SetTransform(transfom);

  // input moving image
  registration->SetMovingImage(m_ExtractedCTImage);
  typedef itk::MultiResolutionPyramidImageFilter<ImageType, ImageType>
      MovingImagePyramidType;
  MovingImagePyramidType::Pointer movingPyramidFilter =
      MovingImagePyramidType::New();
  registration->SetMovingImagePyramid(movingPyramidFilter);

  // input fix image
  ImageType::PointType origion;
  origion = m_ExtractedXRayImage->GetOrigin();

  ImageType::PointType spacing;
  spacing = m_ExtractedXRayImage->GetSpacing();

  ImageType::RegionType::SizeType largeSize;
  largeSize = m_ExtractedXRayImage->GetLargestPossibleRegion().GetSize();

  typedef itk::RayCastInterpolateImageFunction<ImageType, double>
      InterpolatorType;
  InterpolatorType::InputPointType focalPoint;
  // typedef InterpolatorType::InputPointType FocalPointType;
  typedef itk::MultiResolutionPyramidImageFilter<ImageType, ImageType>
      FixedImagePyramidType;
  registration->AddFixedImage(m_ExtractedXRayImage);
  //  registration->AddFixedImageRegion(
  //      m_ExtractedXRayImage->GetLargestPossibleRegion());
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  //  // FocalPointType focalPoint;
  focalPoint[0] = origion[0] + spacing[0] * largeSize[0] / 2.0;
  focalPoint[1] = origion[1] + spacing[1] * largeSize[1] / 2.0;
  focalPoint[2] = origion[2] + 1000;
  interpolator->SetFocalPoint(focalPoint);
  interpolator->SetTransform(transfom);
  interpolator->SetThreshold(0.0);
  registration->AddInterpolator(interpolator);

  FixedImagePyramidType::Pointer fixedPyramidFilter =
      FixedImagePyramidType::New();
  registration->AddFixedImagePyramid(fixedPyramidFilter);

  // define metric
  typedef itk::GradientDifferenceMultiImageToImageMetric<ImageType, ImageType>
      MultiMetricType;
  MultiMetricType::Pointer multiMetric = MultiMetricType::New();
  registration->SetMultiMetric(multiMetric);

  // define optimizer
  typedef itk::FRPROptimizer OptimizerType;
  OptimizerType::Pointer optimizer = OptimizerType::New();

  unsigned int parTotal = transfom->GetNumberOfParameters();
  OptimizerType::ScalesType scales(parTotal);
  scales.Fill(itk::NumericTraits<OptimizerType::ScalesType::ValueType>::One);
  scales[0] = 25.0;
  scales[1] = 25.0;
  scales[2] = 25.0;

  optimizer->SetScales(scales);
  optimizer->SetMaximize(true);
  optimizer->SetMaximumIteration(10);
  optimizer->SetMaximumLineIteration(10);
  optimizer->SetValueTolerance(1e-3);
  optimizer->SetUseUnitLengthGradient(true);
  optimizer->SetToPolakRibiere();
  optimizer->SetCatchGetValueException(true);
  optimizer->SetMetricWorstPossibleValue(
      -itk::NumericTraits<MultiMetricType::MeasureType>::infinity());

  optimizer->SetStepTolerance(0.08);
  optimizer->SetStepLength(8);
  registration->SetOptimizer(optimizer);

  // define schedules
  const unsigned int ResolutionLevels = 3;
  RegistrationType::ScheduleType fixedSchedule(ResolutionLevels, 3);
  fixedSchedule[0][0] = 4;
  fixedSchedule[0][1] = 4;
  fixedSchedule[0][2] = 1;
  fixedSchedule[1][0] = 2;
  fixedSchedule[1][1] = 2;
  fixedSchedule[1][2] = 1;
  fixedSchedule[2][0] = 1;
  fixedSchedule[2][1] = 1;
  fixedSchedule[2][2] = 1;

  RegistrationType::ScheduleType movingSchedule(ResolutionLevels, 3);
  movingSchedule[0][0] = 4;
  movingSchedule[0][1] = 4;
  movingSchedule[0][2] = 4;
  movingSchedule[1][0] = 2;
  movingSchedule[1][1] = 2;
  movingSchedule[1][2] = 2;
  movingSchedule[2][0] = 1;
  movingSchedule[2][1] = 1;
  movingSchedule[2][2] = 1;

  registration->SetSchedules(fixedSchedule, movingSchedule);
  registration->SetInitialTransformParameters(transfom->GetParameters());
  try {
    registration->Update();
  } catch (itk::ExceptionObject &e) {
    std::cout << e.GetDescription() << std::endl;
    return;
  }
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleType;
  ResampleType::Pointer resampler = ResampleType::New();
  resampler->SetInput(m_ExtractedCTImage);
  resampler->SetInterpolator(interpolator);
  resampler->SetTransform(transfom);
  resampler->SetUseReferenceImage(true);
  resampler->SetReferenceImage(m_ExtractedXRayImage);
  resampler->Update();

  m_ExtractedXRayImage = resampler->GetOutput();
  auto tovtk = itk::ImageToVTKImageFilter<ImageType>::New();
  tovtk->SetInput(m_ExtractedXRayImage);
  tovtk->Update();
  qDebug() << "xray region registered";
  auto imagedata = vtkSmartPointer<vtkImageData>::New();
  imagedata->DeepCopy(tovtk->GetOutput());
  double scalarRange[2];
  imagedata->GetScalarRange(scalarRange);

  m_xrayIn3D->GetMapper()->SetInputData(imagedata);
  m_xrayIn3D->SetInputData(imagedata);
  m_xrayIn3D->GetProperty()->SetColorLevel(
      (scalarRange[1] - scalarRange[0]) / 2 + scalarRange[0]);
  m_xrayIn3D->GetProperty()->SetColorWindow(scalarRange[1] - scalarRange[0]);
  if (!m_Render3D->GetViewProps()->IsItemPresent(m_xrayIn3D))
    m_Render3D->AddActor(m_xrayIn3D);
  m_Render3D->GetRenderWindow()->Render();
}

void MainWindow::OnTest() {
  ui->Widget2D->setFocus();
  QPoint mouse = ui->Widget2D->mapFromGlobal(QCursor::pos());

  int x = mouse.x();
  int y = ui->Widget2D->height() - mouse.y();

  qDebug() << "move:" << x << " " << y;
  double worldpos[4];
  vtkInteractorObserver::ComputeDisplayToWorld(m_Render2D, x, y, 0, worldpos);
  double step[3];
  m_femurWidget->SetOrigion(worldpos, step);
  m_kneeUpWidget->OrigionAddStep(step);
  m_kneeDownWidget->OrigionAddStep(step);
  m_ankleWidget->OrigionAddStep(step);

  double kneeUpCenter[3], femurCenter[3];
  m_femurWidget->GetOrigion(femurCenter);
  m_kneeUpWidget->GetOrigion(kneeUpCenter);

  double kneeDownCenter[3], ankleCenter[3];
  m_kneeDownWidget->GetOrigion(kneeDownCenter);
  m_ankleWidget->GetOrigion(ankleCenter);

  m_femur2kneeupLine->SetPoint1and2(femurCenter, kneeUpCenter);
  m_kneedown2ankleLine->SetPoint1and2(kneeDownCenter, ankleCenter);

  m_femur2ankleLine->SetPoint1and2(femurCenter, ankleCenter);

  double kneeDownDirection[3];
  double kneeDownp1[3], kneeDownp2[3];
  auto kneeDownPoints = vtkSmartPointer<vtkPoints>::New();
  m_kneeDownWidget->GetPoints(kneeDownPoints);
  kneeDownPoints->GetPoint(0, kneeDownp1);
  kneeDownPoints->GetPoint(2, kneeDownp2);
  double kneeDownReferencePoint[3];
  for (int i = 0; i < 3; i++) {
    kneeDownDirection[i] = kneeDownp1[i] - kneeDownp2[i];
    kneeDownReferencePoint[i] = femurCenter[i] + kneeDownDirection[i];
  }
  m_femurKneeDownLine->SetPoint1and2(kneeDownReferencePoint, femurCenter);

  // angle 1
  this->UpDateVectorActor(m_TextActorFemurKneeDown, m_GuideActorFemurKneeDown,
                          kneeDownReferencePoint, femurCenter, kneeDownCenter,
                          femurCenter);
  // angle 2
  double kneeUpPoint1[3];
  m_kneeUpWidget->GetPoint1(kneeUpPoint1);
  this->UpDateVectorActor(m_TextActorFemurKneeUp, m_GuideActorFemurKneeUp,
                          kneeUpPoint1, kneeUpCenter, femurCenter,
                          kneeUpCenter);
  // angle 3
  double kneeDownPoint2[3];
  m_kneeDownWidget->GetPoint2(kneeDownPoint2);
  this->UpDateVectorActor(m_TextActorAnkleKneeDown, m_GuideActorAnkleKneeDown,
                          kneeDownPoint2, kneeDownCenter, ankleCenter,
                          kneeDownCenter);
  // angle 4
  double anklePoint1[3];
  m_ankleWidget->GetPoint1(anklePoint1);
  this->UpDateVectorActor(m_TextActorAnkleAnkle, m_GuideActorAnkleAnkle,
                          anklePoint1, ankleCenter, kneeDownCenter,
                          ankleCenter);
  // angle 5
  this->UpDateVectorActor(m_TextActorKneeUpDown, m_GuideActorKneeUpDown,
                          kneeUpPoint1, kneeUpCenter, kneeDownp1,
                          kneeDownCenter);

  m_fujisawaLine->SetOrigion(kneeDownCenter);
}

void MainWindow::OnTestButton() {
  if (ui->testButton->isChecked()) {
    m_vtkqtConnector->Connect(m_Interactor2D, vtkCommand::MouseMoveEvent, this,
                              SLOT(OnTest()));
  } else {
    ui->FemoralCenterButton->setDisabled(false);
    ui->AnkleCenterButton->setDisabled(false);
    m_vtkqtConnector->Disconnect(m_Interactor2D, vtkCommand::MouseMoveEvent,
                                 this, SLOT(OnTest()));
  }
}

void MainWindow::OnChangeFemurWidget() {
  double kneeUpCenter[3], femurCenter[3], ankleCenter[3];
  m_femurWidget->GetOrigion(femurCenter);
  m_kneeUpWidget->GetOrigion(kneeUpCenter);
  m_ankleWidget->GetOrigion(ankleCenter);
  m_femur2kneeupLine->SetPoint1and2(femurCenter, kneeUpCenter);
  m_femur2ankleLine->SetPoint1and2(femurCenter, ankleCenter);

  double kneeDownDirection[3];
  double kneeDownp1[3], kneeDownp2[3];
  auto kneeDownPoints = vtkSmartPointer<vtkPoints>::New();
  m_kneeDownWidget->GetPoints(kneeDownPoints);
  kneeDownPoints->GetPoint(0, kneeDownp1);
  kneeDownPoints->GetPoint(2, kneeDownp2);
  double kneeDownReferencePoint[3];
  for (int i = 0; i < 3; i++) {
    kneeDownDirection[i] = kneeDownp1[i] - kneeDownp2[i];
    kneeDownReferencePoint[i] = femurCenter[i] + kneeDownDirection[i];
  }
  m_femurKneeDownLine->SetPoint1and2(kneeDownReferencePoint, femurCenter);

  double kneeDownCenter[3];
  m_kneeDownWidget->GetOrigion(kneeDownCenter);

  // angle 1
  this->UpDateVectorActor(m_TextActorFemurKneeDown, m_GuideActorFemurKneeDown,
                          kneeDownReferencePoint, femurCenter, kneeDownCenter,
                          femurCenter);
  // angle 2
  double kneeUpPoint1[3];
  m_kneeUpWidget->GetPoint1(kneeUpPoint1);
  this->UpDateVectorActor(m_TextActorFemurKneeUp, m_GuideActorFemurKneeUp,
                          kneeUpPoint1, kneeUpCenter, femurCenter,
                          kneeUpCenter);
}

void MainWindow::OnChangeKneeUpWidget() {
  double kneeUpCenter[3], femurCenter[3];
  m_femurWidget->GetOrigion(femurCenter);
  m_kneeUpWidget->GetOrigion(kneeUpCenter);

  m_femur2kneeupLine->SetPoint1and2(femurCenter, kneeUpCenter);

  // angle 2
  double kneeUpPoint1[3];
  m_kneeUpWidget->GetPoint1(kneeUpPoint1);
  this->UpDateVectorActor(m_TextActorFemurKneeUp, m_GuideActorFemurKneeUp,
                          kneeUpPoint1, kneeUpCenter, femurCenter,
                          kneeUpCenter);
  // angle 5
  double kneeDownPoint1[3];
  m_kneeDownWidget->GetPoint1(kneeDownPoint1);
  double kneeDownCenter[3];
  m_kneeDownWidget->GetOrigion(kneeDownCenter);
  this->UpDateVectorActor(m_TextActorKneeUpDown, m_GuideActorKneeUpDown,
                          kneeUpPoint1, kneeUpCenter, kneeDownPoint1,
                          kneeDownCenter);
  ;
}

void MainWindow::OnChangeKneeDownWidget() {
  double kneeDownCenter[3], ankleCenter[3];
  m_kneeDownWidget->GetOrigion(kneeDownCenter);
  m_ankleWidget->GetOrigion(ankleCenter);

  m_kneedown2ankleLine->SetPoint1and2(kneeDownCenter, ankleCenter);

  double femurCenter[3];
  m_femurWidget->GetOrigion(femurCenter);

  double kneeDownDirection[3];
  double kneeDownp1[3], kneeDownp2[3];

  auto kneeDownPoints = vtkSmartPointer<vtkPoints>::New();
  m_kneeDownWidget->GetPoints(kneeDownPoints);
  kneeDownPoints->GetPoint(0, kneeDownp1);
  kneeDownPoints->GetPoint(2, kneeDownp2);
  double kneeDownReferencePoint[3];
  for (int i = 0; i < 3; i++) {
    kneeDownDirection[i] = kneeDownp1[i] - kneeDownp2[i];
    kneeDownReferencePoint[i] = femurCenter[i] + kneeDownDirection[i];
  }
  m_femurKneeDownLine->SetPoint1and2(kneeDownReferencePoint, femurCenter);

  // angle 1
  this->UpDateVectorActor(m_TextActorFemurKneeDown, m_GuideActorFemurKneeDown,
                          kneeDownReferencePoint, femurCenter, kneeDownCenter,
                          femurCenter);
  // angle 3
  double kneeDownPoint2[3];
  m_kneeDownWidget->GetPoint2(kneeDownPoint2);
  this->UpDateVectorActor(m_TextActorAnkleKneeDown, m_GuideActorAnkleKneeDown,
                          kneeDownPoint2, kneeDownCenter, ankleCenter,
                          kneeDownCenter);
  // angle 4
  double anklePoint1[3];
  m_ankleWidget->GetPoint1(anklePoint1);
  this->UpDateVectorActor(m_TextActorAnkleAnkle, m_GuideActorAnkleAnkle,
                          anklePoint1, ankleCenter, kneeDownCenter,
                          ankleCenter);
  // angle 5
  double kneeUpPoint1[3];
  m_kneeUpWidget->GetPoint1(kneeUpPoint1);
  double kneeUpCenter[3];
  m_kneeUpWidget->GetOrigion(kneeUpCenter);
  this->UpDateVectorActor(m_TextActorKneeUpDown, m_GuideActorKneeUpDown,
                          kneeUpPoint1, kneeUpCenter, kneeDownp1,
                          kneeDownCenter);
}

void MainWindow::OnChangeAnkleWidget() {
  double kneeDownCenter[3], ankleCenter[3], femurCenter[3];
  m_kneeDownWidget->GetOrigion(kneeDownCenter);
  m_ankleWidget->GetOrigion(ankleCenter);
  m_femurWidget->GetOrigion(femurCenter);

  m_kneedown2ankleLine->SetPoint1and2(kneeDownCenter, ankleCenter);
  m_femur2ankleLine->SetPoint1and2(femurCenter, ankleCenter);

  // angle 3
  double kneeDownPoint2[3];
  m_kneeDownWidget->GetPoint2(kneeDownPoint2);
  this->UpDateVectorActor(m_TextActorAnkleKneeDown, m_GuideActorAnkleKneeDown,
                          kneeDownPoint2, kneeDownCenter, ankleCenter,
                          kneeDownCenter);
  // angle 4
  double anklePoint1[3];
  m_ankleWidget->GetPoint1(anklePoint1);
  this->UpDateVectorActor(m_TextActorAnkleAnkle, m_GuideActorAnkleAnkle,
                          anklePoint1, ankleCenter, kneeDownCenter,
                          ankleCenter);
}
