#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include <vector>

void addStudent(const std::string& roll,
                const std::string& name,
                const std::string& dept,
                const std::string& semester,
                const std::string& cgpa);

std::vector<std::string> searchByRoll(const std::string& roll);

std::vector<std::vector<std::string>> searchByName(const std::string& nameQuery);

void updateStudent(const std::string& roll, int colIndex, const std::string& newValue);

void softDelete(const std::string& roll);

std::vector<std::vector<std::string>> listActiveStudents();

#endif