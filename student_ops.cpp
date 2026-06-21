#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
#include <string>
#include <vector>

static const int STU_ROLL   = 0;
static const int STU_NAME   = 1;
static const int STU_DEPT   = 2;
static const int STU_SEM    = 3;
static const int STU_CGPA   = 4;
static const int STU_STATUS = 5;

static std::vector<std::string> studentHeader() {
    std::vector<std::string> h;
    h.push_back("roll_no");
    h.push_back("name");
    h.push_back("department");
    h.push_back("semester");
    h.push_back("cgpa");
    h.push_back("status");
    return h;
}

static bool isValidRollFormat(const std::string& roll) {
    if (roll.length() != 12) return false;
    if (roll.substr(0, 5) != "BSAI-") return false;
    if (roll[7] != '-') return false;

    for (int i = 5; i <= 6; i++)
        if (roll[i] < '0' || roll[i] > '9') return false;

    for (int i = 8; i <= 10; i++)
        if (roll[i] < '0' || roll[i] > '9') return false;

    return true;
}

static bool nameHasDigit(const std::string& name) {
    for (int i = 0; i < (int)name.length(); i++)
        if (name[i] >= '0' && name[i] <= '9') return true;
    return false;
}

static bool isValidCGPA(const std::string& cgpaStr) {
    bool dotSeen = false;
    for (int i = 0; i < (int)cgpaStr.length(); i++) {
        char c = cgpaStr[i];
        if (c == '.') {
            if (dotSeen) return false;
            dotSeen = true;
        } else if (c < '0' || c > '9') {
            return false;
        }
    }
    double val = 0.0;
    double dec = 0.1;
    bool afterDot = false;
    for (int i = 0; i < (int)cgpaStr.length(); i++) {
        if (cgpaStr[i] == '.') { afterDot = true; continue; }
        if (!afterDot)
            val = val * 10 + (cgpaStr[i] - '0');
        else {
            val += (cgpaStr[i] - '0') * dec;
            dec *= 0.1;
        }
    }
    return val >= 0.0 && val <= 4.0;
}

static bool containsSubstring(const std::string& haystack, const std::string& needle) {
    if (needle.length() > haystack.length()) return false;
    for (int i = 0; i <= (int)(haystack.length() - needle.length()); i++) {
        bool match = true;
        for (int j = 0; j < (int)needle.length(); j++) {
            char a = haystack[i + j];
            char b = needle[j];
            if (a >= 'A' && a <= 'Z') a = a - 'A' + 'a';
            if (b >= 'A' && b <= 'Z') b = b - 'A' + 'a';
            if (a != b) { match = false; break; }
        }
        if (match) return true;
    }
    return false;
}

void addStudent(const std::string& roll,
                const std::string& name,
                const std::string& dept,
                const std::string& semester,
                const std::string& cgpa) {
    if (!isValidRollFormat(roll)) {
        std::cout << "Error: Roll number must be in format BSAI-YY-XXX.\n";
        return;
    }
    if (rowExists("students.txt", STU_ROLL, roll)) {
        std::cout << "Error: Student with roll " << roll << " already exists.\n";
        return;
    }
    if (nameHasDigit(name)) {
        std::cout << "Error: Name must not contain digits.\n";
        return;
    }
    if (!isValidCGPA(cgpa)) {
        std::cout << "Error: CGPA must be a number between 0.0 and 4.0.\n";
        return;
    }

    std::vector<std::string> row;
    row.push_back(roll);
    row.push_back(name);
    row.push_back(dept);
    row.push_back(semester);
    row.push_back(cgpa);
    row.push_back("active");

    appendTXT("students.txt", row);
    std::cout << "Student " << roll << " added successfully.\n";
}

std::vector<std::string> searchByRoll(const std::string& roll) {
    return findRow("students.txt", STU_ROLL, roll);
}

std::vector<std::vector<std::string>> searchByName(const std::string& nameQuery) {
    std::vector<std::vector<std::string>> allRows = readTXT("students.txt");
    std::vector<std::vector<std::string>> result;

    for (int i = 0; i < (int)allRows.size(); i++) {
        if ((int)allRows[i].size() <= STU_NAME) continue;
        if (containsSubstring(allRows[i][STU_NAME], nameQuery))
            result.push_back(allRows[i]);
    }
    return result;
}

void updateStudent(const std::string& roll, int colIndex, const std::string& newValue) {
    if (colIndex == STU_ROLL) {
        std::cout << "Error: Roll number cannot be updated.\n";
        return;
    }

    std::vector<std::vector<std::string>> rows = readTXT("students.txt");
    bool found = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() > STU_ROLL && rows[i][STU_ROLL] == roll) {
            if (colIndex < (int)rows[i].size()) {
                rows[i][colIndex] = newValue;
                found = true;
            }
            break;
        }
    }

    if (!found) {
        std::cout << "Error: Student " << roll << " not found.\n";
        return;
    }

    writeTXT("students.txt", studentHeader(), rows);
    std::cout << "Student " << roll << " updated successfully.\n";
}

void softDelete(const std::string& roll) {
    updateStudent(roll, STU_STATUS, "inactive");
}

std::vector<std::vector<std::string>> listActiveStudents() {
    std::vector<std::vector<std::string>> rows = readTXT("students.txt");
    std::vector<std::vector<std::string>> active;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() > STU_STATUS && rows[i][STU_STATUS] == "active")
            active.push_back(rows[i]);
    }

    for (int i = 0; i < (int)active.size() - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j][STU_ROLL] < active[minIdx][STU_ROLL])
                minIdx = j;
        }
        if (minIdx != i) {
            std::vector<std::string> temp = active[i];
            active[i] = active[minIdx];
            active[minIdx] = temp;
        }
    }

    return active;
}