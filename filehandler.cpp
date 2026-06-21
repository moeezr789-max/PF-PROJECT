#include "filehandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::vector<std::string>> readTXT(const std::string& filename) {
    std::vector<std::vector<std::string>> result;
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << "\n";
        return result;
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);

        if (firstLine) { firstLine = false; continue; }
        if (line.empty()) continue;

        std::vector<std::string> fields;
        std::string field = "";
        bool inQuotes = false;

        for (int i = 0; i < (int)line.length(); i++) {
            char c = line[i];
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(field);
                field = "";
            } else {
                field += c;
            }
        }
        fields.push_back(field);
        result.push_back(fields);
    }

    file.close();
    return result;
}

void writeTXT(const std::string& filename,
              const std::vector<std::string>& header,
              const std::vector<std::vector<std::string>>& rows) {
    std::ofstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << " for writing.\n";
        return;
    }

    for (int i = 0; i < (int)header.size(); i++) {
        if (i > 0) file << ",";
        file << header[i];
    }
    file << "\n";

    for (int i = 0; i < (int)rows.size(); i++) {
        for (int j = 0; j < (int)rows[i].size(); j++) {
            if (j > 0) file << ",";

            bool hasComma = false;
            for (int k = 0; k < (int)rows[i][j].length(); k++) {
                if (rows[i][j][k] == ',') { hasComma = true; break; }
            }

            if (hasComma)
                file << "\"" << rows[i][j] << "\"";
            else
                file << rows[i][j];
        }
        file << "\n";
    }

    file.close();
}

void appendTXT(const std::string& filename,
               const std::vector<std::string>& row) {
    std::ofstream file(filename.c_str(), std::ios::app);
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filename << " for appending.\n";
        return;
    }

    for (int i = 0; i < (int)row.size(); i++) {
        if (i > 0) file << ",";

        bool hasComma = false;
        for (int k = 0; k < (int)row[i].length(); k++) {
            if (row[i][k] == ',') { hasComma = true; break; }
        }

        if (hasComma)
            file << "\"" << row[i] << "\"";
        else
            file << row[i];
    }
    file << "\n";

    file.close();
}

std::vector<std::string> findRow(const std::string& filename,
                                  int colIndex,
                                  const std::string& value) {
    std::vector<std::vector<std::string>> rows = readTXT(filename);
    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() > colIndex && rows[i][colIndex] == value)
            return rows[i];
    }
    return std::vector<std::string>();
}

bool rowExists(const std::string& filename,
               int colIndex,
               const std::string& value) {
    return !findRow(filename, colIndex, value).empty();
}