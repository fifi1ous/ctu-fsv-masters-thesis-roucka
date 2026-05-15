// Core data structures used throughout the Control_GP application.

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>
#include <QRectF>

// --- BBox ---
// Represents a bounding box annotation on a PDF page.
struct BBox
{
    int     classId  = 0;   // Numeric class identifier for the detected object.
    QString className;      // Optional human-readable class label; falls back to "cls <id>".
    double  xMin = 0;       // Left edge of the bounding box in PDF points.
    double  yMin = 0;       // Top edge of the bounding box in PDF points.
    double  xMax = 0;       // Right edge of the bounding box in PDF points.
    double  yMax = 0;       // Bottom edge of the bounding box in PDF points.
};

// --- MapPartsGP ---
// Represents structural metadata for sections of a Geometric Plan (GP).
struct MapPartsGP
{
    QString name;           // Human-readable name of the GP part.
    QString abbreviation;   // Internal abbreviation of the GP part.
    QString path;           // Path to the directory where this segment's data is stored.
};

#endif // STRUCTURES_H