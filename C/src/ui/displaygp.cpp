// Implementation of DisplayGP.

#include "displaygp.h"
#include "ui_displaygp.h"

// Build the DisplayGP UI, populate base-class widget pointers, and attach the bounding-box overlay.
DisplayGP::DisplayGP(const QString &path, QWidget *parent)
    : PdfViewerWindow(parent)
{
    m_displayUi = new Ui::DisplayGP;
    m_displayUi->setupUi(this);

    // Populate base-class widget pointers from our own UI
    m_pdfView        = m_displayUi->pdfView;
    m_zoomInBtn      = m_displayUi->zoomInBtn;
    m_zoomOutBtn     = m_displayUi->zoomOutBtn;
    m_prevBtn        = m_displayUi->prevBtn;
    m_nextBtn        = m_displayUi->nextBtn;
    m_pageSpin       = m_displayUi->pageSpin;
    m_pageCountLabel = m_displayUi->pageCountLabel;

    initViewer(path, QPdfView::PageMode::SinglePage);

    // With m_doc and m_pdfView ready, initialize the overlay
    m_overlay = new BBoxOverlay(m_pdfView, m_doc, this);

    // Wire up "Viditelné" checkbox to toggle bounding-box visibility
    m_displayUi->visible->setChecked(true);
    connect(m_displayUi->visible, &QCheckBox::toggled,
            m_overlay, &QWidget::setVisible);

    // Wire up "Úpravy" checkbox to toggle interactive edit mode (move / resize boxes)
    connect(m_displayUi->edit, &QCheckBox::toggled,
            m_overlay, &BBoxOverlay::setEditMode);
}

// Destructor — frees the .ui object. The overlay is owned by Qt's parent hierarchy.
DisplayGP::~DisplayGP()
{
    delete m_displayUi;
}

// Forward the new page index to the overlay after the base class updates navigation state.
void DisplayGP::onCurrentPageChanged(int page0based)
{
    // Let the base class update the page spin / nav as usual
    PdfViewerWindow::onCurrentPageChanged(page0based);

    // Notify the overlay of the active page index
    if (m_overlay)
        m_overlay->notifyPageChanged(page0based);
}

// Apply the base-class zoom and ask the overlay to repaint at the new scale.
void DisplayGP::applyZoom(double factor)
{
    // Let the base class execute the zoom math and apply it
    PdfViewerWindow::applyZoom(factor);

    // Notify the overlay to rescale and repaint annotations
    if (m_overlay)
        m_overlay->notifyZoomChanged();
}