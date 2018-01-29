#include "DICOMBrowser.h"
#include "ui_DICOMBrowser.h"

DICOMBrowser::DICOMBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::DICOMBrowser) {
  ui->setupUi(this);

  QString tableheaders[7] = {tr("Slice Number"),
                             tr("Slice Thickness (mm)"),
                             tr("Spacing Between Slices (mm)"),
                             tr("Modality"),
                             tr("Patient's ID"),
                             tr("Study Description"),
                             tr("Series Description")};
  ui->DICOMItemWidget->setColumnCount(7);
  for (int i = 0; i < 7; i++) {
    auto header = new QTableWidgetItem;
    header->setText(tableheaders[i]);
    ui->DICOMItemWidget->setHorizontalHeaderItem(i, header);
  }
  ui->DICOMItemWidget->resizeColumnsToContents();
  ui->DICOMItemWidget->resizeRowsToContents();

  connect(ui->BrowseButton, SIGNAL(clicked(bool)), this,
          SLOT(OnDICOMBrowseButton()));
  connect(ui->DICOMItemWidget, SIGNAL(currentCellChanged(int, int, int, int)),
          this, SLOT(OnSetImageInformation()));
  connect(ui->ImportVolumeButton, SIGNAL(clicked(bool)), this,
          SLOT(OnImportVolume()));
  connect(ui->ImportXRayButton, SIGNAL(clicked(bool)), this,
          SLOT(OnImportXRay()));
}

DICOMBrowser::~DICOMBrowser() { delete ui; }

vtkSmartPointer<vtkImageData> DICOMBrowser::GetItemImageData(int index) {
  qDebug() << "index: " << index;
  auto img = vtkSmartPointer<vtkImageData>::New();
  m_dicomList.at(index)->GetImageData(img);
  qDebug() << " dicom browser" << img->GetDimensions()[2];
  return img;
}

void DICOMBrowser::OnDICOMBrowseButton() {
  QString dirPath =
      QFileDialog::getExistingDirectory(this, tr("Select dir"), 0);
  qDebug() << dirPath;
  if (dirPath.isEmpty())
    return;

  auto nameGenerator = itk::GDCMSeriesFileNames::New();
  nameGenerator->SetUseSeriesDetails(true);
  nameGenerator->SetRecursive(true);
  nameGenerator->SetDirectory(qPrintable(dirPath));
  nameGenerator->Update();

  std::vector<std::string> seriesUID = nameGenerator->GetSeriesUIDs();
  if (seriesUID.empty())
    return;

  for (auto seriesItr = seriesUID.begin(); seriesItr != seriesUID.end();
       ++seriesItr) {
    std::vector<std::string> filenames =
        nameGenerator->GetFileNames(seriesItr->c_str());
    DICOMItem *item = new DICOMItem(this, filenames);
    m_dicomList.append(item);
  }

  ui->DICOMItemWidget->setRowCount(m_dicomList.size());
  ui->DICOMItemWidget->clearContents();
  for (int i = 0; i < m_dicomList.size(); i++) {
    QString tabletags[7];
    m_dicomList.at(i)->GetTableTags(tabletags);
    for (int c = 0; c < 7; ++c) {
      auto *item = new QTableWidgetItem(tabletags[c]);
      item->setTextAlignment(Qt::AlignCenter);
      ui->DICOMItemWidget->setItem(i, c, item);
    }
  }
  ui->DICOMItemWidget->resizeColumnsToContents();
  ui->DICOMItemWidget->resizeRowsToContents();
  qDebug() << m_dicomList.size();
}

void DICOMBrowser::OnSetImageInformation() {
  int currentindex = ui->DICOMItemWidget->currentRow();
  if (currentindex == -1) {
    ui->DICOMInformationEdit->setText("");
    return;
  }
  ui->DICOMInformationEdit->setText(
      m_dicomList.at(currentindex)->GetTagInformation());
}

void DICOMBrowser::OnImportVolume() {
  int currentIndex = ui->DICOMItemWidget->currentRow();
  if (currentIndex == -1)
    return;
  auto img = vtkSmartPointer<vtkImageData>::New();
  m_dicomList.at(currentIndex)->GetImageData(img);
  int dimension[3];
  img->GetDimensions(dimension);
  if (dimension[2] < 2) {
    QMessageBox::information(this, tr("Error"),
                             tr("Selected Item  has only one slices!"));
    return;
  }
  emit importVolume(currentIndex);
  this->close();
}

void DICOMBrowser::OnImportXRay() {
  int currentIndex = ui->DICOMItemWidget->currentRow();
  if (currentIndex == -1)
    return;
  auto img = vtkSmartPointer<vtkImageData>::New();
  m_dicomList.at(currentIndex)->GetImageData(img);
  int dimension[3];
  img->GetDimensions(dimension);
  if (dimension[2] > 1) {
    QMessageBox::information(this, tr("Error"),
                             tr("Selected Item has muilt slices!"));
    return;
  }
  this->close();
  emit importXRay(currentIndex);
}
