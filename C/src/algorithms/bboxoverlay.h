// Declaration of BBoxOverlay — transparent PDF annotation rendering and editing widget.

#ifndef BBOXOVERLAY_H
#define BBOXOVERLAY_H

#include <QWidget>
#include <QColor>
#include <QMap>
#include <QVector>
#include <QString>
#include <QPointF>

#include "structures.h"
#include "addpath.h"

class QPdfView;
class QPdfDocument;

// --- BBoxOverlay ---
// A transparent overlay widget reparented onto a QPdfView's viewport.
class BBoxOverlay : public QWidget
{
    Q_OBJECT

public:
    // Construct the overlay and bind it to the target PDF view.
    explicit BBoxOverlay(QPdfView* pdfView, QPdfDocument* doc, QWidget* parent = nullptr);

    // Set the transparency of the filled detection rectangles (0-255).
    void setFillAlpha(int alpha);

    // Toggle interactive edit mode to allow moving, resizing, and saving bounding boxes.
    void setEditMode(bool on);
    bool editMode() const { return m_editMode; }

    // Navigation and scaling hooks driven by the parent PdfViewerWindow.
    void notifyPageChanged(int page0based);
    void notifyZoomChanged();

protected:
    // --- Core Events ---
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    
    // --- Mouse Interaction ---
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // --- Hit-Testing State ---
    // Identifies which part of a bounding box the mouse is hovering over/dragging.
    enum Handle {
        HandleNone = 0,
        HandleMove,       
        HandleTopLeft,
        HandleTop,
        HandleTopRight,
        HandleRight,
        HandleBottomRight,
        HandleBottom,
        HandleBottomLeft,
        HandleLeft
    };

    struct HitResult {
        int    boxIndex = -1;       // Index of the box in the page's vector
        Handle handle   = HandleNone;
    };

    static constexpr int HANDLE_SIZE = 6; // Half-size (radius) of interaction grips

    // Determine what box/handle is at the given screen coordinate.
    HitResult hitTest(const QPointF& pos) const;
    
    // Map a handle type to the appropriate OS cursor icon.
    Qt::CursorShape cursorForHandle(Handle h) const;

    // --- Coordinate Transformers ---
    // Map a screen pixel position to PDF point coordinates.
    QPointF overlayToPdf(int page0based, const QPointF& pos) const;

    // Map a PDF point bounding box to a screen pixel QRectF.
    QRectF pdfRectToOverlay(int page0based, const BBox& box) const;

    // --- I/O & Caching ---
    // Persist edited annotations to disk.
    void saveAnnotations(int page0based);

    // Retrieve cached annotations for a page (loads from disk on first request).
    const QVector<BBox>& boxesForPage(int page0based);

    // Retrieve mutable access to the page's annotations.
    QVector<BBox>& mutableBoxesForPage(int page0based);

    // --- Visual Helpers ---
    QColor  colorForClass(int classId) const;
    QString labelForClass(int classId) const;
    static QColor paletteColor(int index);

    // --- Data Members ---
    QPdfView* m_pdfView   = nullptr;
    QPdfDocument* m_doc       = nullptr;
    QWidget* m_viewport  = nullptr; // The actual drawing surface of QPdfView

    QString m_annotationDir = AddPath::getAnnotationPath();
    int     m_currentPage   = 0;
    int     m_fillAlpha     = 40;

    QMap<int, QVector<BBox>> m_cache; // Memory cache: pageIndex -> list of boxes

    // --- Interaction State ---
    bool    m_editMode     = false;
    int     m_selectedBox  = -1;
    Handle  m_activeHandle = HandleNone;
    bool    m_dragging     = false;
    QPointF m_dragStartPdf;           // Initial PDF coordinate at drag start
    BBox    m_dragOrigBox;            // Snapshot of box dimensions before mutation
};

#endif // BBOXOVERLAY_H