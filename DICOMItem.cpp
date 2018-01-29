#include "DICOMItem.h"

DICOMItem::DICOMItem(QObject *parent, std::vector<std::string> path)
    : QObject(parent) {

  m_path = path;
  m_imgData = vtkSmartPointer<vtkImageData>::New();

  typedef itk::Image<short, 3> ImageShortType;
  auto gdcmIO = itk::GDCMImageIO::New();
  auto reader = itk::ImageSeriesReader<ImageShortType>::New();
  reader->SetImageIO(gdcmIO);
  reader->SetFileNames(m_path);
  try {
    reader->Update();
  } catch (itk::ExceptionObject &e) {
    QMessageBox::critical(0, tr("Error"), QString(e.GetDescription()));
  }
  auto tovtk = itk::ImageToVTKImageFilter<ImageShortType>::New();
  tovtk->SetInput(reader->GetOutput());
  tovtk->Update();

  m_imgData->DeepCopy(tovtk->GetOutput());
  this->InsertTagVolume();
  this->GetImageTagValue(gdcmIO);
}

vtkSmartPointer<vtkImageData> DICOMItem::GetImageData() { return m_imgData; }

void DICOMItem::GetImageData(vtkSmartPointer<vtkImageData> img) {
  qDebug() << "dicom item" << m_imgData->GetDimensions()[2];
  img->DeepCopy(m_imgData);
}

QString DICOMItem::GetTagInformation() { return m_tagvaluelist; }

void DICOMItem::GetTableTags(QString tag[]) {
  for (int i = 0; i < 7; i++) {
    tag[i] = m_tabletags[i];
  }
}

void DICOMItem::GetImageTagValue(itk::GDCMImageIO::Pointer gdcmio) {

  QList<QMap<QString, QString>> tagMaps = {
      TagMapVolume, TagMapImaging, TagMapPatient, TagMapStudy, TagMapSeries};

  QString title[5] = {tr("- Volume  -"), tr("- Imaging -"), tr("- Patient -"),
                      tr("- Study   -"), tr("- Series  -")};

  for (int i = 0; i < 5; ++i) {
    m_tagvaluelist += title[i] + "\n";
    foreach (auto key, tagMaps[i].keys()) {
      std::string value;
      gdcmio->GetValueFromTag(tagMaps[i].value(key).toStdString(), value);
      m_tagvaluelist +=
          key + ": " + QString::fromLocal8Bit(value.c_str()) + "\n";
    }
    if (tagMaps[i] != tagMaps.last())
      m_tagvaluelist += "\n";
  }

  std::string value;
  m_tabletags[0] = QString::number(m_path.size());
  gdcmio->GetValueFromTag(std::string("0018|0050"), value);
  m_tabletags[1] = QString::fromStdString(value);
  gdcmio->GetValueFromTag(std::string("0018|0088"), value);
  m_tabletags[2] = QString::fromStdString(value);
  gdcmio->GetValueFromTag(std::string("0008|0060"), value);
  m_tabletags[3] = QString::fromStdString(value);
  gdcmio->GetValueFromTag(std::string("0010|0020"), value);
  m_tabletags[4] = QString::fromStdString(value);
  gdcmio->GetValueFromTag(std::string("0008|1030"), value);
  m_tabletags[5] = QString::fromStdString(value);
  gdcmio->GetValueFromTag(std::string("0008|103e"), value);
  m_tabletags[6] = QString::fromStdString(value);
}

void DICOMItem::InsertTagVolume() {

  TagMapVolume.clear();
  TagMapImaging.clear();
  TagMapPatient.clear();
  TagMapStudy.clear();
  TagMapSeries.clear();

  TagMapVolume.insert(tr("01. Slice Thickness"), "0018|0050");
  TagMapVolume.insert(tr("02. Pixel Spacing"), "0028|0030");
  TagMapVolume.insert(tr("03. Rows"), "0028|0010");
  TagMapVolume.insert(tr("04. Columns"), "0028|0011");

  TagMapImaging.insert(tr("01. Image Type"), "0008|0008");
  TagMapImaging.insert(tr("02. Modality"), "0008|0060");
  TagMapImaging.insert(tr("03. Contrast/Bolus Agent"), "0018|0010");
  TagMapImaging.insert(tr("04. Repetition Time"), "0018|0080");
  TagMapImaging.insert(tr("05. Echo Time"), "0018|0081");
  TagMapImaging.insert(tr("06. Echo Number(s)"), "0018|0086");
  TagMapImaging.insert(tr("07. Scanning Sequence"), "0018|0020");
  TagMapImaging.insert(tr("08. Manufacturer"), "0008|0070");
  TagMapImaging.insert(tr("09. Institution Name"), "0008|0080");
  TagMapImaging.insert(tr("10. Institution Address"), "0008|0081");

  TagMapPatient.insert(tr("01. Patient's Name"), "0010|0010");
  TagMapPatient.insert(tr("02. Patient's Sex"), "0010|0040");
  TagMapPatient.insert(tr("03. Patient's Age"), "0010|1010");
  TagMapPatient.insert(tr("04. Patient's Weight"), "0010|1030");
  TagMapPatient.insert(tr("05. Patient's Birth Date"), "0010|0030");
  TagMapPatient.insert(tr("06. Patient's ID"), "0010|0020");
  TagMapPatient.insert(tr("07. Patient's Address"), "0010|1040");

  TagMapStudy.insert(tr("01. Study Description"), "0008|1030");
  TagMapStudy.insert(tr("02. Study Date"), "0008|0020");
  TagMapStudy.insert(tr("03. Study Time"), "0008|0030");
  TagMapStudy.insert(tr("04. Study ID"), "0020|0010");

  TagMapSeries.insert(tr("01. Series Description"), "0008|103e");
  TagMapSeries.insert(tr("02. Series Date"), "0008|0021");
  TagMapSeries.insert(tr("03. Series Time"), "0008|0031");
  TagMapSeries.insert(tr("04. Series Number"), "0020|0011");
}
