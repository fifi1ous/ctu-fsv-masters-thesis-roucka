// Declaration of DisplayResults — report generator for classification and segmentation.

#ifndef DISPLAYRESULTS_H
#define DISPLAYRESULTS_H

#include <QString>
#include <vector>

// --- DisplayResults ---
// Utility class responsible for generating plain-text/ASCII-table reports.

class DisplayResults
{
public:
    DisplayResults();

    // Generate a classification report table.
    QString generateReport(const std::vector<QString>& fileNames,
                           const std::vector<QString>& predictions,
                           bool hasGnss);

    // Generate a segmentation report table.
    QString generateSegmentationReport(const QString& annotationDir);
};

#endif // DISPLAYRESULTS_H