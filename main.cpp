#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   QCoreApplication::setOrganizationName("AGCO");
   QCoreApplication::setApplicationName("Texte editor");

   MainWindow mainWin;
   mainWin.show();

   return app.exec();
}
