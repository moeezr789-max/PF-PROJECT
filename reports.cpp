#include "reports.h"
#include "filehandler.h"
#include "student_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

static const int STU_ROLL   = 0;
static const int STU_NAME   = 1;
static const int STU_DEPT   = 2;
static const int STU_SEM    = 3;
static const int STU_CGPA   = 4;
static const int STU_STATUS = 5;

static const int GRD_ROLL   = 0;
static const int GRD_COURSE = 1;
static const int GRD_SEM    = 2;
static const int GRD_TOTAL  = 7;
static const int GRD_GRADE  = 8;

static double stringToDouble(const std::string& s) {
    double intPart  = 0.0;
    double fracPart = 0.0;
    double dec      = 0.1;
    bool   afterDot = false;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] == '.') { afterDot = true; continue; }
        if (s[i] < '0' || s[i] > '9') continue;
        if (!afterDot) intPart = intPart * 10 + (s[i] - '0');
        else { fracPart += (s[i] - '0') * dec; dec *= 0.1; }
    }
    return intPart + fracPart;
}

static std::string doubleToString(double val, int decimals) {
    if (val < 0) return "-" + doubleToString(-val, decimals);
    int intPart = (int)val;
    double frac = val - intPart;
    for (int i = 0; i < decimals; i++) frac *= 10;
    int fracInt = (int)(frac + 0.5);
    std::string intStr = "";
    int temp = intPart;
    if (temp == 0) intStr = "0";
    while (temp > 0) { intStr = (char)('0' + temp % 10) + intStr; temp /= 10; }
    if (decimals == 0) return intStr;
    std::string fracStr = "";
    temp = fracInt;
    for (int i = 0; i < decimals; i++) { fracStr = (char)('0' + temp % 10) + fracStr; temp /= 10; }
    return intStr + "." + fracStr;
}

void printMeritList() {
    std::vector<std::vector<std::string>> students = listActiveStudents();

    for (int i = 0; i < (int)students.size() - 1; i++) {
        for (int j = 0; j < (int)students.size() - 1 - i; j++) {
            double cgpaA = stringToDouble(students[j][STU_CGPA]);
            double cgpaB = stringToDouble(students[j + 1][STU_CGPA]);
            if (cgpaA < cgpaB) {
                std::vector<std::string> tmp = students[j];
                students[j]     = students[j + 1];
                students[j + 1] = tmp;
            }
        }
    }

    std::string border(62, '=');
    std::string divider(62, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(38) << std::right << "MERIT LIST\n";
    std::cout << border << "\n";
    std::cout << std::left
              << std::setw(6)  << "Rank"
              << std::setw(16) << "Roll No"
              << std::setw(26) << "Name"
              << std::setw(28) << "Dept"
              << std::setw(6)  << "CGPA"
              << "\n";
    std::cout << divider << "\n";

    for (int i = 0; i < (int)students.size(); i++) {
        if ((int)students[i].size() <= STU_CGPA) continue;
        std::cout << std::left
                  << std::setw(6)  << (i + 1)
                  << std::setw(16) << students[i][STU_ROLL]
                  << std::setw(26) << students[i][STU_NAME]
                  << std::setw(28) << students[i][STU_DEPT]
                  << std::setw(6)  << students[i][STU_CGPA]
                  << "\n";
    }

    std::cout << border << "\n";
    std::cout << "Total Active Students: " << students.size() << "\n\n";
}

void printAttendanceDefaulters() {
    std::vector<AttendanceShortage> shortages = getShortageList();

    std::string border(62, '=');
    std::string divider(62, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(44) << std::right << "ATTENDANCE DEFAULTERS\n";
    std::cout << border << "\n";
    std::cout << std::left
              << std::setw(16) << "Roll No"
              << std::setw(24) << "Name"
              << std::setw(10) << "Course"
              << std::setw(12) << "Attendance%"
              << "\n";
    std::cout << divider << "\n";

    if (shortages.empty()) {
        std::cout << "  No attendance defaulters found.\n";
    } else {
        for (int i = 0; i < (int)shortages.size(); i++) {
            std::vector<std::string> stu = searchByRoll(shortages[i].roll);
            std::string name = (stu.size() > (size_t)STU_NAME) ? stu[STU_NAME] : shortages[i].roll;
            std::cout << std::left
                      << std::setw(16) << shortages[i].roll
                      << std::setw(24) << name
                      << std::setw(10) << shortages[i].courseCode
                      << std::setw(12) << doubleToString(shortages[i].percentage, 1) + "%"
                      << "\n";
        }
    }

    std::cout << border << "\n";
    std::cout << "Total Defaulters: " << shortages.size() << "\n\n";
}

void printFeeDefaulters() {
    std::vector<FeeDefaulter> defaulters = getDefaulters();

    std::string border(66, '=');
    std::string divider(66, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(44) << std::right << "FEE DEFAULTERS\n";
    std::cout << border << "\n";
    std::cout << std::left
              << std::setw(16) << "Roll No"
              << std::setw(24) << "Name"
              << std::setw(10) << "Semester"
              << std::setw(12) << "Balance"
              << std::setw(8)  << "Weeks Late"
              << "\n";
    std::cout << divider << "\n";

    if (defaulters.empty()) {
        std::cout << "  No fee defaulters found.\n";
    } else {
        for (int i = 0; i < (int)defaulters.size(); i++) {
            std::vector<std::string> stu = searchByRoll(defaulters[i].roll);
            std::string name = (stu.size() > (size_t)STU_NAME) ? stu[STU_NAME] : defaulters[i].roll;
            std::cout << std::left
                      << std::setw(16) << defaulters[i].roll
                      << std::setw(24) << name
                      << std::setw(10) << defaulters[i].semester
                      << std::setw(12) << "Rs." + doubleToString(defaulters[i].balance, 0)
                      << std::setw(8)  << defaulters[i].weeksOverdue
                      << "\n";
        }
    }

    std::cout << border << "\n";
    std::cout << "Total Fee Defaulters: " << defaulters.size() << "\n\n";
}

void printSemesterResult(const std::string& semester) {
    std::vector<std::vector<std::string>> students = listActiveStudents();
    std::vector<std::vector<std::string>> grades   = readTXT("grades.txt");

    std::string border(72, '=');
    std::string divider(72, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(46) << std::right
              << "SEMESTER RESULT SHEET | Semester: " + semester + "\n";
    std::cout << border << "\n";

    for (int i = 0; i < (int)students.size(); i++) {
        if ((int)students[i].size() <= STU_NAME) continue;

        std::string roll = students[i][STU_ROLL];
        std::string name = students[i][STU_NAME];

        std::cout << "\n  Student : " << roll << "  |  " << name << "\n";
        std::cout << divider << "\n";
        std::cout << std::left
                  << std::setw(12) << "Course"
                  << std::setw(10) << "Total"
                  << std::setw(8)  << "Grade"
                  << std::setw(14) << "Attendance%"
                  << "\n";
        std::cout << std::string(44, '-') << "\n";

        bool hasCourses = false;
        for (int j = 0; j < (int)grades.size(); j++) {
            if ((int)grades[j].size() <= GRD_GRADE) continue;
            if (grades[j][GRD_ROLL] != roll)         continue;
            if (grades[j][GRD_SEM]  != semester)     continue;

            std::string course = grades[j][GRD_COURSE];
            double      attPct = getAttendancePct(roll, course);
            std::string attStr = doubleToString(attPct, 1) + "%";
            if (attPct < 75.0) attStr += " [!]";

            std::cout << std::left
                      << std::setw(12) << course
                      << std::setw(10) << grades[j][GRD_TOTAL]
                      << std::setw(8)  << grades[j][GRD_GRADE]
                      << std::setw(14) << attStr
                      << "\n";
            hasCourses = true;
        }

        if (!hasCourses) {
            std::cout << "  No grade records for this semester.\n";
        } else {
            double gpa = computeGPA(roll, semester);
            std::cout << std::string(44, '-') << "\n";
            std::cout << "  Semester GPA: " << doubleToString(gpa, 2) << "\n";
        }
    }

    std::cout << "\n" << border << "\n\n";
}

void printDepartmentSummary() {
    std::vector<std::vector<std::string>> students = listActiveStudents();
    std::vector<std::vector<std::string>> grades   = readTXT("grades.txt");

    std::vector<std::string> depts;
    std::vector<int>         counts;
    std::vector<double>      cgpaSums;
    std::vector<int>         passCounts;

    for (int i = 0; i < (int)students.size(); i++) {
        if ((int)students[i].size() <= STU_CGPA) continue;
        std::string dept = students[i][STU_DEPT];

        int deptIdx = -1;
        for (int j = 0; j < (int)depts.size(); j++) {
            if (depts[j] == dept) { deptIdx = j; break; }
        }
        if (deptIdx == -1) {
            depts.push_back(dept);
            counts.push_back(0);
            cgpaSums.push_back(0.0);
            passCounts.push_back(0);
            deptIdx = (int)depts.size() - 1;
        }

        counts[deptIdx]++;
        cgpaSums[deptIdx] += stringToDouble(students[i][STU_CGPA]);

        std::string roll = students[i][STU_ROLL];
        bool passed = true;
        bool hasGrades = false;
        for (int j = 0; j < (int)grades.size(); j++) {
            if ((int)grades[j].size() <= GRD_GRADE) continue;
            if (grades[j][GRD_ROLL] != roll) continue;
            hasGrades = true;
            if (grades[j][GRD_GRADE] == "F") { passed = false; break; }
        }
        if (hasGrades && passed) passCounts[deptIdx]++;
    }

    std::string border(62, '=');
    std::string divider(62, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(42) << std::right << "DEPARTMENT SUMMARY\n";
    std::cout << border << "\n";
    std::cout << std::left
              << std::setw(28) << "Department"
              << std::setw(10) << "Students"
              << std::setw(12) << "Avg CGPA"
              << std::setw(10) << "Pass Rate"
              << "\n";
    std::cout << divider << "\n";

    for (int i = 0; i < (int)depts.size(); i++) {
        double avgCgpa   = (counts[i] > 0) ? cgpaSums[i] / counts[i] : 0.0;
        double passRate  = (counts[i] > 0) ? (passCounts[i] * 100.0 / counts[i]) : 0.0;

        std::cout << std::left
                  << std::setw(28) << depts[i]
                  << std::setw(10) << counts[i]
                  << std::setw(12) << doubleToString(avgCgpa, 2)
                  << std::setw(10) << doubleToString(passRate, 1) + "%"
                  << "\n";
    }

    std::cout << border << "\n\n";
}

void exportReportToFile(int reportChoice, const std::string& semester) {
    std::string filename = "";
    if      (reportChoice == 1) filename = "report_merit_list.txt";
    else if (reportChoice == 2) filename = "report_attendance_defaulters.txt";
    else if (reportChoice == 3) filename = "report_fee_defaulters.txt";
    else if (reportChoice == 4) filename = "report_semester_result.txt";
    else if (reportChoice == 5) filename = "report_department_summary.txt";
    else {
        std::cout << "Invalid report choice.\n";
        return;
    }

    std::ofstream outFile(filename.c_str());
    if (!outFile.is_open()) {
        std::cout << "Error: Could not create file " << filename << "\n";
        return;
    }

    std::streambuf* origBuf = std::cout.rdbuf(outFile.rdbuf());

    if      (reportChoice == 1) printMeritList();
    else if (reportChoice == 2) printAttendanceDefaulters();
    else if (reportChoice == 3) printFeeDefaulters();
    else if (reportChoice == 4) printSemesterResult(semester);
    else if (reportChoice == 5) printDepartmentSummary();

    std::cout.rdbuf(origBuf);
    outFile.close();

    std::cout << "Report saved to: " << filename << "\n";
}