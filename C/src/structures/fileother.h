// Declaration of the FileOther class for miscellaneous file metadata.

#ifndef FILEOTHER_H
#define FILEOTHER_H

#include <QString>

// --- FileOther ---
// Represents metadata of an additional/miscellaneous file in a geometric plan.
class FileOther
{
private:
    QString name;           // File name.
    QString format;         // File format/extension.
    QString path;           // Absolute path to the file on disk.
    QString what_it_is;     // Description of what the file contains.

public:
    // Default constructor.
    FileOther();

    // Parameterized constructor.
    //   name        - File name.
    //   format      - File format/extension.
    //   path        - Absolute file-system path.
    //   what_it_is  - Description of the file contents.
    FileOther(const QString& name, const QString& format, const QString& path, const QString& what_it_is);

    virtual ~FileOther() = default;

    // --- Setters ---
    void setName(const QString& name);              
    void setFormat(const QString& format);          
    void setPath(const QString& path);              
    void setWhatItIs(const QString& what_it_is);    

    // --- Getters ---
    QString getName() const;            
    QString getFormat() const;          
    QString getPath() const;            
    QString getWhatItIs() const;        
};

// Equality operator for FileOther.
inline bool operator==(const FileOther& lhs, const FileOther& rhs)
{
    return lhs.getName() == rhs.getName() &&
           lhs.getFormat() == rhs.getFormat() &&
           lhs.getPath() == rhs.getPath() &&
           lhs.getWhatItIs() == rhs.getWhatItIs();
}

#endif // FILEOTHER_H