#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

// Ce commentaire sert uniquement à vérifier que je parviens à faire un git
// push

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
   void ExtractEnum(void);
   void ArrangeDirectory(void);

private:
   QPlainTextEdit* m_pqTextEdit;
   QString         m_qstrCurFile;

   void createActions(void);
   void createStatusBar(void);
   void readGeometry(void);
   void writeGeometry(void);
   QDir ReadWorkingDirectory(void);
   void WriteWorkingDirectory(const QDir& qWorkingDirectory);
   bool maybeSave(void);
   bool saveFile(const QString& fileName);
   void setCurrentFile(const QString& fileName);
   QString strippedName(const QString& fullFileName);
};

#endif
