#include "attendance.h"
#include "filehandler.h"
#include "course_ops.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

static const int ATT_LOG_ID  = 0;
static const int ATT_ROLL    = 1;
static const int ATT_COURSE  = 2;
static const int ATT_DATE    = 3;
static const int ATT_STATUS  = 4;

static const int STU_ROLL    = 0;
static const int STU_NAME    = 1;

static std::vector<std::vector<std::string>> g_attendanceBackup;
static bool g_backupExists = false;

static std::string intToZeroPadded(int value, int width) {
    std::string result = "";
    if (value == 0) result = "0";
    int temp = value;
    while (temp > 0) {
        result = (char)('0' + temp % 10) + result;
        temp /= 10;
    }
    while ((int)result.length() < width) result = "0" + result;
    return result;
}

static int parseLogIdNum(const std::string& logId) {
    if (logId.length() < 2) return 0;
    std::string digits = logId.substr(1);
    int value = 0;
    for (int i = 0; i < (int)digits.length(); i++) {
        if (digits[i] >= '0' && digits[i] <= '9')
            value = value * 10 + (digits[i] - '0');
    }
    return value;
}

static std::string nextLogId(const std::vector<std::vector<std::string>>& rows) {
    int maxNum = 0;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() <= (size_t)ATT_LOG_ID) continue;
        int num = parseLogIdNum(rows[i][ATT_LOG_ID]);
        if (num > maxNum) maxNum = num;
    }
    return "L" + intToZeroPadded(maxNum + 1, 5);
}

static std::string toUpper(const std::string& s) {
    std::string result = s;
    for (int i = 0; i < (int)result.length(); i++) {
        if (result[i] >= 'a' && result[i] <= 'z')
            result[i] = result[i] - 'a' + 'A';
    }
    return result;
}

static std::string getStudentName(const std::string& roll) {
    std::vector<std::string> row = findRow("students.txt", STU_ROLL, roll);
    if ((int)row.size() > STU_NAME) return row[STU_NAME];
    return roll;
}

static std::vector<std::string> attendanceHeader() {
    std::vector<std::string> h;
    h.push_back("log_id");
    h.push_back("roll_no");
    h.push_back("course_code");
    h.push_back("session_date");
    h.push_back("status");
    return h;
}

void markAttendance(const std::string& courseCode, const std::string& date) {
    std::vector<std::string> enrolledRolls = listEnrolledStudents(courseCode);

    if (enrolledRolls.empty()) {
        std::cout << "No active students enrolled in course: " << courseCode << "\n";
        return;
    }

    g_attendanceBackup = readTXT("attendance_log.txt");
    g_backupExists     = true;

    std::string currentLogId = nextLogId(g_attendanceBackup);
    int currentIdNum = parseLogIdNum(currentLogId);

    std::string divider(58, '=');
    std::cout << "\n" << divider << "\n";
    std::cout << "  Marking Attendance | Course: " << courseCode
              << " | Date: " << date << "\n";
    std::cout << divider << "\n";
    std::cout << std::left
              << std::setw(16) << "Roll No"
              << std::setw(26) << "Name"
              << "Status (P/A/L)\n";
    std::cout << std::string(58, '-') << "\n";

    for (int i = 0; i < (int)enrolledRolls.size(); i++) {
        std::string roll   = enrolledRolls[i];
        std::string name   = getStudentName(roll);
        std::string status = "";

        while (true) {
            std::cout << std::left
                      << std::setw(16) << roll
                      << std::setw(26) << name
                      << ": ";
            std::cin >> status;
            status = toUpper(status);

            if (status == "P" || status == "A" || status == "L") break;

            std::cout << "  [!] Invalid input. Enter P (Present), "
                         "A (Absent), or L (Late).\n";
        }

        std::string logId = "L" + intToZeroPadded(currentIdNum, 5);
        currentIdNum++;

        std::vector<std::string> newRow;
        newRow.push_back(logId);
        newRow.push_back(roll);
        newRow.push_back(courseCode);
        newRow.push_back(date);
        newRow.push_back(status);

        appendTXT("attendance_log.txt", newRow);
    }

    std::cout << std::string(58, '-') << "\n";
    std::cout << "  Attendance saved for " << (int)enrolledRolls.size()
              << " student(s). [Undo available this session]\n\n";
}

double getAttendancePct(const std::string& roll, const std::string& courseCode) {
    std::vector<std::vector<std::string>> allRows = readTXT("attendance_log.txt");

    int totalSessions = 0;
    int presentCount  = 0;
    int lateCount     = 0;

    for (int i = 0; i < (int)allRows.size(); i++) {
        if ((int)allRows[i].size() <= ATT_STATUS) continue;
        if (allRows[i][ATT_ROLL]   != roll)       continue;
        if (allRows[i][ATT_COURSE] != courseCode)   continue;

        totalSessions++;

        std::string s = allRows[i][ATT_STATUS];
        if      (s == "P") presentCount++;
        else if (s == "L") lateCount++;
    }

    if (totalSessions == 0) return 0.0;

    return (presentCount + 0.5 * lateCount) / (double)totalSessions * 100.0;
}

std::vector<AttendanceShortage> getShortageList() {
    std::vector<std::vector<std::string>> allRows = readTXT("attendance_log.txt");

    std::vector<std::string> seenRolls;
    std::vector<std::string> seenCourses;

    for (int i = 0; i < (int)allRows.size(); i++) {
        if ((int)allRows[i].size() <= ATT_STATUS) continue;

        std::string roll   = allRows[i][ATT_ROLL];
        std::string course = allRows[i][ATT_COURSE];

        bool alreadySeen = false;
        for (int j = 0; j < (int)seenRolls.size(); j++) {
            if (seenRolls[j] == roll && seenCourses[j] == course) {
                alreadySeen = true;
                break;
            }
        }

        if (!alreadySeen) {
            seenRolls.push_back(roll);
            seenCourses.push_back(course);
        }
    }

    std::vector<AttendanceShortage> shortageList;

    for (int i = 0; i < (int)seenRolls.size(); i++) {
        double pct = getAttendancePct(seenRolls[i], seenCourses[i]);
        if (pct < 75.0) {
            AttendanceShortage entry;
            entry.roll         = seenRolls[i];
            entry.courseCode = seenCourses[i];
            entry.percentage = pct;
            shortageList.push_back(entry);
        }
    }

    return shortageList;
}

bool undoLastSession() {
    if (!g_backupExists) {
        std::cout << "[!] No backup found. "
                     "Undo is only available immediately after marking attendance.\n";
        return false;
    }

    writeTXT("attendance_log.txt", attendanceHeader(), g_attendanceBackup);

    g_attendanceBackup.clear();
    g_backupExists = false;

    std::cout << "  Last session undone. attendance_log.txt restored to "
                 "pre-session state.\n";
    return true;
}

void printDailySheet(const std::string& courseCode, const std::string& date) {
    std::vector<std::vector<std::string>> allLogs = readTXT("attendance_log.txt");

    std::vector<std::string> rolls;
    std::vector<std::string> statuses;

    for (int i = 0; i < (int)allLogs.size(); i++) {
        if ((int)allLogs[i].size() <= ATT_STATUS) continue;
        if (allLogs[i][ATT_COURSE] != courseCode) continue;
        if (allLogs[i][ATT_DATE]   != date)       continue;

        rolls.push_back(allLogs[i][ATT_ROLL]);
        statuses.push_back(allLogs[i][ATT_STATUS]);
    }

    if (rolls.empty()) {
        std::cout << "[!] No attendance record found for course "
                  << courseCode << " on " << date << ".\n";
        return;
    }

    int presentCount = 0, absentCount = 0, lateCount = 0;
    for (int i = 0; i < (int)statuses.size(); i++) {
        if      (statuses[i] == "P") presentCount++;
        else if (statuses[i] == "A") absentCount++;
        else if (statuses[i] == "L") lateCount++;
    }

    std::string border(60, '=');
    std::string divider(60, '-');

    std::cout << "\n" << border << "\n";
    std::cout << "  Daily Attendance Sheet\n";
    std::cout << "  Course : " << courseCode << "\n";
    std::cout << "  Date   : " << date       << "\n";
    std::cout << border << "\n";

    std::cout << std::left
              << std::setw(5)  << "#"
              << std::setw(16) << "Roll No"
              << std::setw(26) << "Name"
              << std::setw(10) << "Status"
              << "\n";
    std::cout << divider << "\n";

    for (int i = 0; i < (int)rolls.size(); i++) {
        std::string name         = getStudentName(rolls[i]);
        std::string statusLabel = statuses[i];

        if      (statusLabel == "P") statusLabel = "Present";
        else if (statusLabel == "A") statusLabel = "Absent";
        else if (statusLabel == "L") statusLabel = "Late";

        std::cout << std::left
                  << std::setw(5)  << (i + 1)
                  << std::setw(16) << rolls[i]
                  << std::setw(26) << name
                  << std::setw(10) << statusLabel
                  << "\n";
    }

    std::cout << border << "\n";
    std::cout << "  Total: " << (int)rolls.size()
              << "  |  Present: " << presentCount
              << "  |  Absent: "  << absentCount
              << "  |  Late: "    << lateCount
              << "\n" << border << "\n\n";
}