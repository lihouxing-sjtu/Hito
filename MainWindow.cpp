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
  m_Render3D->SetBackground2(1, 1, 1);

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

  for (int i = 0; i < 3; i++) {
    m_FemoralCenter[i] = 0;
    m_KneeCenter[i] = 0;
    m_AnkleCenter[i] = 0;
  }
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
  auto points = vtkSmartPointer<vtkPoints>::New();
  points->Initialize();
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  cells->Initialize();
  auto pd = vtkSmartPointer<vtkPolyData>::New();
  //定义了上面两个点
  if (ui->FemoralCenterButton->isChecked()) {
    if (iskneeDefined) {
      if (isankleDefined) {
        points->InsertNextPoint(m_FemoralMoveCenter);
        points->InsertNextPoint(m_KneeCenter);
        points->InsertNextPoint(m_AnkleCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        idlist->InsertNextId(2);
        cells->InsertNextCell(idlist);
      } else {
        points->InsertNextPoint(m_FemoralMoveCenter);
        points->InsertNextPoint(m_KneeCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        cells->InsertNextCell(idlist);
      }
    }
  } else if (ui->KneeCenterButton->isChecked()) {
    if (isfemoralDefined) {
      if (isankleDefined) {
        points->InsertNextPoint(m_FemoralCenter);
        points->InsertNextPoint(m_KneeMoveCenter);
        points->InsertNextPoint(m_AnkleCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        idlist->InsertNextId(2);
        cells->InsertNextCell(idlist);
      } else {
        points->InsertNextPoint(m_FemoralCenter);
        points->InsertNextPoint(m_KneeMoveCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        cells->InsertNextCell(idlist);
      }
    }
  } else if (ui->AnkleCenterButton->isChecked()) {
    if (iskneeDefined) {
      if (isfemoralDefined) {
        points->InsertNextPoint(m_FemoralCenter);
        points->InsertNextPoint(m_KneeCenter);
        points->InsertNextPoint(m_AnkleMoveCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        idlist->InsertNextId(2);
        cells->InsertNextCell(idlist);
      } else {
        points->InsertNextPoint(m_KneeCenter);
        points->InsertNextPoint(m_AnkleMoveCenter);
        auto idlist = vtkSmartPointer<vtkIdList>::New();
        idlist->Initialize();
        idlist->InsertNextId(0);
        idlist->InsertNextId(1);
        cells->InsertNextCell(idlist);
      }
    }
  } else {
    if (isfemoralDefined && iskneeDefined && isankleDefined) {
      points->InsertNextPoint(m_FemoralCenter);
      points->InsertNextPoint(m_KneeCenter);
      points->InsertNextPoint(m_AnkleCenter);
      auto idlist = vtkSmartPointer<vtkIdList>::New();
      idlist->Initialize();
      idlist->InsertNextId(0);
      idlist->InsertNextId(1);
      idlist->InsertNextId(2);
      cells->InsertNextCell(idlist);
    } else if (isfemoralDefined && iskneeDefined) {
      points->InsertNextPoint(m_FemoralCenter);
      points->InsertNextPoint(m_KneeCenter);
      auto idlist = vtkSmartPointer<vtkIdList>::New();
      idlist->Initialize();
      idlist->InsertNextId(0);
      idlist->InsertNextId(1);
      cells->InsertNextCell(idlist);
    } else if (iskneeDefined && isankleDefined) {
      points->InsertNextPoint(m_KneeCenter);
      points->InsertNextPoint(m_AnkleCenter);
      auto idlist = vtkSmartPointer<vtkIdList>::New();
      idlist->Initialize();
      idlist->InsertNextId(0);
      idlist->InsertNextId(1);
      cells->InsertNextCell(idlist);
    } else
      return;
  }

  pd->SetPoints(points);
  pd->SetLines(cells);
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(pd);
  m_ForceLineActor->SetMapper(mapper);
  m_Render2D->GetRenderWindow()->Render();
}

bool MainWindow::isDataNull(double data[]) {
  if (data[0] == 0 && data[1] == 0 && data[2] == 0)
    return 0;
  else
    return 1;
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
