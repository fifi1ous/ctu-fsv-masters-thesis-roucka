// Declaration of ControlZadost — metadata controller for application/request files.

#ifndef CONTROLZADOST_H
#define CONTROLZADOST_H

#include "filepdf.h"

// --- ControlZadost ---
// Subclass container managing the "Žádost" (Application/Request) PDF.
class ControlZadost : public FilePDF
{
public:
    ControlZadost();
};

#endif // CONTROLZADOST_H