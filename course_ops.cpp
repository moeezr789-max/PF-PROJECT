#include "course_ops.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <string>
#include <vector>

static const int CRS_CODE    = 0;
static const int CRS_NAME    = 1;
static const int CRS_CREDITS = 2;
static const int CRS_INST    = 3;
static const int CRS_CAP     = 4;
static const int CRS_ENRLD   = 5;
static const int CRS_PREREQ  = 6;

static const int ENR_ID      = 0;
static const int ENR_ROLL    = 1;
static const int ENR_COURSE  = 2;
static const int ENR_SEM     = 3;
static const int ENR_DATE    = 4;
static const int ENR_STATUS  = 5;

static const int ATT_ROLL    = 1;
static const int ATT_COURSE  = 2;

static const int GRD_ROLL    = 0;
static const int GRD_COURSE  = 1;
static const int GRD_GRADE   = 8;

static const int STU_STATUS  = 5;

static std::vector<std::string> enrollmentHeader() {
    std::vector<std::string> h;
    h.push_back("enrollment_id");
    h.push_back("roll_no");
    h.push_back("course_code");
    h.push_back("semester");
    h.push_back("enrollment_date");
    h.push_back("status");
    return h;
}

static std::vector<std::string> courseHeader() {
    std::vector<std::string> h;
    h.push_back("course_code");
    h.push_back("course_name");
    h.push_back("credit_hours");
    h.push_back("instructor");
    h.push_back("capacity");
    h.push_back("enrolled");
    h.push_back("prerequisite");
    return h;
}

static std::string nextEnrollmentId(const std::vector<std::vector<std::string>>& rows) {
    int maxNum = 0;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i].size() <= (size_t)ENR_ID) continue;
        std::string id = rows[i][ENR_ID];
        if (id.length() < 2) continue;
        std::string digits = id.substr(1);
        int val = 0;
        for (int j = 0; j < (int)digits.length(); j++) {
            if (digits[j] >= '0' && digits[j] <= '9')
                val = val * 10 + (digits[j] - '0');
        }
        if (val > maxNum) maxNum = val;
    }
    maxNum++;
    std::string num = "";
    int temp = maxNum;
    if (temp == 0) num = "0";
    while (temp > 0) { num = (char)('0' + temp % 10) + num; temp /= 10; }
    while ((int)num.length() < 4) num = "0" + num;
    return "E" + num;
}

static int stringToInt(const std::string& s) {
    int val = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= '0' && s[i] <= '9')
            val = val * 10 + (s[i] - '0');
    }
    return val;
}

static std::string intToString(int val) {
    if (val == 0) return "0";
    std::string s = "";
    while (val > 0) { s = (char)('0' + val % 10) + s; val /= 10; }
    return s;
}

bool checkPrerequisite(const std::string& roll, const std::string& courseCode) {
    std::vector<std::string> course = findRow("courses.txt", CRS_CODE, courseCode);
    if (course.size() <= (size_t)CRS_PREREQ) return false;

    std::string prereq = course[CRS_PREREQ];
    if (prereq == "NONE") return true;

    std::vector<std::vector<std::string>> grades = readTXT("grades.txt");
    for (int i = 0; i < (int)grades.size(); i++) {
        if ((int)grades[i].size() <= GRD_GRADE) continue;
        if (grades[i][GRD_ROLL]   != roll)   continue;
        if (grades[i][GRD_COURSE] != prereq)  continue;
        if (grades[i][GRD_GRADE]  != "F")     return true;
    }
    return false;
}

int getCreditLoad(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> enrollments = readTXT("enrollments.txt");
    std::vector<std::vector<std::string>> courses     = readTXT("courses.txt");

    int totalCredits = 0;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if ((int)enrollments[i].size() <= ENR_STATUS) continue;
        if (enrollments[i][ENR_ROLL]   != roll)       continue;
        if (enrollments[i][ENR_SEM]    != semester)   continue;
        if (enrollments[i][ENR_STATUS] != "active")   continue;

        std::string code = enrollments[i][ENR_COURSE];

        for (int j = 0; j < (int)courses.size(); j++) {
            if ((int)courses[j].size() <= CRS_CREDITS) continue;
            if (courses[j][CRS_CODE] != code) continue;
            totalCredits += stringToInt(courses[j][CRS_CREDITS]);
            break;
        }
    }

    return totalCredits;
}

EnrollResult enrollStudent(const std::string& roll,
                           const std::string& courseCode,
                           const std::string& semester) {
    std::vector<std::string> student = searchByRoll(roll);
    if (student.empty() || student[STU_STATUS] != "active")
        return ENROLL_STUDENT_NOT_ACTIVE;

    std::vector<std::string> course = findRow("courses.txt", CRS_CODE, courseCode);
    if (course.empty())
        return ENROLL_COURSE_NOT_FOUND;

    int capacity = stringToInt(course[CRS_CAP]);
    int enrolled = stringToInt(course[CRS_ENRLD]);
    if (enrolled >= capacity)
        return ENROLL_NO_SEATS;

    std::vector<std::vector<std::string>> enrollments = readTXT("enrollments.txt");
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if ((int)enrollments[i].size() <= ENR_STATUS) continue;
        if (enrollments[i][ENR_ROLL]   != roll)       continue;
        if (enrollments[i][ENR_COURSE] != courseCode)  continue;
        if (enrollments[i][ENR_STATUS] == "active")
            return ENROLL_ALREADY_ENROLLED;
    }

    std::vector<std::string> courseRow = findRow("courses.txt", CRS_CODE, courseCode);
    int credits = stringToInt(courseRow[CRS_CREDITS]);
    if (getCreditLoad(roll, semester) + credits > 21)
        return ENROLL_CREDIT_OVERLOAD;

    if (!checkPrerequisite(roll, courseCode))
        return ENROLL_PREREQUISITE_NOT_MET;

    std::string newId = nextEnrollmentId(enrollments);
    std::vector<std::string> newRow;
    newRow.push_back(newId);
    newRow.push_back(roll);
    newRow.push_back(courseCode);
    newRow.push_back(semester);
    newRow.push_back("01-01-2024");
    newRow.push_back("active");
    appendTXT("enrollments.txt", newRow);

    std::vector<std::vector<std::string>> allCourses = readTXT("courses.txt");
    for (int i = 0; i < (int)allCourses.size(); i++) {
        if ((int)allCourses[i].size() <= CRS_ENRLD) continue;
        if (allCourses[i][CRS_CODE] != courseCode) continue;
        int cur = stringToInt(allCourses[i][CRS_ENRLD]);
        allCourses[i][CRS_ENRLD] = intToString(cur + 1);
        break;
    }
    writeTXT("courses.txt", courseHeader(), allCourses);

    return ENROLL_SUCCESS;
}

bool dropCourse(const std::string& roll,
                const std::string& courseCode,
                const std::string& semester) {
    std::vector<std::vector<std::string>> attRows = readTXT("attendance_log.txt");
    for (int i = 0; i < (int)attRows.size(); i++) {
        if ((int)attRows[i].size() <= ATT_COURSE) continue;
        if (attRows[i][ATT_ROLL]   == roll &&
            attRows[i][ATT_COURSE] == courseCode) {
            std::cout << "Error: Cannot drop course. Attendance already recorded.\n";
            return false;
        }
    }

    std::vector<std::vector<std::string>> enrollments = readTXT("enrollments.txt");
    bool found = false;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if ((int)enrollments[i].size() <= ENR_STATUS) continue;
        if (enrollments[i][ENR_ROLL]   != roll)       continue;
        if (enrollments[i][ENR_COURSE] != courseCode)  continue;
        if (enrollments[i][ENR_SEM]    != semester)   continue;
        if (enrollments[i][ENR_STATUS] != "active")   continue;

        enrollments[i][ENR_STATUS] = "dropped";
        found = true;
        break;
    }

    if (!found) {
        std::cout << "Error: Active enrollment not found.\n";
        return false;
    }

    writeTXT("enrollments.txt", enrollmentHeader(), enrollments);

    std::vector<std::vector<std::string>> allCourses = readTXT("courses.txt");
    for (int i = 0; i < (int)allCourses.size(); i++) {
        if ((int)allCourses[i].size() <= CRS_ENRLD) continue;
        if (allCourses[i][CRS_CODE] != courseCode) continue;
        int cur = stringToInt(allCourses[i][CRS_ENRLD]);
        if (cur > 0) allCourses[i][CRS_ENRLD] = intToString(cur - 1);
        break;
    }
    writeTXT("courses.txt", courseHeader(), allCourses);

    std::cout << "Course " << courseCode << " dropped for " << roll << ".\n";
    return true;
}

std::vector<std::string> listEnrolledStudents(const std::string& courseCode) {
    std::vector<std::vector<std::string>> enrollments = readTXT("enrollments.txt");
    std::vector<std::string> rolls;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if ((int)enrollments[i].size() <= ENR_STATUS) continue;
        if (enrollments[i][ENR_COURSE] != courseCode)  continue;
        if (enrollments[i][ENR_STATUS] != "active")   continue;

        std::string roll = enrollments[i][ENR_ROLL];
        bool duplicate = false;
        for (int j = 0; j < (int)rolls.size(); j++) {
            if (rolls[j] == roll) { duplicate = true; break; }
        }
        if (!duplicate) rolls.push_back(roll);
    }

    return rolls;
}