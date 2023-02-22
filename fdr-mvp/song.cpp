#include <fstream>
#include <string>
#include <QString>
#include <QUrl>
#include <QDebug>

#include "song.h"

Song::Song(QUrl chartfile) {
  // Initial values for the attributes
  QString title = QString("Untitled");
  QString artist = QString("Unknown Artist");
  QString album = QString("Unknown Album");
  QString year = QString("Year Unknown");
  QString genre = QString("Unknown Genre");
  QString charter = QString("Unknown Charter");
  QString audiofile = QString("No audio file found");

  // Temporary variables for non-string (will need to be changed to ints)
  QString resolutionStr = QString("");
  QString offsetStr = QString("");
  // Attributes for easy parsing:
  QString *attributePtr[] = {
    &title, &artist, &album, &year, &genre,
    &charter, &resolutionStr, &offsetStr, &audiofile
  };
  std::string attributeNames[] {
    "Name", "Artist", "Album", "Year", "Genre",
    "Charter", "Resolution", "Offset", "MusicStream"
  };

  // open the file
  std::ifstream file;
  file.open(chartfile);
  if (!file.is_open()) throw "Failed to open file";

  std::string line;
  // Go through every line in the file
  while (getline(file, line)) {
    // Check for every possible attribute on that line
    for (int i=0; i<9; i++) {
      // If that attribute is a match, assign the value to the attribute
      if ( line.find(attributeNames[i]) != std::string::npos ) {
        qDebug() << QString::fromStdString("Found attribute: ")
                 << QString::fromStdString(attributeNames[i]);
        // The pattern in the file is `attribute: "value",`
        // so we need to get the value from that is in the quotes
        int start = line.find('"') + 1;
        int end = line.find('"', start);
        *attributePtr[i] = QString::fromStdString(
            line.substr(start, end-start)
        );
        qDebug() << "Value: " << *attributePtr[i];
      }
    }
  }
  // Convert the resolution and offset strings to ints
  if (resolutionStr != QString("")) resolution = resolutionStr.toInt();
  if (offsetStr != QString("")) offset = offsetStr.toInt();
}
