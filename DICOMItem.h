#ifndef DICOMITEM_H
#define DICOMITEM_H
#include "stdafx.h"
#include <QObject>

class DICOMItem : public QObject {
  Q_OBJECT
public:
  DICOMItem(QObject *parent = nullptr, std::vector<std::string> path = {""});

  vtkSmartPointer<vtkImageData> GetImageData();
  void GetImageData(vtkSmartPointer<vtkImageData> img);
  QString GetTagInformation();
  void GetTableTags(QString tag[]);

protected:
  std::vector<std::string> m_path;
  vtkSmartPointer<vtkImageData> m_imgData;
  QString m_tagvaluelist;
  QString m_tabletags[7];
  QMap<QString, QString> TagMapVolume;
  QMap<QString, QString> TagMapImaging;
  QMap<QString, QString> TagMapPatient;
  QMap<QString, QString> TagMapStudy;
  QMap<QString, QString> TagMapSeries;

private:
  void GetImageTagValue(itk::GDCMImageIO::Pointer gdcmio);
  void InsertTagVolume();

public slots:
};

#endif // DICOMITEM_H
