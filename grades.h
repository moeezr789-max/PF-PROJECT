#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>

struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

void enterMarks(const std::string& roll,
                const std::string& courseCode,
                const std::string& semester);

double bestThreeOfFive(double marks[], int n);

double computeWeightedTotal(double quizPct,
                            double asgnPct,
                            double midPct,
                            double finalPct);

std::string getLetterGrade(double total);

double computeGPA(const std::string& roll, const std::string& semester);

Stats computeClassStats(const std::string& courseCode,
                        const std::string& semester);

void applyAttendancePenalty(const std::string& roll,
                            const std::string& courseCode,
                            const std::string& semester);

#endif