// Declaration of the FileBase class — common fields for cadastral file metadata.

#ifndef FILEBASE_H
#define FILEBASE_H

#include <QString>

// --- FileBase ---
// Base class for cadastral file metadata shared by FilePDF and FileNonPDF.
class FileBase
{
private:
    bool is_present;        // Whether the file is present in the submission.
    int ku;                 // Cadastral unit number (katastrální území).
    int zpmz;               // ZPMZ number (záznam podrobného měření změn).
    QString name;           // File name.
    QString date;           // Date and time of the last modification.
    QString path;           // Absolute path to the file on disk.
    QString what_it_is;     // Description when the file differs from expectations.
    QString letter;         // Letter suffix used when multiple GPs share one "náčrt".

public:
    FileBase();
    FileBase(bool is_present, int ku, int zpmz, const QString& name,
             const QString& date, const QString& path,
             const QString& what_it_is, const QString& letter);

    virtual ~FileBase() = default;

    // --- Setters ---
    void setPresent(bool is_present);
    void setKu(int ku);
    void setZpmz(int zpmz);
    void setName(const QString& name);
    void setDate(const QString& date);
    void setPath(const QString& path);
    void setWhatItIs(const QString& what_it_is);
    void setLetter(const QString& letter);

    // --- Getters ---
    bool getPresent() const;
    int getKu() const;
    int getZpmz() const;
    QString getName() const;
    QString getDate() const;
    QString getPath() const;
    QString getWhatItIs() const;
    QString getLetter() const;

    // Resets all fields to their default, empty, or false states.
    void clearAll();
};

#endif // FILEBASE_H