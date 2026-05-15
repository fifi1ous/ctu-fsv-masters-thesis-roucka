// Declaration of DisplayGP — PDF viewer specialized for geometric-plan documents with bounding-box overlay.

#ifndef DISPLAYGP_H
#define DISPLAYGP_H

#include "pdfviewerwindow.h"
#include "bboxoverlay.h"

namespace Ui { class DisplayGP; }

// --- DisplayGP ---
// Single-page PDF viewer that renders an interactive BBoxOverlay on top of the geometric plan.
class DisplayGP : public PdfViewerWindow
{
    Q_OBJECT

public:
    // Construct a DisplayGP that loads the PDF at path.
    explicit DisplayGP(const QString &path, QWidget *parent = nullptr);

    // Destructor — frees the .ui object.
    ~DisplayGP();

private Q_SLOTS:
    // Forward page changes to the BBoxOverlay so it repaints for the new page.
    void onCurrentPageChanged(int page0based) override;

private:
    // Apply zoom and notify the overlay so it can rescale its boxes.
    void applyZoom(double factor) override;

    // uic-generated UI for this subclass.
    Ui::DisplayGP* m_displayUi = nullptr;   
    
    // Overlay widget that draws bounding boxes on top of m_pdfView.
    BBoxOverlay* m_overlay = nullptr;     
};

#endif // DISPLAYGP_H