#include "mainwindow.h"

MainWindow::MainWindow():m_pqTextEdit(new QPlainTextEdit),
                         m_qstrCurFile()
{
   setCentralWidget(m_pqTextEdit);

   createActions();
   createStatusBar();

   readSettings();

   connect(m_pqTextEdit->document(), &QTextDocument::contentsChanged,
           this,                     &MainWindow::documentWasModified);

   setCurrentFile(QString());
   setUnifiedTitleAndToolBarOnMac(true);

   // J'applique une police courier pour l'éditeur
   QTextCharFormat qTextFormat = m_pqTextEdit->currentCharFormat();
   qTextFormat.setFont(QFont("Courier New", 10));
   m_pqTextEdit->setCurrentCharFormat(qTextFormat);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   if(maybeSave() == true)
   {
      writeSettings();
      event->accept();
   }
   else
   {
      event->ignore();
   }
}

void MainWindow::newFile(void)
{
   if(maybeSave() == true)
   {
      m_pqTextEdit->clear();
      setCurrentFile(QString());
   }
}

void MainWindow::open(void)
{
   if(maybeSave() == true)
   {
      QString fileName = QFileDialog::getOpenFileName(this);
      if(fileName.isEmpty() == false)
      {
         loadFile(fileName);
      }
   }
}

bool MainWindow::save(void)
{
   if(m_qstrCurFile.isEmpty() == true)
   {
      return saveAs();
   }
   else
   {
      return saveFile(m_qstrCurFile);
   }
}

bool MainWindow::saveAs(void)
{
   QFileDialog dialog(this);
   dialog.setWindowModality(Qt::WindowModal);
   dialog.setAcceptMode(QFileDialog::AcceptSave);
   if(dialog.exec() != QDialog::Accepted)
   {
      return false;
   }

   return saveFile(dialog.selectedFiles().first());
}

void MainWindow::documentWasModified(void)
{
    setWindowModified(m_pqTextEdit->document()->isModified());
}

void MainWindow::extractEnum(void)
{
   // Je récupère les données et je les mets en forme
   QString qstrText = m_pqTextEdit->toPlainText();
   qstrText.replace("\"", "");
   qstrText.replace("\n", "");
   while(qstrText.contains("  ") == true)
   {
      qstrText.replace("  ", " ");
   }
   qstrText.replace("Not used",      "Not_used",      Qt::CaseInsensitive);
   qstrText.replace("Not available", "Not_available", Qt::CaseInsensitive);
   QStringList qlstrText = qstrText.split(" ");

   // Je range ces données dans un QMap
   QMap<int, QString> qMapData;
   int iMaxNameSize = 0;
   while(true)
   {
      if(qlstrText.isEmpty() == true)
      {
         break;
      }
      QString qstrValue = qlstrText.takeFirst();

      if(qlstrText.isEmpty() == true)
      {
         break;
      }
      QString qstrName = qlstrText.takeFirst();
      qstrName.replace("+", "Plus");
      qstrName.replace("-", "Minus");
      if(qstrName.size() > iMaxNameSize)
      {
         iMaxNameSize = qstrName.size();
      }

      qMapData.insert(qstrValue.toInt(), qstrName);
   }

   // J'écris le nouveau code
   qstrText = "";
   while(true)
   {
      if(qMapData.isEmpty() == true)
      {
         break;
      }
      int     iValue   = qMapData.firstKey();
      QString qstrName = qMapData.first();
      qMapData.remove(iValue);

      QString qstrLine("       ");
      qstrLine += qstrName;
      qstrLine.insert(iMaxNameSize + 8, ":= 16#");
      qstrLine += QString("%1").arg(iValue, 2, 16, QLatin1Char('0')).toUpper();
      if(qMapData.isEmpty() == true)
      {
         qstrLine += "\n";
      }
      else
      {
         qstrLine += ",\n";
      }
      qstrText += qstrLine;
   }

   // Et on affiche le résultat dans l'éditeur
   m_pqTextEdit->setPlainText(qstrText);
   m_pqTextEdit->document()->setModified(true);
}

void MainWindow::createActions(void)
{
   // Création du menu File
   QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
   QToolBar* fileToolBar = addToolBar(tr("File"));
   const QIcon newIcon = QIcon::fromTheme("document-new",
                                          QIcon(":/images/new.png"));
   QAction* newAct = new QAction(newIcon, tr("&New"), this);
   newAct->setShortcuts(QKeySequence::New);
   newAct->setStatusTip(tr("Create a new file"));
   connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
   fileMenu->addAction(newAct);
   fileToolBar->addAction(newAct);

   const QIcon openIcon = QIcon::fromTheme("document-open",
                                           QIcon(":/images/open.png"));
   QAction* openAct = new QAction(openIcon, tr("&Open..."), this);
   openAct->setShortcuts(QKeySequence::Open);
   openAct->setStatusTip(tr("Open an existing file"));
   connect(openAct, &QAction::triggered, this, &MainWindow::open);
   fileMenu->addAction(openAct);
   fileToolBar->addAction(openAct);

   const QIcon saveIcon = QIcon::fromTheme("document-save",
                                           QIcon(":/images/save.png"));
   QAction* saveAct = new QAction(saveIcon, tr("&Save"), this);
   saveAct->setShortcuts(QKeySequence::Save);
   saveAct->setStatusTip(tr("Save the document to disk"));
   connect(saveAct, &QAction::triggered, this, &MainWindow::save);
   fileMenu->addAction(saveAct);
   fileToolBar->addAction(saveAct);

   const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
   QAction* saveAsAct = fileMenu->addAction(saveAsIcon,
                                            tr("Save &As..."),
                                            this,
                                            &MainWindow::saveAs);
   saveAsAct->setShortcuts(QKeySequence::SaveAs);
   saveAsAct->setStatusTip(tr("Save the document under a new name"));

   fileMenu->addSeparator();

   const QIcon exitIcon = QIcon::fromTheme("application-exit");
   QAction* exitAct = fileMenu->addAction(exitIcon,
                                          tr("E&xit"),
                                          this,
                                          &QWidget::close);
   exitAct->setShortcuts(QKeySequence::Quit);
   exitAct->setStatusTip(tr("Exit the application"));

   // Création du menu édit
   QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
   QToolBar* editToolBar = addToolBar(tr("Edit"));

   const QIcon cutIcon = QIcon::fromTheme("edit-cut",
                                          QIcon(":/images/cut.png"));
   QAction* cutAct = new QAction(cutIcon, tr("Cu&t"), this);
   cutAct->setShortcuts(QKeySequence::Cut);
   cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                           "clipboard"));
   connect(cutAct, &QAction::triggered, m_pqTextEdit, &QPlainTextEdit::cut);
   editMenu->addAction(cutAct);
   editToolBar->addAction(cutAct);

   const QIcon copyIcon = QIcon::fromTheme("edit-copy",
                                           QIcon(":/images/copy.png"));
   QAction* copyAct = new QAction(copyIcon, tr("&Copy"), this);
   copyAct->setShortcuts(QKeySequence::Copy);
   copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
   connect(copyAct, &QAction::triggered, m_pqTextEdit, &QPlainTextEdit::copy);
   editMenu->addAction(copyAct);
   editToolBar->addAction(copyAct);

   const QIcon pasteIcon = QIcon::fromTheme("edit-paste",
                                            QIcon(":/images/paste.png"));
   QAction* pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
   pasteAct->setShortcuts(QKeySequence::Paste);
   pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
   connect(pasteAct, &QAction::triggered, m_pqTextEdit, &QPlainTextEdit::paste);
   editMenu->addAction(pasteAct);
   editToolBar->addAction(pasteAct);

   cutAct->setEnabled(false);
   copyAct->setEnabled(false);
   connect(m_pqTextEdit, &QPlainTextEdit::copyAvailable,
           cutAct,       &QAction::setEnabled);
   connect(m_pqTextEdit, &QPlainTextEdit::copyAvailable,
           copyAct,      &QAction::setEnabled);

   // Création du menu Macro
   QMenu* macroMenu = menuBar()->addMenu(tr("&Macro"));
   QAction* extractEnum = new QAction(tr("Enum extraction"), this);
   extractEnum->setStatusTip(tr("Convert the enum from a dbc file into a "
                                "structured text code"));
   macroMenu->addAction(extractEnum);
   connect(extractEnum, &QAction::triggered, this, &MainWindow::extractEnum);
}

void MainWindow::createStatusBar(void)
{
   statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings(void)
{
   QSettings settings(QCoreApplication::organizationName(),
                      QCoreApplication::applicationName());
   const QByteArray geometry = settings.value("geometry",
                                              QByteArray()).toByteArray();
   if(geometry.isEmpty() == true)
   {
      const QRect availableGeometry =
                           QApplication::desktop()->availableGeometry(this);
      resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
      move((availableGeometry.width() - width()) / 2,
           (availableGeometry.height() - height()) / 2);
   }
   else
   {
      restoreGeometry(geometry);
   }
}

void MainWindow::writeSettings(void)
{
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave(void)
{
   if(m_pqTextEdit->document()->isModified() == false)
   {
      return true;
   }
   const QMessageBox::StandardButton ret
               = QMessageBox::warning(this,
                                      tr("Application"),
                                      tr("The document has been modified.\n"
                                         "Do you want to save your changes?"),
                                        QMessageBox::Save
                                      | QMessageBox::Discard
                                      | QMessageBox::Cancel);

   if(ret == QMessageBox::Save)
   {
      return save();
   }
   else if(ret == QMessageBox::Cancel)
   {
      return false;
   }
   else
   {
      return true;
   }
}

void MainWindow::loadFile(const QString& fileName)
{
   QFile file(fileName);
   if(file.open(QFile::ReadOnly | QFile::Text) == false)
   {
      QMessageBox::warning(this,
                           tr("Application"),
                           tr("Cannot read file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName),
                                file.errorString()));
      return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_pqTextEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString& fileName)
{
   QFile file(fileName);
   if(file.open(QFile::WriteOnly | QFile::Text) == false)
   {
      QMessageBox::warning(this,
                           tr("Application"),
                           tr("Cannot write file %1:\n%2.")
                                    .arg(QDir::toNativeSeparators(fileName),
                                         file.errorString()));
      return false;
   }

   QTextStream out(&file);
   QApplication::setOverrideCursor(Qt::WaitCursor);
   out << m_pqTextEdit->toPlainText();
   QApplication::restoreOverrideCursor();

   setCurrentFile(fileName);
   statusBar()->showMessage(tr("File saved"), 2000);

   return true;
}

void MainWindow::setCurrentFile(const QString& fileName)
{
   m_qstrCurFile = fileName;
   m_pqTextEdit->document()->setModified(false);
   setWindowModified(false);

   if(m_qstrCurFile.isEmpty() == true)
   {
      setWindowFilePath("untitled.txt");
   }
   else
   {
      setWindowFilePath(m_qstrCurFile);
   }
}

QString MainWindow::strippedName(const QString& fullFileName)
{
   return QFileInfo(fullFileName).fileName();
}
