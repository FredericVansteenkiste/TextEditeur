#include "mainwindow.h"

MainWindow::MainWindow():m_pqTextEdit(new QPlainTextEdit),
                         m_qstrCurFile()
{
   setCentralWidget(m_pqTextEdit);

   createActions();
   createStatusBar();

   readGeometry();

   connect(m_pqTextEdit->document(), &QTextDocument::contentsChanged,
           this,                     &MainWindow::documentWasModified);

   setCurrentFile(QString());
   setUnifiedTitleAndToolBarOnMac(true);

   // J'applique une police courier pour l'éditeur
   QTextCharFormat qTextFormat = m_pqTextEdit->currentCharFormat();
   QFont qFont("Courier New", 10);
   QFontMetrics qFontMetrics(qFont);
   qTextFormat.setFont(qFont);
   m_pqTextEdit->setCurrentCharFormat(qTextFormat);
   m_pqTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
   m_pqTextEdit->setTabStopDistance(3 * qFontMetrics.horizontalAdvance(' '));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
   if(maybeSave() == true)
   {
      writeGeometry();
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
      QDir qWorkingDirectory = ReadWorkingDirectory();
      QString fileName = QFileDialog::getOpenFileName(
                                             this,
                                             QString(),
                                             qWorkingDirectory.absolutePath());
      if(fileName.isEmpty() == false)
      {
         loadFile(fileName);
         WriteWorkingDirectory(QDir(fileName));
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

void MainWindow::ExtractEnum(void)
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
   qstrText.replace("Driver Only",   "Driver_Only",   Qt::CaseInsensitive);
   qstrText.replace("Driver and Windows",
                    "Driver_and_Windows",
                    Qt::CaseInsensitive);
   qstrText.replace("Windows Only",  "Windows_Only",  Qt::CaseInsensitive);
   qstrText.replace("/",             "_",  Qt::CaseInsensitive);
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

void MainWindow::ArrangeDirectory(void)
{
   QString qstrFiles(m_pqTextEdit->toPlainText());
   QStringList qlstrLines = qstrFiles.split("\n");
   QFileInfo qFileInfo(m_qstrCurFile);
   QDir qSourceDirectory(qFileInfo.absolutePath());
   if(qSourceDirectory.cd("generated") == false)
   {
      QMessageBox::critical(this,
                            tr("Error"),
                            tr("The sub-directory \"generated\" "
                               "was not found"));
      return;
   }
   if(qSourceDirectory.cd("Converted.png") == false)
   {
      QMessageBox::critical(this,
                            tr("Error"),
                            tr("The sub-directory \"generated\\Converted.png\" "
                               "was not found"));
      return;
   }
   if(qSourceDirectory.cd("pictures") == false)
   {
      QMessageBox::critical(this,
                            tr("Error"),
                            tr("The sub-directory \"generated\\Converted.png\\"
                               "pictures\" was not found"));
      return;
   }

   for(int i = 0; i < qlstrLines.size(); i++)
   {
      int iIndDeb = qlstrLines[i].indexOf("<Image ");
      int iIndFin = qlstrLines[i].indexOf("/>");
      if(  (iIndDeb == -1)
         ||(iIndFin == -1)
         ||((iIndFin - iIndDeb) < 0))
      {
         continue;
      }

      QString qstrLine(qlstrLines[i].mid(iIndDeb + 7, iIndFin - iIndDeb - 7));
      iIndDeb = qstrLine.indexOf("paletteID");
      iIndFin = qstrLine.indexOf("\"", iIndDeb + 11);
      QString qstrPaletteID(qstrLine.mid(iIndDeb + 11, iIndFin - iIndDeb - 11));
      if(qstrPaletteID == "65535")
      {
         continue;
      }
      iIndDeb = qstrLine.indexOf("colorFormat");
      iIndFin = qstrLine.indexOf("\"", iIndDeb + 13);
      QString qstrColorFormat(qstrLine.mid(iIndDeb + 13,
                                           iIndFin - iIndDeb - 13));
      iIndDeb = qstrLine.indexOf("name");
      iIndFin = qstrLine.indexOf("\"", iIndDeb + 6);
      QString qstrNameFile(qstrLine.mid(iIndDeb + 6,
                                        iIndFin - iIndDeb - 6));
      qSourceDirectory.mkdir(qstrPaletteID + "_" + qstrColorFormat);
      QDir qDestDirectory(qSourceDirectory);
      qDestDirectory.cd(qstrPaletteID + "_" + qstrColorFormat);
      QFile qFile(  qSourceDirectory.absolutePath()
                  + "/"
                  + qstrNameFile
                  + ".png");
      qFile.rename(  qDestDirectory.absolutePath()
                     + "/"
                     + qstrNameFile
                     + ".png");
   }
}

void MainWindow::ExtractDataFromAsc(void)
{
   QString qstrInputFileName;

   if(maybeSave() == true)
   {
      QDir qWorkingDirectory = ReadWorkingDirectory();
      qstrInputFileName = QFileDialog::getOpenFileName(this,
                                              QString(),
                                              qWorkingDirectory.absolutePath());
      if(qstrInputFileName.isEmpty())
      {
         return;
      }
      else
      {
         loadFile(qstrInputFileName);
         WriteWorkingDirectory(QDir(qstrInputFileName));
      }
   }

   QFile file(qstrInputFileName);
   if(file.open(QFile::ReadOnly | QFile::Text) == false)
   {
      QMessageBox::warning(this,
                           tr("Application"),
                           tr("Cannot read file %1:\n%2.")
                              .arg(QDir::toNativeSeparators(qstrInputFileName),
                                   file.errorString()));
      return;
   }

   QString qstrOutputFileName(qstrInputFileName);
   qstrOutputFileName.replace(".asc", ".xlsx");
   QFile::remove(qstrOutputFileName);
   QXlsx::Document xlsx(qstrOutputFileName);
   xlsx.addSheet("FiltreGaugePasseBas");
   quint16 qu16IndiceInPB(1);
   xlsx.write(qu16IndiceInPB, 2, "Input Signal");
   xlsx.write(qu16IndiceInPB, 3, "Filtre passe bas");
   xlsx.write(qu16IndiceInPB, 4, "Déduction du niveau");
   xlsx.write(qu16IndiceInPB, 5, "Traitement des variations");
   xlsx.write(qu16IndiceInPB, 6, "Convertion en %");
   qu16IndiceInPB++;
   xlsx.addSheet("FiltreGaugeValMin_10s");
   quint16 qu16IndiceInMin10s(1);
   xlsx.write(qu16IndiceInMin10s, 2, "Input Signal");
   xlsx.write(qu16IndiceInMin10s, 3, "Filtre min value sur 10s");
   xlsx.write(qu16IndiceInMin10s, 4, "Déduction du niveau");
   xlsx.write(qu16IndiceInMin10s, 5, "Traitement des variations");
   xlsx.write(qu16IndiceInMin10s, 6, "Convertion en %");
   qu16IndiceInMin10s++;
   xlsx.addSheet("FiltreGaugeValMin_60s");
   quint16 qu16IndiceInMin60s(1);
   xlsx.write(qu16IndiceInMin60s, 2, "Input Signal");
   xlsx.write(qu16IndiceInMin60s, 3, "Filtre min value sur 60s");
   xlsx.write(qu16IndiceInMin60s, 4, "Déduction du niveau");
   xlsx.write(qu16IndiceInMin60s, 5, "Traitement des variations");
   xlsx.write(qu16IndiceInMin60s, 6, "Convertion en %");
   qu16IndiceInMin60s++;
   xlsx.addSheet("FiltreGaugePasseBasValMin10");
   quint16 qu16IndiceInMinPB_10s(1);
   xlsx.write(qu16IndiceInMinPB_10s, 2, "Input Signal");
   xlsx.write(qu16IndiceInMinPB_10s, 3, "Filtre min value sur 10s");
   xlsx.write(qu16IndiceInMinPB_10s, 4, "Filtre pass bas sur min value");
   xlsx.write(qu16IndiceInMinPB_10s, 5, "Déduction du niveau");
   xlsx.write(qu16IndiceInMinPB_10s, 6, "Traitement des variations");
   xlsx.write(qu16IndiceInMinPB_10s, 7, "Convertion en %");
   qu16IndiceInMinPB_10s++;

   QTextStream in(&file);
   QString qstrLine;
   bool bIsOK;
   while(in.readLineInto(&qstrLine))
   {
      while(qstrLine.contains("  "))
      {
         qstrLine.replace("  ", " ");
      }

      QStringList qlstrLineElement = qstrLine.split(" ");
      if(  (qlstrLineElement.size() > 3)
         &&(qlstrLineElement[3] == "100"))
      {
         QString qstrInputValue(qlstrLineElement[7]);
         qstrInputValue = qstrInputValue + qlstrLineElement[8];
         uint uiInputValue = qstrInputValue.toUInt(&bIsOK, 16);
         qstrInputValue = qlstrLineElement[1];
         float fTime = qstrInputValue.toFloat(&bIsOK);
         qstrInputValue = qlstrLineElement[9] + qlstrLineElement[10];
         uint uiValueFiltered = qstrInputValue.toUInt(&bIsOK, 16);
         uint uiDeducedLevel = qlstrLineElement[11].toUInt(&bIsOK, 16);
         uint uiVariationTreatment = qlstrLineElement[12].toUInt(&bIsOK, 16);
         uint uiLevelPercent = qlstrLineElement[13].toUInt(&bIsOK, 16);

         xlsx.selectSheet("FiltreGaugePasseBas");
         xlsx.write(qu16IndiceInPB, 1, fTime);
         xlsx.write(qu16IndiceInPB, 2, uiInputValue);
         xlsx.write(qu16IndiceInPB, 3, uiValueFiltered);
         xlsx.write(qu16IndiceInPB, 4, uiDeducedLevel);
         xlsx.write(qu16IndiceInPB, 5, uiVariationTreatment);
         xlsx.write(qu16IndiceInPB, 6, uiLevelPercent);
         xlsx.write(qu16IndiceInPB, 7, qlstrLineElement[14]);

         xlsx.selectSheet("FiltreGaugeValMin_10s");
         xlsx.write(qu16IndiceInPB, 2, uiInputValue);

         xlsx.selectSheet("FiltreGaugeValMin_60s");
         xlsx.write(qu16IndiceInPB, 2, uiInputValue);

         xlsx.selectSheet("FiltreGaugePasseBasValMin10");
         xlsx.write(qu16IndiceInPB, 2, uiInputValue);

         qu16IndiceInPB++;
      }
      else if(  (qlstrLineElement.size() > 3)
              &&(qlstrLineElement[3] == "101"))
      {
         QString qstrInputValue(qlstrLineElement[7]);
         qstrInputValue = qstrInputValue + qlstrLineElement[8];
         uint uiValMin10s = qstrInputValue.toUInt(&bIsOK, 16);
         qstrInputValue = qlstrLineElement[1];
         float fTime = qstrInputValue.toFloat(&bIsOK);
         uint uiDeducedLevel = qlstrLineElement[9].toUInt(&bIsOK, 16);
         uint uiVariationTreatment = qlstrLineElement[10].toUInt(&bIsOK, 16);
         uint uiLevelPercent = qlstrLineElement[11].toUInt(&bIsOK, 16);

         xlsx.selectSheet("FiltreGaugeValMin_10s");
         xlsx.write(qu16IndiceInMin10s, 1, fTime);
         xlsx.write(qu16IndiceInMin10s, 3, uiValMin10s);
         xlsx.write(qu16IndiceInMin10s, 4, uiDeducedLevel);
         xlsx.write(qu16IndiceInMin10s, 5, uiVariationTreatment);
         xlsx.write(qu16IndiceInMin10s, 6, uiLevelPercent);
         xlsx.write(qu16IndiceInMin10s, 7, qlstrLineElement[14]);

         xlsx.selectSheet("FiltreGaugePasseBasValMin10");
         xlsx.write(qu16IndiceInMin10s, 3, uiValMin10s);

         qu16IndiceInMin10s++;
      }
      else if(  (qlstrLineElement.size() > 3)
              &&(qlstrLineElement[3] == "102"))
      {
         QString qstrInputValue(qlstrLineElement[7]);
         qstrInputValue = qstrInputValue + qlstrLineElement[8];
         uint uiValMin60s = qstrInputValue.toUInt(&bIsOK, 16);
         qstrInputValue = qlstrLineElement[1];
         float fTime = qstrInputValue.toFloat(&bIsOK);
         uint uiDeducedLevel = qlstrLineElement[9].toUInt(&bIsOK, 16);
         uint uiVariationTreatment = qlstrLineElement[10].toUInt(&bIsOK, 16);
         uint uiLevelPercent = qlstrLineElement[11].toUInt(&bIsOK, 16);

         xlsx.selectSheet("FiltreGaugeValMin_60s");
         xlsx.write(qu16IndiceInMin60s, 1, fTime);
         xlsx.write(qu16IndiceInMin60s, 3, uiValMin60s);
         xlsx.write(qu16IndiceInMin60s, 4, uiDeducedLevel);
         xlsx.write(qu16IndiceInMin60s, 5, uiVariationTreatment);
         xlsx.write(qu16IndiceInMin60s, 6, uiLevelPercent);
         xlsx.write(qu16IndiceInMin60s, 7, qlstrLineElement[14]);

         qu16IndiceInMin60s++;
      }
      else if(  (qlstrLineElement.size() > 3)
              &&(qlstrLineElement[3] == "103"))
      {
         QString qstrInputValue(qlstrLineElement[8]);
         qstrInputValue = qstrInputValue + qlstrLineElement[9];
         uint uiValueFiltered_10s = qstrInputValue.toUInt(&bIsOK, 16);
         qstrInputValue = qlstrLineElement[1];
         float fTime = qstrInputValue.toFloat(&bIsOK);
         uint uiDeducedLevel = qlstrLineElement[10].toUInt(&bIsOK, 16);
         uint uiVariationTreatment = qlstrLineElement[11].toUInt(&bIsOK, 16);
         uint uiLevelPercent = qlstrLineElement[12].toUInt(&bIsOK, 16);

         xlsx.selectSheet("FiltreGaugePasseBasValMin10");
         xlsx.write(qu16IndiceInMinPB_10s, 1, fTime);
         xlsx.write(qu16IndiceInMinPB_10s, 4, uiValueFiltered_10s);
         xlsx.write(qu16IndiceInMinPB_10s, 5, uiDeducedLevel);
         xlsx.write(qu16IndiceInMinPB_10s, 6, uiVariationTreatment);
         xlsx.write(qu16IndiceInMinPB_10s, 7, uiLevelPercent);
         xlsx.write(qu16IndiceInMinPB_10s, 8, qlstrLineElement[14]);

         qu16IndiceInMinPB_10s++;
      }
   }

   xlsx.selectSheet("FiltreGaugePasseBas");
   if(xlsx.save() == false)
   {
      QMessageBox msgBox;
      msgBox.setText("Erreur lors de la sauvegarde du fichier Excel");
      msgBox.exec();
   }
}

void MainWindow::createActions(void)
{
   //***************************************************************************
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

   //***************************************************************************
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

   //***************************************************************************
   // Création du menu Macro
   QMenu* macroMenu = menuBar()->addMenu(tr("&Macro"));
   QToolBar* macroToolBar = addToolBar(tr("Macro"));

   QAction* pqExtractEnum = new QAction(QIcon(":/images/ToEnumerate.png"),
                                        tr("Enum extraction"),
                                        this);
   pqExtractEnum->setStatusTip(tr("Convert the enum from a dbc file into a "
                                "structured text code"));
   macroMenu->addAction(pqExtractEnum);
   macroToolBar->addAction(pqExtractEnum);
   connect(pqExtractEnum, &QAction::triggered, this, &MainWindow::ExtractEnum);

   QAction* pqArrangeDirectory = new QAction(QIcon(":/images/folder.png"),
                                             tr("Rearrange directory"),
                                             this);
   pqArrangeDirectory->setStatusTip(tr("Organize the directory of the picture "
                                       "in function of the information in "
                                       "xml"));
   macroMenu->addAction(pqArrangeDirectory);
   macroToolBar->addAction(pqArrangeDirectory);
   connect(pqArrangeDirectory, &QAction::triggered,
           this, &MainWindow::ArrangeDirectory);

   QAction* pqConvertAsc2xlsx = new QAction(QIcon(":/images/MacroAsc2xlsx.png"),
                                             tr("Extract asc data"),
                                             this);
   pqConvertAsc2xlsx->setStatusTip(tr("Extract data from asc and put it in an "
                                       "Excel table"));
   macroMenu->addAction(pqConvertAsc2xlsx);
   macroToolBar->addAction(pqConvertAsc2xlsx);
   connect(pqConvertAsc2xlsx, &QAction::triggered,
           this, &MainWindow::ExtractDataFromAsc);
}

void MainWindow::createStatusBar(void)
{
   statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readGeometry(void)
{
   QSettings qSettings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
   const QByteArray qGeometry = qSettings.value("geometry",
                                                QByteArray()).toByteArray();
   if(qGeometry.isEmpty() == true)
   {
      const QRect qAvailableGeometry =
                           QApplication::desktop()->availableGeometry(this);
      resize(qAvailableGeometry.width() / 3, qAvailableGeometry.height() / 2);
      move((qAvailableGeometry.width() - width()) / 2,
           (qAvailableGeometry.height() - height()) / 2);
   }
   else
   {
      restoreGeometry(qGeometry);
   }
}

void MainWindow::writeGeometry(void)
{
    QSettings qSettings(QCoreApplication::organizationName(),
                        QCoreApplication::applicationName());
    qSettings.setValue("geometry", saveGeometry());
}

QDir MainWindow::ReadWorkingDirectory(void)
{
   QSettings qSettings(QCoreApplication::organizationName(),
                      QCoreApplication::applicationName());
   QString qstrDir = qSettings.value("WorkingDirectory").toString();

   return QDir(qstrDir);
}

void MainWindow::WriteWorkingDirectory(const QDir& qWorkingDirectory)
{
   QSettings qSettings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
   qSettings.setValue("WorkingDirectory", qWorkingDirectory.absolutePath());
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
   WriteWorkingDirectory(QDir(fileName));

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
