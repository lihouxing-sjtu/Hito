#ifndef DICOMBROWSER_H
#define DICOMBROWSER_H
#include "DICOMItem.h"
#include "stdafx.h"
#include <QWidget>
namespace Ui {
class DICOMBrowser;
}

class DICOMBrowser : public QWidget {
  Q_OBJECT

public:
  explicit DICOMBrowser(QWidget *parent = 0);
  ~DICOMBrowser();
  vtkSmartPointer<vtkImageData> GetItemImageData(int index);

private:
  Ui::DICOMBrowser *ui;
  QList<std::vector<std::string>> FilesList;
  QList<std::vector<std::string>> AddFilesList;
  QList<DICOMItem *> m_dicomList;
signals:
  void importVolume(int index);
  void importXRay(int index);
protected slots:
  void OnDICOMBrowseButton();
  void OnSetImageInformation();
  void OnImportVolume();
  void OnImportXRay();
};

#endif // DICOMBROWSER_H
