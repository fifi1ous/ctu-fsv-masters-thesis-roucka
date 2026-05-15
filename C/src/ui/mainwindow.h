// Declaration of MainWindow — top-level window for the Control_GP application.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QFileDialog>
#include <QMessageBox>

#include "geometricplan.h"
#include "controlgeometricplan.h"
#include "controlzpmz.h"
#include "controlzadost.h"
#include "pythonmanager.h"

#include "pdfviewerwindow.h"
#include "displayzpmz.h"
#include "displaygp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// --- MainWindow ---
// Application main window — wires menu actions to the controllers and PDF viewers.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Construct the main window and load the .ui.
    MainWindow(QWidget *parent = nullptr);

    // Destructor — frees the .ui object.
    ~MainWindow();

    // Display text in the results QTextEdit.
    void displayText(const QString& text);

    // Append text to the results QTextEdit, preserving previous content.
    void appendText(const QString& text);

private Q_SLOTS:
    // --- Geometric-plan and import actions ---
    void on_actionNactiGP_triggered();          // Load a geometric-plan PDF into controlGP.
    void on_actionOtevri_triggered();           // Multi-file import dialog (placeholder).
    void on_actionImportSlozky_triggered();     // Folder import dialog (placeholder).
    void on_actionNahled_a_Uprava_triggered();  // View the loaded GP in a DisplayGP viewer.

    // --- Žádost actions ---
    void on_actionNacistZadost_triggered();     // Load the Žádost PDF.
    void on_actionNahled_triggered();           // View the loaded Žádost PDF.

    // --- ZPMZ document load/view pairs ---.
    void on_actionNacist_triggered();                // Load Popispole PDF.
    void on_actionNahled_a_Uprava_2_triggered();     // View Popispole PDF.

    void on_actionNacist_2_triggered();              // Load Náčrt PDF.
    void on_actionNahled_a_Uprava_3_triggered();     // View Náčrt PDF.

    void on_actionNacist_3_triggered();              // Load Zápisník měření PDF.
    void on_actionNahled_a_Uprava_4_triggered();     // View Zápisník měření PDF.

    void on_actionNacist_4_triggered();              // Load Protokol PDF.
    void on_actionNahled_a_Uprava_5_triggered();     // View Protokol PDF.

    void on_actionNacist_5_triggered();              // Load Výměry PDF.
    void on_actionNahled_Uprava_triggered();         // View Výměry PDF.

    void on_actionNacist_7_triggered();              // Load Seznámení vlastníků PDF.
    void on_actionNahled_a_Uprava_7_triggered();     // View Seznámení vlastníků PDF.

    void on_actionNacist_8_triggered();              // Load Opravy PDF.
    void on_actionNahled_a_Uprava_8_triggered();     // View Opravy PDF.

    void on_actionNacist_9_triggered();              // Load DSPS PDF.
    void on_actionNahled_a_Uprava_9_triggered();     // View DSPS PDF.

    void on_actionNacist_10_triggered();             // Load Dokumentace o vytyčení PDF.
    void on_actionNahled_a_Uprava_10_triggered();    // View Dokumentace o vytyčení PDF.

    void on_actionNacist_6_triggered();              // Load VFK návrh změny file.

    // --- Workspace and processing actions ---
    void on_actionSmazatVysledky_triggered();      // Clear all controllers and delete generated work files.
    void on_actionSegmentace_triggered();          // Run Python annotation/segmentation on the loaded GP.
    void on_actionVycistitVysledky_triggered();    // Delete generated work files only.
    void on_actionNahled_a_Uprava_11_triggered();  // View GNSS PDF.
    void on_actionKlasifikaceSouboru_triggered();  // Run Python classification on imported folder.

private:
    Ui::MainWindow *ui;                         // uic-generated UI for the main window.
    ControlGeometricPlan controlGP;             // Controller for the geometric-plan PDF.
    ControlZadost controlZadost;                // Controller for the Žádost PDF.
    ControlZPMZ controlZPMZ;                    // Controller for the ZPMZ document set.
    QPointer<PdfViewerWindow> m_pdfViewer;      // Currently shown viewer (nullptr if none).
    QString folder;                             // Path cache for the imported folder.

    // Helper: Open a file dialog and store the chosen path in controlZPMZ for the given type.
    void loadZPMZFile(ControlZPMZ::FileType type, const QString& dialogTitle, const QString& filter);

    // Helper: Display the file currently associated with type using a DisplayZPMZ viewer.
    void viewZPMZFile(ControlZPMZ::FileType type);

    // Template Helper: Display path in a fresh viewer of type T, replacing any existing one.
    // This code was modified by Claude Opus 4.7 //
    template<typename T>
    void checkAndDisplayPDF(const QString &path)
    {
        QFileInfo info(path);
        
        // Guard: Prevent launching viewer with invalid or missing files
        if (path.isEmpty() || !info.exists() || !info.isFile())
        {
            QMessageBox::warning(this, tr("Neplatný soubor"), tr("Vybraný soubor není platné PDF."));
            return;
        }

        // Close existing viewer to maintain a single-instance paradigm
        if (m_pdfViewer) {
            m_pdfViewer->close();
        }

        // Launch new viewer
        m_pdfViewer = new T(path, this);
        m_pdfViewer->setWindowFlag(Qt::Window, true);
        m_pdfViewer->setAttribute(Qt::WA_DeleteOnClose);
        m_pdfViewer->show();
    }
    // End of code modified by Claude Opus 4.7 //
};

#endif // MAINWINDOW_H