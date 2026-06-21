#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>

struct AttendanceShortage {
    std::string roll;
    std::string courseCode;
    double percentage;
};

void markAttendance(const std::string& courseCode, const std::string& date);

double getAttendancePct(const std::string& roll, const std::string& courseCode);

std::vector<AttendanceShortage> getShortageList();

bool undoLastSession();

void printDailySheet(const std::string& courseCode, const std::string& date);

#endif