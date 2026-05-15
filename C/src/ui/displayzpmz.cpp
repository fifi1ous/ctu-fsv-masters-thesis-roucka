// Implementation of DisplayZPMZ.

#include "displayzpmz.h"
#include "ui_displayzpmz.h"

// Build the DisplayZPMZ UI, populate base-class widget pointers, and load the path in multi-page mode.
DisplayZPMZ::DisplayZPMZ(const QString &path, QWidget *parent)
    : PdfViewerWindow(parent)
{
    m_displayUi = new Ui::DisplayZPMZ;
    m_displayUi->setupUi(this);

    // Populate base-class widget pointers from our own UI file
    m_pdfView        = m_displayUi->pdfView;
    m_zoomInBtn      = m_displayUi->zoomInBtn;
    m_zoomOutBtn     = m_displayUi->zoomOutBtn;
    m_prevBtn        = m_displayUi->prevBtn;
    m_nextBtn        = m_displayUi->nextBtn;
    m_pageSpin       = m_displayUi->pageSpin;
    m_pageCountLabel = m_displayUi->pageCountLabel;

    initViewer(path, QPdfView::PageMode::MultiPage);
}

// Destructor — frees the .ui object.
DisplayZPMZ::~DisplayZPMZ()
{
    delete m_displayUi;
}

// Forward the page change to the base class. 
void DisplayZPMZ::onCurrentPageChanged(int page0based)
{
    // Let the base class update the page spin and navigation states
    PdfViewerWindow::onCurrentPageChanged(page0based);
}

// Forward the zoom request to the base class. 
void DisplayZPMZ::applyZoom(double factor)
{
    // Let the base class execute the actual zoom logic
    PdfViewerWindow::applyZoom(factor);
}