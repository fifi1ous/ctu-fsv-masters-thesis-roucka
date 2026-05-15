// Implementation of MainWindow.

#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QStandardPaths>
#include <QTextCursor>
#include <QtDebug>

#include "addpath.h"
#include "deleteworkfiles.h"
#include "displayresults.h"


// Construct the main window and load the .ui.
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

// Destructor — frees the .ui object.
MainWindow::~MainWindow()
{
    delete ui;
}

// Display text in the results QTextEdit (replaces any previous content).
void MainWindow::displayText(const QString& text)
{
    ui->textEditResults->setPlainText(text);
}

// Append text to the results QTextEdit (preserves existing content).
void MainWindow::appendText(const QString& text)
{
    QTextCursor cursor = ui->textEditResults->textCursor();
    cursor.movePosition(QTextCursor::End);
    
    if (!ui->textEditResults->toPlainText().isEmpty())
        cursor.insertText(QStringLiteral("\n"));
        
    cursor.insertText(text);
    ui->textEditResults->setTextCursor(cursor);
}

// Open a file dialog and store the chosen path in the ZPMZ controller for type.
// This code was modified by Claude Opus 4.7 //
void MainWindow::loadZPMZFile(ControlZPMZ::FileType type, const QString& dialogTitle, const QString& filter)
{
    QString fileName = QFileDialog::getOpenFileName(this, dialogTitle, "", filter);
    if (!fileName.isEmpty())
    {
        controlZPMZ.setFilePath(type, fileName);
    }
}
// End of code modified by Claude Opus 4.7 //

// Display the file currently held by ControlZPMZ for type in a DisplayZPMZ viewer.
void MainWindow::viewZPMZFile(ControlZPMZ::FileType type)
{
    checkAndDisplayPDF<DisplayZPMZ>(controlZPMZ.getFilePath(type));
}

// Action: load a geometric-plan PDF and store its path in controlGP.
void MainWindow::on_actionNactiGP_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Otevřít soubor GP"), "", tr("PDF dokument (*.pdf)"));

    if (!fileName.isEmpty())
    {
        controlGP.setPath(fileName);
    }
}

// Action: bulk-import dialog (placeholder — selection is not yet wired anywhere).
void MainWindow::on_actionOtevri_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Import souborů"),
        QString(),
        tr("PDF dokument (*.pdf);;ZIP archiv (*.zip);;Výměnný formát katastru (*.vfk);;Textový soubor (*.txt);;Časové razítko (*.tsr);;Podpis (*.p7s);;Všechny soubory (*.*)")
    );
}

// Action: folder-import dialog. Stores selection in member `folder`.
void MainWindow::on_actionImportSlozky_triggered()
{
    folder = QFileDialog::getExistingDirectory(
        this,
        tr("Import složky"),
        QString()
    );
}

// Action: open the loaded GP in a DisplayGP viewer (with bounding-box overlay).
void MainWindow::on_actionNahled_a_Uprava_triggered()
{
    QString path_to_gp = controlGP.getPath();
    checkAndDisplayPDF<DisplayGP>(path_to_gp);
}

// Action: load the Žádost PDF into controlZadost.
void MainWindow::on_actionNacistZadost_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Otevřít soubor Žádost"), "", tr("PDF dokument (*.pdf)"));

    if (!fileName.isEmpty())
    {
        controlZadost.setPath(fileName);
    }
}

// Action: open the loaded Žádost PDF in a DisplayZPMZ viewer.
void MainWindow::on_actionNahled_triggered()
{
    QString path_to_zadost = controlZadost.getPath();
    checkAndDisplayPDF<DisplayZPMZ>(path_to_zadost);
}

// ZPMZ load/view action pairs
void MainWindow::on_actionNacist_triggered()
{
    loadZPMZFile(ControlZPMZ::Popispole, tr("Otevřít soubor Popispole"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_2_triggered()
{
    viewZPMZFile(ControlZPMZ::Popispole);
}

void MainWindow::on_actionNacist_2_triggered()
{
    loadZPMZFile(ControlZPMZ::Nacrt, tr("Otevřít soubor Náčrt"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_3_triggered()
{
    viewZPMZFile(ControlZPMZ::Nacrt);
}

void MainWindow::on_actionNacist_3_triggered()
{
    loadZPMZFile(ControlZPMZ::Zap, tr("Otevřít soubor Zápisník měření"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_4_triggered()
{
    viewZPMZFile(ControlZPMZ::Zap);
}

void MainWindow::on_actionNacist_4_triggered()
{
    loadZPMZFile(ControlZPMZ::Prot, tr("Otevřít soubor Protokol"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_5_triggered()
{
    viewZPMZFile(ControlZPMZ::Prot);
}

void MainWindow::on_actionNacist_5_triggered()
{
    loadZPMZFile(ControlZPMZ::Vymery, tr("Otevřít soubor Výměry"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_Uprava_triggered()
{
    viewZPMZFile(ControlZPMZ::Vymery);
}

void MainWindow::on_actionNacist_7_triggered()
{
    loadZPMZFile(ControlZPMZ::Sezvlast, tr("Otevřít soubor Seznámení vlastníků"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_7_triggered()
{
    viewZPMZFile(ControlZPMZ::Sezvlast);
}

void MainWindow::on_actionNacist_8_triggered()
{
    loadZPMZFile(ControlZPMZ::Oprav, tr("Otevřít soubor Opravy"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_8_triggered()
{
    viewZPMZFile(ControlZPMZ::Oprav);
}

void MainWindow::on_actionNacist_9_triggered()
{
    loadZPMZFile(ControlZPMZ::Dsps, tr("Otevřít soubor DSPS"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_9_triggered()
{
    viewZPMZFile(ControlZPMZ::Dsps);
}

void MainWindow::on_actionNacist_10_triggered()
{
    loadZPMZFile(ControlZPMZ::Vytyc, tr("Otevřít soubor Dokumentace o vytyčení"), tr("PDF dokument (*.pdf)"));
}

void MainWindow::on_actionNahled_a_Uprava_10_triggered()
{
    viewZPMZFile(ControlZPMZ::Vytyc);
}

void MainWindow::on_actionNahled_a_Uprava_11_triggered()
{
    viewZPMZFile(ControlZPMZ::GNSS);
}

// Action: clear every controller and delete generated work files.
void MainWindow::on_actionSmazatVysledky_triggered()
{
    controlGP.clearAll();
    controlZadost.clearAll();
    controlZPMZ.clearAll();

    DeleteWorkFiles::deleteFiles();
}

// Action: load the VFK návrh změny file (uses .vfk filter).
void MainWindow::on_actionNacist_6_triggered()
{
    loadZPMZFile(ControlZPMZ::Vfk, tr("Otevřít soubor návrh změny"), tr("Výměnný formát katastru (*.vfk)"));
}

// Action: run Python segmentation on the loaded GP to produce annotations.
void MainWindow::on_actionSegmentace_triggered()
{
    QString path_to_gp = controlGP.getPath();

    QFileInfo info(path_to_gp);
    
    // Guard: ensure valid GP file exists before attempting Python operations
    if (path_to_gp.isEmpty() || !info.exists() || !info.isFile())
    {
        QMessageBox::warning(this, tr("Neplatný soubor"), tr("Nebyl vybrán soubor GP"));
        return;
    }

    PythonManager::create_annotaions(path_to_gp);

    DisplayResults displayResults;
    appendText(displayResults.generateSegmentationReport(AddPath::getAnnotationPath()));
}

// Action: delete generated work files without touching controller state.
void MainWindow::on_actionVycistitVysledky_triggered()
{
    DeleteWorkFiles::deleteFiles();
}

// Action: classify PDFs inside the imported folder via Python and assign them to correct controllers.
void MainWindow::on_actionKlasifikaceSouboru_triggered()
{
    // Guard: ensure a directory was selected
    if (folder.isEmpty()) 
    {
        QMessageBox::warning(this, tr("Žádné data"), tr("Nejsou k dispozici žádné data."));
        return;
    }

    std::vector<QString> pdf_classes = PythonManager::classify(folder);

    QDir dir(folder);
    dir.setNameFilters({"*.pdf"});
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList fileList = dir.entryInfoList();

    const int n = std::min<int>(fileList.size(), static_cast<int>(pdf_classes.size()));

    std::vector<QString> fileNames;
    std::vector<QString> predictions;
    fileNames.reserve(n);
    predictions.reserve(n);

    // Map the classified outputs to their corresponding internal controllers
    for (int i = 0; i < n; ++i)
    {
        const QString filePath = fileList[i].absoluteFilePath();
        const QString& cls = pdf_classes[i];

        fileNames.push_back(fileList[i].fileName());
        predictions.push_back(cls);

        if (cls == QLatin1String("popispole"))
            controlZPMZ.setFilePath(ControlZPMZ::Popispole, filePath);
        else if (cls == QLatin1String("nacrt"))
            controlZPMZ.setFilePath(ControlZPMZ::Nacrt, filePath);
        else if (cls == QLatin1String("zap"))
            controlZPMZ.setFilePath(ControlZPMZ::Zap, filePath);
        else if (cls == QLatin1String("prot"))
            controlZPMZ.setFilePath(ControlZPMZ::Prot, filePath);
        else if (cls == QLatin1String("vymery"))
            controlZPMZ.setFilePath(ControlZPMZ::Vymery, filePath);
        else if (cls == QLatin1String("sezvlast"))
            controlZPMZ.setFilePath(ControlZPMZ::Sezvlast, filePath);
        else if (cls == QLatin1String("oprav"))
            controlZPMZ.setFilePath(ControlZPMZ::Oprav, filePath);
        else if (cls == QLatin1String("dsps"))
            controlZPMZ.setFilePath(ControlZPMZ::Dsps, filePath);
        else if (cls == QLatin1String("vytyc"))
            controlZPMZ.setFilePath(ControlZPMZ::Vytyc, filePath);
        else if (cls == QLatin1String("gp"))
            controlGP.setPath(filePath);
        else if (cls == QLatin1String("zadost"))
            controlZadost.setPath(filePath);
    }

    // Special case check for GNSS file logic
    const QString gnssPath = QDir(AddPath::getGNSSPath()).filePath("gnss.pdf");
    const bool hasGnss = QFileInfo::exists(gnssPath);
    if (hasGnss)
        controlZPMZ.setFilePath(ControlZPMZ::GNSS, gnssPath);

    DisplayResults displayResults;
    displayText(displayResults.generateReport(fileNames, predictions, hasGnss));
}