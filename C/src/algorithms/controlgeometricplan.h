// Declaration of ControlGeometricPlan — metadata controller for geometric plan files.

#ifndef CONTROLGEOMETRICPLAN_H
#define CONTROLGEOMETRICPLAN_H

#include "filepdf.h"
#include <QString>

// --- ControlGeometricPlan ---
// Subclass container managing the main Geometric Plan PDF document.
//
// Extends the standard FilePDF to provide a distinct type for the primary 
// geometric plan, allowing the main window and python bridge to track it 
// separately from the supporting ZPMZ documents.
class ControlGeometricPlan : public FilePDF
{
public:
    ControlGeometricPlan();
};

#endif // CONTROLGEOMETRICPLAN_H