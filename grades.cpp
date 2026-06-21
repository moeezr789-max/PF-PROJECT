#include "grades.h"
#include "filehandler.h"
#include "student_ops.h"
#include "attendance.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

static const int GRD_ROLL    = 0;
static const int GRD_COURSE  = 1;
static const int GRD_SEM     = 2;
static const int GRD_QUIZ    = 3;
static const int GRD_ASGN    = 4;
static const int GRD_MID     = 5;
static const int GRD_FINAL   = 6;
static const int GRD_TOTAL   = 7;
static const int GRD_GRADE   = 8;

static const int CRS_CODE    = 0;
static const int CRS_CREDITS = 2;

static const int ENR_ROLL    = 1;
static const int ENR_COURSE  = 2;
static const int ENR_SEM     = 3;
static const int ENR_STATUS  = 5;

static std::vector<std::string> gradesHeader() {
    std::vector<std::string> h;
    h.push_back("roll_no");
    h.push_back("course_code");
    h.push_back("semester");
    h.push_back("quiz_pct");
    h.push_back("asgn_pct");
    h.push_back("mid_pct");
    h.push_back("final_pct");
    h.push_back("total");
    h.push_back("letter_grade");
    return h;
}

static double stringToDouble(const std::string& s) {
    double intPart  = 0.0;
    double fracPart = 0.0;
    double dec      = 0.1;
    bool   afterDot = false;
    bool   negative = false;
    int    start    = 0;

    if (!s.empty() && s[0] == '-') { negative = true; start = 1; }

    for (int i = start; i < (int)s.length(); i++) {
        if (s[i] == '.') { afterDot = true; continue; }
        if (s[i] < '0' || s[i] > '9') continue;
        if (!afterDot)
            intPart = intPart * 10 + (s[i] - '0');
        else {
            fracPart += (s[i] - '0') * dec;
            dec *= 0.1;
        }
    }
    double result = intPart + fracPart;
    return negative ? -result : result;
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

    std::string fracStr = "";
    temp = fracInt;
    if (decimals == 0) return intStr;
    for (int i = 0; i < decimals; i++) { fracStr = (char)('0' + temp % 10) + fracStr; temp /= 10; }

    return intStr + "." + fracStr;
}

static int stringToInt(const std::string& s) {
    int val = 0;
    for (int i = 0; i < (int)s.length(); i++)
        if (s[i] >= '0' && s[i] <= '9') val = val * 10 + (s[i] - '0');
    return val;
}

static double gradeToGPA(const std::string& grade) {
    if (grade == "A")  return 4.0;
    if (grade == "B+") return 3.5;
    if (grade == "B")  return 3.0;
    if (grade == "C+") return 2.5;
    if (grade == "C")  return 2.0;
    if (grade == "D")  return 1.0;
    return 0.0;
}

double bestThreeOfFive(double marks[], int n) {
    if (n <= 0) return 0.0;
    if (n <= 3) {
        double sum = 0.0;
        for (int i = 0; i < n; i++) sum += marks[i];
        return sum / n;
    }

    int count = (n > 5) ? 5 : n;

    int low1 = 0, low2 = 1;
    if (marks[low1] > marks[low2]) { int t = low1; low1 = low2; low2 = t; }

    for (int i = 2; i < count; i++) {
        if (marks[i] < marks[low1]) {
            low2 = low1;
            low1 = i;
        } else if (marks[i] < marks[low2]) {
            low2 = i;
        }
    }

    double sum = 0.0;
    int kept = 0;
    for (int i = 0; i < count; i++) {
        if (i == low1 || i == low2) continue;
        sum += marks[i];
        kept++;
    }
    return (kept > 0) ? sum / kept : 0.0;
}

double computeWeightedTotal(double quizPct, double asgnPct,
                            double midPct,  double finalPct) {
    return quizPct * 0.10 + asgnPct * 0.10 + midPct * 0.30 + finalPct * 0.50;
}

std::string getLetterGrade(double total) {
    if (total >= 85) return "A";
    if (total >= 80) return "B+";
    if (total >= 70) return "B";
    if (total >= 65) return "C+";
    if (total >= 60) return "C";
    if (total >= 50) return "D";
    return "F";
}

void enterMarks(const std::string& roll,
                const std::string& courseCode,
                const std::string& semester) {
    std::cout << "\n--- Enter Marks: " << roll << " | " << courseCode << " ---\n";

    double quizMarks[5];
    int    quizCount = 0;
    std::cout << "Number of quizzes (1-5): ";
    std::cin >> quizCount;
    if (quizCount < 1) quizCount = 1;
    if (quizCount > 5) quizCount = 5;

    for (int i = 0; i < quizCount; i++) {
        double q = -1;
        while (q < 0 || q > 10) {
            std::cout << "  Quiz " << (i + 1) << " (0-10): ";
            std::cin >> q;
            if (q < 0 || q > 10) std::cout << "  Invalid. Enter 0-10.\n";
        }
        quizMarks[i] = q;
    }
    double quizAvg  = bestThreeOfFive(quizMarks, quizCount);
    double quizPct  = quizAvg * 10.0;

    double asgnMarks[5];
    int    asgnCount = 0;
    std::cout << "Number of assignments (1-5): ";
    std::cin >> asgnCount;
    if (asgnCount < 1) asgnCount = 1;
    if (asgnCount > 5) asgnCount = 5;

    for (int i = 0; i < asgnCount; i++) {
        double a = -1;
        while (a < 0 || a > 10) {
            std::cout << "  Assignment " << (i + 1) << " (0-10): ";
            std::cin >> a;
            if (a < 0 || a > 10) std::cout << "  Invalid. Enter 0-10.\n";
        }
        asgnMarks[i] = a;
    }
    double asgnSum = 0.0;
    for (int i = 0; i < asgnCount; i++) asgnSum += asgnMarks[i];
    double asgnPct = (asgnSum / asgnCount) * 10.0;

    double mid = -1;
    while (mid < 0 || mid > 40) {
        std::cout << "Mid marks (0-40): ";
        std::cin >> mid;
        if (mid < 0 || mid > 40) std::cout << "  Invalid. Enter 0-40.\n";
    }
    double midPct = (mid / 40.0) * 100.0;

    double finalMark = -1;
    while (finalMark < 0 || finalMark > 60) {
        std::cout << "Final marks (0-60): ";
        std::cin >> finalMark;
        if (finalMark < 0 || finalMark > 60) std::cout << "  Invalid. Enter 0-60.\n";
    }
    double finalPct = (finalMark / 60.0) * 100.0;

    double total       = computeWeightedTotal(quizPct, asgnPct, midPct, finalPct);
    std::string grade  = getLetterGrade(total);

    std::vector<std::string> newRow;
    newRow.push_back(roll);
    newRow.push_back(courseCode);
    newRow.push_back(semester);
    newRow.push_back(doubleToString(quizPct,   2));
    newRow.push_back(doubleToString(asgnPct,   2));
    newRow.push_back(doubleToString(midPct,    2));
    newRow.push_back(doubleToString(finalPct,  2));
    newRow.push_back(doubleToString(total,     2));
    newRow.push_back(grade);

    std::vector<std::vector<std::string>> rows = readTXT("grades.txt");
    bool updated = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() <= GRD_SEM) continue;
        if (rows[i][GRD_ROLL]   != roll)       continue;
        if (rows[i][GRD_COURSE] != courseCode)  continue;
        if (rows[i][GRD_SEM]    != semester)   continue;
        rows[i]  = newRow;
        updated  = true;
        break;
    }

    if (updated)
        writeTXT("grades.txt", gradesHeader(), rows);
    else
        appendTXT("grades.txt", newRow);

    applyAttendancePenalty(roll, courseCode, semester);

    std::cout << "Marks saved. Total: " << doubleToString(total, 2)
              << "  Grade: " << grade << "\n";
}

double computeGPA(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> grades      = readTXT("grades.txt");
    std::vector<std::vector<std::string>> enrollments = readTXT("enrollments.txt");
    std::vector<std::vector<std::string>> courses     = readTXT("courses.txt");

    double totalPoints  = 0.0;
    int    totalCredits = 0;

    for (int i = 0; i < (int)grades.size(); i++) {
        if ((int)grades[i].size() <= GRD_GRADE) continue;
        if (grades[i][GRD_ROLL] != roll)        continue;
        if (grades[i][GRD_SEM]  != semester)    continue;

        std::string code = grades[i][GRD_COURSE];

        int credits = 0;
        for (int j = 0; j < (int)courses.size(); j++) {
            if ((int)courses[j].size() <= CRS_CREDITS) continue;
            if (courses[j][CRS_CODE] != code) continue;
            credits = stringToInt(courses[j][CRS_CREDITS]);
            break;
        }

        double gpaPoints = gradeToGPA(grades[i][GRD_GRADE]);
        totalPoints  += gpaPoints * credits;
        totalCredits += credits;
    }

    if (totalCredits == 0) return 0.0;
    return totalPoints / totalCredits;
}

Stats computeClassStats(const std::string& courseCode,
                        const std::string& semester) {
    std::vector<std::vector<std::string>> grades = readTXT("grades.txt");
    std::vector<double> totals;

    for (int i = 0; i < (int)grades.size(); i++) {
        if ((int)grades[i].size() <= GRD_TOTAL) continue;
        if (grades[i][GRD_COURSE] != courseCode) continue;
        if (grades[i][GRD_SEM]    != semester)   continue;
        totals.push_back(stringToDouble(grades[i][GRD_TOTAL]));
    }

    Stats s;
    s.highest = 0.0;
    s.lowest  = 100.0;
    s.mean    = 0.0;
    s.median  = 0.0;

    if (totals.empty()) return s;

    double sum = 0.0;
    for (int i = 0; i < (int)totals.size(); i++) {
        sum += totals[i];
        if (totals[i] > s.highest) s.highest = totals[i];
        if (totals[i] < s.lowest)  s.lowest  = totals[i];
    }
    s.mean = sum / totals.size();

    for (int i = 0; i < (int)totals.size() - 1; i++) {
        for (int j = 0; j < (int)totals.size() - 1 - i; j++) {
            if (totals[j] > totals[j + 1]) {
                double tmp    = totals[j];
                totals[j]     = totals[j + 1];
                totals[j + 1] = tmp;
            }
        }
    }

    int n = (int)totals.size();
    if (n % 2 == 0)
        s.median = (totals[n / 2 - 1] + totals[n / 2]) / 2.0;
    else
        s.median = totals[n / 2];

    return s;
}

void applyAttendancePenalty(const std::string& roll,
                            const std::string& courseCode,
                            const std::string& semester) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct >= 75.0) return;

    std::vector<std::vector<std::string>> rows = readTXT("grades.txt");
    bool found = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() <= GRD_GRADE) continue;
        if (rows[i][GRD_ROLL]   != roll)       continue;
        if (rows[i][GRD_COURSE] != courseCode)  continue;
        if (rows[i][GRD_SEM]    != semester)   continue;
        rows[i][GRD_GRADE] = "F";
        found = true;
        break;
    }

    if (found) {
        writeTXT("grades.txt", gradesHeader(), rows);
        std::cout << "  [!] Attendance below 75%. Grade overridden to F for "
                  << roll << " in " << courseCode << ".\n";
    }
}