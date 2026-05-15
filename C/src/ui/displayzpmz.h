// Declaration of DisplayZPMZ — multi-page PDF viewer for ZPMZ documents.

#ifndef DISPLAYZPMZ_H
#define DISPLAYZPMZ_H

#include "pdfviewerwindow.h"

namespace Ui { class DisplayZPMZ; }

// --- DisplayZPMZ ---
// PDF viewer specialized for ZPMZ documents.
class DisplayZPMZ : public PdfViewerWindow
{
    Q_OBJECT

public:
    // Construct a DisplayZPMZ that loads the PDF at path in multi-page mode.
    explicit DisplayZPMZ(const QString &path, QWidget *parent = nullptr);

    // Destructor — frees the .ui object.
    ~DisplayZPMZ();

private Q_SLOTS:
    // Forward page-change events to the base class.
    void onCurrentPageChanged(int page0based) override;

private:
    // Forward zoom requests to the base class.
    void applyZoom(double factor) override;

    // uic-generated UI for this subclass.
    Ui::DisplayZPMZ* m_displayUi = nullptr; 
};

#endif // DISPLAYZPMZ_H