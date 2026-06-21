#ifndef REPORTS_H
#define REPORTS_H

#include <string>

void printMeritList();

void printAttendanceDefaulters();

void printFeeDefaulters();

void printSemesterResult(const std::string& semester);

void printDepartmentSummary();

void exportReportToFile(int reportChoice, const std::string& semester);

#endif