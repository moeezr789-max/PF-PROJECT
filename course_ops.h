#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include <vector>

enum EnrollResult {
    ENROLL_SUCCESS,
    ENROLL_STUDENT_NOT_ACTIVE,
    ENROLL_COURSE_NOT_FOUND,
    ENROLL_NO_SEATS,
    ENROLL_ALREADY_ENROLLED,
    ENROLL_CREDIT_OVERLOAD,
    ENROLL_PREREQUISITE_NOT_MET
};

EnrollResult enrollStudent(const std::string& roll,
                           const std::string& courseCode,
                           const std::string& semester);

bool dropCourse(const std::string& roll,
                const std::string& courseCode,
                const std::string& semester);

int getCreditLoad(const std::string& roll, const std::string& semester);

bool checkPrerequisite(const std::string& roll, const std::string& courseCode);

std::vector<std::string> listEnrolledStudents(const std::string& courseCode);

#endif