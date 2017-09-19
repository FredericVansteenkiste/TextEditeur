#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow();

   void loadFile(const QString& fileName);

protected:
   void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;

private slots:
   void newFile(void);
   void open(void);
   bool save(void);
   bool saveAs(void);
   void documentWasModified(void);
   void extractEnum(void);

private:
   QPlainTextEdit* m_pqTextEdit;
   QString         m_qstrCurFile;

   void createActions(void);
   void createStatusBar(void);
   void readSettings(void);
   void writeSettings(void);
   bool maybeSave(void);
   bool saveFile(const QString& fileName);
   void setCurrentFile(const QString& fileName);
   QString strippedName(const QString& fullFileName);
};

#endif
