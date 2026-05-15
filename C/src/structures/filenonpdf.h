// Declaration of the FileNonPDF class for non-PDF file metadata.

#ifndef FILENONPDF_H
#define FILENONPDF_H

#include "filebase.h"

// --- FileNonPDF ---
// Represents metadata of a non-PDF file within a geometric plan.
class FileNonPDF : public FileBase
{
private:
    QString zpmz_gp_noth;   // Indicates whether the file belongs to ZPMZ, GP, or neither.
    QString format;         // File format or extension string.

public:
    // Default constructor. Initializes all fields to empty/zero/false.
    FileNonPDF();

    // Parameterized constructor.
    //   is_present    - Whether the file is present.
    //   ku            - Cadastral unit number.
    //   zpmz          - ZPMZ number.
    //   zpmz_gp_noth  - ZPMZ, GP, or nothing indicator string.
    //   name          - File name.
    //   format        - File format/extension.
    //   date          - Last modification date/time.
    //   path          - Absolute file-system path.
    //   what_it_is    - Description if file does not match expectations.
    //   letter        - Letter suffix for multiple GPs per náčrt.
    FileNonPDF(bool is_present, int ku, int zpmz, const QString& zpmz_gp_noth, 
               const QString& name, const QString& format, const QString& date, 
               const QString& path, const QString& what_it_is, const QString& letter);

    virtual ~FileNonPDF() = default;

    // --- Setters ---
    void setZpmz_gp_noth(const QString& zpmz_gp_noth);     
    void setFormat(const QString& format);                  

    // Convenience method to set all attributes at once.
    void setAll(bool is_present, int ku, int zpmz, const QString& zpmz_gp_noth, 
                const QString& name, const QString& format, const QString& date, 
                const QString& path, const QString& what_it_is, const QString& letter);

    // --- Getters ---
    QString getZpmz_gp_noth() const;    
    QString getFormat() const;          

    // Clears base data and format-specific strings.
    void clearAll();
};

#endif // FILENONPDF_H