#include "MainWindow.h"
#include "stdafx.h"
#include <QApplication>
#include <qsurfaceformat.h>
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
int main(int argc, char *argv[]) {
  QApplication::setStyle(QStyleFactory::create("Fusion"));
  QPalette p;
  p = qApp->palette();
  p.setColor(QPalette::Highlight, QColor(142, 45, 197));
  qApp->setPalette(p);
  QApplication a(argc, argv);
  MainWindow w;
  w.showMaximized();

  return a.exec();
}
