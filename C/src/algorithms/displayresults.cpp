// Implementation of DisplayResults.

#include "displayresults.h"
#include "maps.h"
#include <QTextStream>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QRegularExpression>
#include <QSet>
#include <QStringList>
#include <algorithm>

DisplayResults::DisplayResults() {}

namespace {

// Helper: truncate or pad a string to exactly 'width' characters.
// This code was modified by Claude Opus 4.7 //
QString fitToWidth(const QString& s, int width)
{
    if (width <= 0)
        return QString();
    if (s.length() <= width)
        return s.leftJustified(width);
    return s.left(width - 1) + QStringLiteral("…");
}
// End of code modified by Claude Opus 4.7 //

// Helper: Find the NAMESMAP key whose token best matches the filename.
QString findExpectedKey(const QString& fileName)
{
    QString bestKey;
    int bestLen = 0;
    for (auto it = NAMESMAP.begin(); it != NAMESMAP.end(); ++it) {
        if (fileName.contains(it.key(), Qt::CaseInsensitive) && it.key().length() > bestLen) {
            bestLen = it.key().length();
            bestKey = it.key();
        }
    }
    return bestKey;
}

// Helper: Retrieve the localized label for a given key, falling back to the key itself.
QString labelFor(const QString& key)
{
    auto it = NAMESMAP.find(key);
    return (it == NAMESMAP.end()) ? key : it.value();
}

}


QString DisplayResults::generateReport(const std::vector<QString>& fileNames,
                                       const std::vector<QString>& predictions,
                                       bool hasGnss)
{
    const size_t rowCount = (std::min)(fileNames.size(), predictions.size());

    struct Row {
        QString expectedLabel;
        QString fileName;
        QString predictedLabel;
        bool    hasExpected;
        bool    match;
    };

    std::vector<Row> rows;
    rows.reserve(rowCount);

    int correctCount = 0;
    
    // Evaluate predictions against expected keys derived from filenames
    for (size_t i = 0; i < rowCount; ++i) {
        const QString& fileName   = fileNames[i];
        const QString& prediction = predictions[i];

        const QString expectedKey = findExpectedKey(fileName);
        const bool hasExpected    = !expectedKey.isEmpty();
        
        // Compare internal short codes rather than localized labels
        const bool match = hasExpected && expectedKey.compare(prediction, Qt::CaseInsensitive) == 0;
        if (match)
            ++correctCount;

        rows.push_back({
            hasExpected ? labelFor(expectedKey) : QStringLiteral("—"),
            fileName,
            labelFor(prediction),
            hasExpected,
            match
        });
    }

    // Define table headers
    const QString h1 = QStringLiteral("Soubor");
    const QString h2 = QStringLiteral("Název souboru");
    const QString h3 = QStringLiteral("Predikce třídy");
    const QString h4 = QStringLiteral("Správně");

    // Hardcoded column widths optimized for expected Czech content lengths
    constexpr int w1 = 21;  // Fits "Protokol o výpočtech"
    constexpr int w2 = 42;  // Fits long standard filenames (e.g., "641146_ZPMZ_040111_popispole_A_signed.pdf")
    constexpr int w3 = 21;  // Fits "Protokol o výpočtech"
    constexpr int w4 = 9;   // Fits "Správně"

    // Outer cell width incorporates a single space of padding on each side
    const int c1 = w1 + 2;
    const int c2 = w2 + 2;
    const int c3 = w3 + 2;
    const int c4 = w4 + 2;

    QString result;
    QTextStream out(&result);

    // Closure to format and write a single table row
    auto writeRow = [&](const QString& a, const QString& b, const QString& c, const QString& d) {
        out << "| " << fitToWidth(a, w1) << " "
            << "| " << fitToWidth(b, w2) << " "
            << "| " << fitToWidth(c, w3) << " "
            << "| " << fitToWidth(d, w4) << " |\n";
    };

    // Closure to draw horizontal table dividers
    auto writeSeparator = [&](QChar ch) {
        out << "+" << QString(c1, ch)
            << "+" << QString(c2, ch)
            << "+" << QString(c3, ch)
            << "+" << QString(c4, ch)
            << "+\n";
    };

    // Assemble table
    writeSeparator('=');
    writeRow(h1, h2, h3, h4);
    writeSeparator('=');
    
    for (const auto& r : rows) {
        const QString matchText = !r.hasExpected ? QStringLiteral("?")
                                                 : (r.match ? QStringLiteral("✓") : QStringLiteral("✗"));
        writeRow(r.expectedLabel, r.fileName, r.predictedLabel, matchText);
    }
    writeSeparator('=');

    // Append summary statistics
    out << "\n";
    out << "Správně klasifikováno: " << correctCount << " / " << rowCount << "\n";
    out << "Bylo měřeno pomocí GNSS: " << (hasGnss ? "Ano" : "Ne") << "\n";

    return result;
}


QString DisplayResults::generateSegmentationReport(const QString& annotationDir)
{
    QDir dir(annotationDir);
    if (!dir.exists()) {
        return QStringLiteral("Složka s anotacemi neexistuje: %1\n").arg(annotationDir);
    }

    dir.setNameFilters({QStringLiteral("page_*.txt")});
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList annotFiles = dir.entryInfoList();

    if (annotFiles.isEmpty()) {
        return QStringLiteral("Ve složce s anotacemi nebyly nalezeny žádné soubory.\n");
    }

    // Mappings to track which pages contain which classes, and detection frequency.
    QMap<int, QList<int>> pagesByClass;
    QMap<int, int>        countByClass;

    static const QRegularExpression pageRe(QStringLiteral("^page_(\\d+)$"));

    // Parse each YOLO annotation file to aggregate detections
    for (const QFileInfo& fi : annotFiles) {
        const auto match = pageRe.match(fi.completeBaseName());
        if (!match.hasMatch())
            continue;
            
        const int pageNum = match.captured(1).toInt(); // Python writes 1-based indexes

        QFile f(fi.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QSet<int> classesOnThisPage;
        
        while (!f.atEnd()) {
            const QString line = QString::fromUtf8(f.readLine()).trimmed();
            if (line.isEmpty())
                continue;

            const QStringList parts = line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (parts.isEmpty())
                continue;

            bool ok = false;
            const int classId = parts.first().toInt(&ok);
            if (!ok)
                continue;

            countByClass[classId] += 1;
            classesOnThisPage.insert(classId);
        }
        
        for (int cls : classesOnThisPage)
            pagesByClass[cls].append(pageNum);
    }

    // Sort page lists for clean reporting
    for (auto it = pagesByClass.begin(); it != pagesByClass.end(); ++it) {
        std::sort(it.value().begin(), it.value().end());
    }

    struct SegRow {
        QString name;
        int     count;
        QString pages;
        QString missing;
    };

    std::vector<SegRow> rows;
    QSet<int> knownClasses;
    const auto& names = BB_NAMES();

    // Prepare auxiliary variables for missing-class logic evaluation
    bool hasAnyMainClass = !pagesByClass.value(0).isEmpty() ||
                           !pagesByClass.value(1).isEmpty() ||
                           !pagesByClass.value(3).isEmpty() ||
                           !pagesByClass.value(4).isEmpty();

    int  class0PageCount = pagesByClass.value(0).size();
    bool hasClass5       = !pagesByClass.value(5).isEmpty();

    // Populate rows based on detected classes
    for (auto it = names.begin(); it != names.end(); ++it) {
        const int classId = it.key();
        knownClasses.insert(classId);

        const auto pagesForClass = pagesByClass.value(classId);
        const int  countForClass = countByClass.value(classId, 0);

        QStringList pageStrs;
        for (int p : pagesForClass)
            pageStrs << QString::number(p);

        QString pagesFormatted = pageStrs.isEmpty()
                                     ? QStringLiteral("—")
                                     : pageStrs.join(QStringLiteral(", "));

        // Determine if a mandatory class is missing
        const bool classIsMissing = pagesForClass.isEmpty() && countForClass == 0;
        QString missingStatus = "";

        if (classIsMissing && classId != 5) {
            missingStatus = QStringLiteral("CHYBÍ ❌");
        }

        // Specific heuristic: if class 0 appears on 3+ pages and class 5 is absent, flag class 5 as missing.
        if (class0PageCount >= 3 && !hasClass5 && classId == 5) {
            missingStatus = QStringLiteral("CHYBÍ ❌");
        }

        rows.push_back({
            it.value().name,
            countForClass,
            pagesFormatted,
            missingStatus
        });
    }

    // Hardcoded column widths for the segmentation ASCII table
    constexpr int w1 = 21;
    constexpr int w2 = 7;
    constexpr int w3 = 32;
    constexpr int w4 = 10;

    QString result;
    QTextStream out(&result);

    // Closure to format and write a single table row
    auto writeRow = [&](const QString& a, const QString& b, const QString& c, const QString& d) {
        out << "| " << fitToWidth(a, w1 - 1)
            << "| " << fitToWidth(b, w2 - 1)
            << "| " << fitToWidth(c, w3 - 1)
            << "| " << fitToWidth(d, w4 - 1)
            << "|\n";
    };

    // Closure to draw horizontal table dividers
    auto writeSeparator = [&](QChar ch) {
        out << "|" << QString(10, ch)
            << "|" << QString(w2, ch)
            << "|" << QString(w3, ch)
            << "|" << QString(w4, ch)
            << "|\n";
    };

    // Assemble table
    writeSeparator('=');
    writeRow(QStringLiteral("Náležitost"), QStringLiteral("Počet"),
             QStringLiteral("Strany"), QStringLiteral("Chybí"));
    writeSeparator('=');

    int totalDetections = 0;
    for (const auto& r : rows) {
        writeRow(r.name, QString::number(r.count), r.pages, r.missing);
        totalDetections += r.count;
    }
    writeSeparator('=');

    // Append summary statistics
    out << "\n";
    out << "Zpracováno stran: " << annotFiles.size() << "\n";
    out << "Celkem detekcí: "  << totalDetections   << "\n";

    return result;
}