// Declaration of the FilePDF class for PDF file metadata.

#ifndef FILEPDF_H
#define FILEPDF_H

#include "filebase.h"

// --- FilePDF ---
// Represents metadata of a PDF file within a geometric plan.
class FilePDF : public FileBase
{
private:
    QString zpmz_gp; 
    QString version;        

public:
    // Default constructor. Initializes all fields to empty/zero/false.
    FilePDF();

    // Parameterized constructor.
    //   is_present  - Whether the PDF file is present.
    //   ku          - Cadastral unit number.
    //   zpmz        - ZPMZ number.
    //   zpmz_gp     - ZPMZ or GP indicator string.
    //   name        - File name.
    //   version     - PDF version string.
    //   date        - Last modification date/time.
    //   path        - Absolute file-system path.
    //   what_it_is  - Description if file does not match expectations.
    //   letter      - Letter suffix for multiple GPs per náčrt.
    FilePDF(bool is_present, int ku, int zpmz, const QString& zpmz_gp, 
            const QString& name, const QString& version, const QString& date, 
            const QString& path, const QString& what_it_is, const QString& letter);

    virtual ~FilePDF() = default;

    // --- Setters ---
    void setZpmz_gp(const QString& zpmz_gp);       
    void setVersion(const QString& version);        

    // Convenience method to set all attributes at once.
    void setAll(bool is_present, int ku, int zpmz, const QString& zpmz_gp, 
                const QString& name, const QString& version, const QString& date, 
                const QString& path, const QString& what_it_is, const QString& letter);

    // --- Getters ---
    QString getZpmz_gp() const;         
    QString getVersion() const;         

    // Clears base data and PDF-specific strings.
    void clearAll();
};

#endif // FILEPDF_H