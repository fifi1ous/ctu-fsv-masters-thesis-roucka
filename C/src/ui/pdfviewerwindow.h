// Declaration of PdfViewerWindow — base PDF viewing widget with zoom, navigation, and gesture support.

#ifndef PDFVIEWERWINDOW_H
#define PDFVIEWERWINDOW_H

#include <QWidget>
#include <QElapsedTimer>
#include <QPdfDocument>
#include <QPdfView>

// Forward declarations
class QPdfDocument;
class QPdfPageNavigator;
class QPushButton;
class QSpinBox;
class QLabel;

namespace Ui { class PdfViewerWindow; }

// --- PdfViewerWindow ---
class PdfViewerWindow : public QWidget
{
    Q_OBJECT

protected:
    // --- Event Overrides ---
    
    // Double-click zooms in (left), out (right) or resets (middle).
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    // Throttled wheel handler: Ctrl+wheel zooms, plain wheel scrolls.
    void wheelEvent(QWheelEvent *event) override;

    // Keyboard shortcuts: +/- zoom, 0 reset, Esc close.
    void keyPressEvent(QKeyEvent *event) override;

    // Catches QGestureEvent to dispatch pinch-to-zoom.
    bool event(QEvent *event) override;

    // Filters events on the inner QPdfView to implement panning, Ctrl+wheel zoom interception, and arrow-key navigation.
    bool eventFilter(QObject *watched, QEvent *event) override;

    // Evaluates whether arrow keys should turn pages based on the current zoom level and page mode.
    bool shouldAllowArrowPageTurn() const;

    // --- Widget pointers ---
    QPdfView* m_pdfView = nullptr;        // The embedded PDF view.
    QPushButton* m_zoomInBtn = nullptr;      // Zoom-in toolbar button.
    QPushButton* m_zoomOutBtn = nullptr;     // Zoom-out toolbar button.
    QPushButton* m_prevBtn = nullptr;        // Previous-page toolbar button.
    QPushButton* m_nextBtn = nullptr;        // Next-page toolbar button.
    QSpinBox* m_pageSpin = nullptr;       // 1-based current-page spin box.
    QLabel* m_pageCountLabel = nullptr; // Label that displays "/ N".

    // Apply a zoom factor relative to the current zoom level, maintaining the viewport anchor.
    virtual void applyZoom(double factor);

    // Loaded PDF document and internal page navigator state.
    QPdfDocument* m_doc = nullptr;
    QPdfPageNavigator* m_nav = nullptr;

    // Protected constructor for subclasses that provide their own .ui file.
    PdfViewerWindow(QWidget* parent);

    // Initialize the viewer by wiring up signals/slots, installing event filters, and loading the PDF document.
    void initViewer(const QString& pdfPath, QPdfView::PageMode pageMode);

protected Q_SLOTS:
    // Hook called when the navigator's current page changes.
    virtual void onCurrentPageChanged(int page0based);

public:
    // Construct, load pdfPath, and show the viewer.
    explicit PdfViewerWindow(const QString& pdfPath, QWidget* parent = nullptr, QPdfView::PageMode pageMode = QPdfView::PageMode::MultiPage);

    // Destructor — blocks signals on the document/navigator before cleanup.
    ~PdfViewerWindow();

private Q_SLOTS:
    // UI action handlers
    void zoomIn();                          // Toolbar/button slot — applyZoom(1.2).
    void zoomOut();                         // Toolbar/button slot — applyZoom(1/1.1).
    void prevPage();                        // Toolbar/button slot — jump to previous page.
    void nextPage();                        // Toolbar/button slot — jump to next page.
    void pageSpinChanged(int page1based);   // Spin-box slot — jump to a 1-based page.
    void onPageCountChanged(int pageCount); // QPdfDocument slot — update page label and spin range.

private:
    // Jump to a 0-based page index, with bounds checking.
    void jumpToPage0(int page0based);

    // Refresh the enabled state of the navigation UI based on current page context.
    void updateUiEnabled();

    // Base-class .ui (only used by the public constructor).
    Ui::PdfViewerWindow* m_baseUi = nullptr;    

    // Throttle timers to prevent event flooding during high-frequency inputs (trackpads, touchscreens).
    QElapsedTimer m_zoomTimer;                  
    QElapsedTimer m_scrollTimer;               

    // Maximum and minimum allowed zoom constraints.
    static constexpr double MIN_ZOOM = 0.30;
    static constexpr double MAX_ZOOM = 5.0;

    // Minimum interval between zoom and scroll events to ensure smooth rendering.
    static constexpr int ZOOM_THROTTLE_MS   = 15;   
    static constexpr int SCROLL_THROTTLE_MS = 15;   

    // State tracking for the click-and-drag pan gesture.
    bool    m_isPanning = false;
    QPointF m_panStart; 
};

#endif // PDFVIEWERWINDOW_H