#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

static int readIntInRange(int lo, int hi) {
    int val;
    while (true) {
        std::cin >> val;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again: ";
            continue;
        }
        if (val < lo || val > hi) {
            std::cout << "Choice out of range. Try again: ";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return val;
    }
}

static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static void printRow(const std::vector<std::string>& row) {
    if (row.empty()) {
        std::cout << "  Not found.\n";
        return;
    }
    for (int i = 0; i < (int)row.size(); i++) {
        std::cout << row[i];
        if (i != (int)row.size() - 1) std::cout << " | ";
    }
    std::cout << "\n";
}

static void updateStudentSubmenu(const std::string& roll) {
    std::cout << "\n  -- Select field to update --\n";
    std::cout << "  1. Name\n";
    std::cout << "  2. Department\n";
    std::cout << "  3. Semester\n";
    std::cout << "  4. CGPA\n";
    std::cout << "  5. Status\n";
    std::cout << "  0. Cancel\n";
    std::cout << "  Choice: ";
    int choice = readIntInRange(0, 5);

    if (choice == 0) return;

    int colIndex = choice;
    std::string newValue = readLine("  Enter new value: ");
    updateStudent(roll, colIndex, newValue);
}

static void studentMenu() {
    while (true) {
        std::cout << "\n===== STUDENT MANAGEMENT =====\n";
        std::cout << "1. Add Student\n";
        std::cout << "2. Search by Roll\n";
        std::cout << "3. Search by Name\n";
        std::cout << "4. Update Student\n";
        std::cout << "5. Soft Delete Student\n";
        std::cout << "6. List Active Students\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 6);

        if (choice == 0) return;

        if (choice == 1) {
            std::string roll = readLine("Roll (BSAI-YY-XXX): ");
            std::string name = readLine("Name: ");
            std::string dept = readLine("Department: ");
            std::string sem  = readLine("Semester: ");
            std::string cgpa = readLine("CGPA: ");
            addStudent(roll, name, dept, sem, cgpa);
        }
        else if (choice == 2) {
            std::string roll = readLine("Roll: ");
            printRow(searchByRoll(roll));
        }
        else if (choice == 3) {
            std::string query = readLine("Name contains: ");
            std::vector<std::vector<std::string>> results = searchByName(query);
            if (results.empty()) std::cout << "  No matches found.\n";
            for (int i = 0; i < (int)results.size(); i++) printRow(results[i]);
        }
        else if (choice == 4) {
            std::string roll = readLine("Roll: ");
            updateStudentSubmenu(roll);
        }
        else if (choice == 5) {
            std::string roll = readLine("Roll: ");
            softDelete(roll);
        }
        else if (choice == 6) {
            std::vector<std::vector<std::string>> active = listActiveStudents();
            for (int i = 0; i < (int)active.size(); i++) printRow(active[i]);
            std::cout << "Total: " << active.size() << "\n";
        }
    }
}

static void printEnrollResult(EnrollResult result) {
    if      (result == ENROLL_SUCCESS)               std::cout << "  Enrollment successful.\n";
    else if (result == ENROLL_STUDENT_NOT_ACTIVE)     std::cout << "  Error: Student not active.\n";
    else if (result == ENROLL_COURSE_NOT_FOUND)       std::cout << "  Error: Course not found.\n";
    else if (result == ENROLL_NO_SEATS)               std::cout << "  Error: No seats available.\n";
    else if (result == ENROLL_ALREADY_ENROLLED)       std::cout << "  Error: Already enrolled.\n";
    else if (result == ENROLL_CREDIT_OVERLOAD)        std::cout << "  Error: Credit load exceeds 21 hours.\n";
    else if (result == ENROLL_PREREQUISITE_NOT_MET)   std::cout << "  Error: Prerequisite not met.\n";
}

static void courseMenu() {
    while (true) {
        std::cout << "\n===== COURSE MANAGEMENT =====\n";
        std::cout << "1. Enroll Student\n";
        std::cout << "2. Drop Course\n";
        std::cout << "3. Check Credit Load\n";
        std::cout << "4. Check Prerequisite\n";
        std::cout << "5. List Enrolled Students\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 5);

        if (choice == 0) return;

        if (choice == 1) {
            std::string roll = readLine("Roll: ");
            std::string code = readLine("Course Code: ");
            std::string sem  = readLine("Semester: ");
            EnrollResult r = enrollStudent(roll, code, sem);
            printEnrollResult(r);
        }
        else if (choice == 2) {
            std::string roll = readLine("Roll: ");
            std::string code = readLine("Course Code: ");
            std::string sem  = readLine("Semester: ");
            dropCourse(roll, code, sem);
        }
        else if (choice == 3) {
            std::string roll = readLine("Roll: ");
            std::string sem  = readLine("Semester: ");
            std::cout << "  Credit Load: " << getCreditLoad(roll, sem) << " hrs\n";
        }
        else if (choice == 4) {
            std::string roll = readLine("Roll: ");
            std::string code = readLine("Course Code: ");
            bool ok = checkPrerequisite(roll, code);
            std::cout << "  Prerequisite " << (ok ? "satisfied." : "NOT satisfied.") << "\n";
        }
        else if (choice == 5) {
            std::string code = readLine("Course Code: ");
            std::vector<std::string> rolls = listEnrolledStudents(code);
            for (int i = 0; i < (int)rolls.size(); i++) std::cout << "  " << rolls[i] << "\n";
            std::cout << "Total: " << rolls.size() << "\n";
        }
    }
}

static void attendanceMenu() {
    while (true) {
        std::cout << "\n===== ATTENDANCE MANAGEMENT =====\n";
        std::cout << "1. Mark Attendance\n";
        std::cout << "2. View Attendance %\n";
        std::cout << "3. View Shortage List\n";
        std::cout << "4. Undo Last Session\n";
        std::cout << "5. Print Daily Sheet\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 5);

        if (choice == 0) return;

        if (choice == 1) {
            std::string code = readLine("Course Code: ");
            std::string date = readLine("Date (DD-MM-YYYY): ");
            markAttendance(code, date);
        }
        else if (choice == 2) {
            std::string roll = readLine("Roll: ");
            std::string code = readLine("Course Code: ");
            std::cout << "  Attendance: " << getAttendancePct(roll, code) << "%\n";
        }
        else if (choice == 3) {
            std::vector<AttendanceShortage> list = getShortageList();
            for (int i = 0; i < (int)list.size(); i++) {
                std::cout << "  " << list[i].roll << " | " << list[i].courseCode
                          << " | " << list[i].percentage << "%\n";
            }
            std::cout << "Total: " << list.size() << "\n";
        }
        else if (choice == 4) {
            undoLastSession();
        }
        else if (choice == 5) {
            std::string code = readLine("Course Code: ");
            std::string date = readLine("Date (DD-MM-YYYY): ");
            printDailySheet(code, date);
        }
    }
}

static void gradesMenu() {
    while (true) {
        std::cout << "\n===== GRADES MANAGEMENT =====\n";
        std::cout << "1. Enter Marks\n";
        std::cout << "2. View GPA\n";
        std::cout << "3. View Class Stats\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 3);

        if (choice == 0) return;

        if (choice == 1) {
            std::string roll = readLine("Roll: ");
            std::string code = readLine("Course Code: ");
            std::string sem  = readLine("Semester: ");
            enterMarks(roll, code, sem);
        }
        else if (choice == 2) {
            std::string roll = readLine("Roll: ");
            std::string sem  = readLine("Semester: ");
            std::cout << "  GPA: " << computeGPA(roll, sem) << "\n";
        }
        else if (choice == 3) {
            std::string code = readLine("Course Code: ");
            std::string sem  = readLine("Semester: ");
            Stats s = computeClassStats(code, sem);
            std::cout << "  Highest: " << s.highest << "\n";
            std::cout << "  Lowest:  " << s.lowest  << "\n";
            std::cout << "  Mean:    " << s.mean    << "\n";
            std::cout << "  Median:  " << s.median  << "\n";
        }
    }
}

static void feeMenu() {
    while (true) {
        std::cout << "\n===== FEE MANAGEMENT =====\n";
        std::cout << "1. Record Payment\n";
        std::cout << "2. Compute Late Fine\n";
        std::cout << "3. Generate Receipt\n";
        std::cout << "4. View Fee Defaulters\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 4);

        if (choice == 0) return;

        if (choice == 1) {
            std::string roll = readLine("Roll: ");
            std::string sem  = readLine("Semester: ");
            std::string amountStr = readLine("Amount: ");
            std::string method = readLine("Method (Cash/Online): ");
            std::string date = readLine("Payment Date (DD-MM-YYYY): ");

            double amount = 0.0;
            double dec = 0.1;
            bool afterDot = false;
            for (int i = 0; i < (int)amountStr.length(); i++) {
                if (amountStr[i] == '.') { afterDot = true; continue; }
                if (amountStr[i] < '0' || amountStr[i] > '9') continue;
                if (!afterDot) amount = amount * 10 + (amountStr[i] - '0');
                else { amount += (amountStr[i] - '0') * dec; dec *= 0.1; }
            }
            recordPayment(roll, sem, amount, method, date);
        }
        else if (choice == 2) {
            std::string roll = readLine("Roll: ");
            std::string sem  = readLine("Semester: ");
            std::cout << "  Late Fine: Rs." << computeLateFine(roll, sem) << "\n";
        }
        else if (choice == 3) {
            std::string roll = readLine("Roll: ");
            std::string sem  = readLine("Semester: ");
            generateReceipt(roll, sem);
        }
        else if (choice == 4) {
            std::vector<FeeDefaulter> list = getDefaulters();
            for (int i = 0; i < (int)list.size(); i++) {
                std::cout << "  " << list[i].roll << " | " << list[i].semester
                          << " | Rs." << list[i].balance
                          << " | " << list[i].weeksOverdue << " wk(s) late\n";
            }
            std::cout << "Total: " << list.size() << "\n";
        }
    }
}

static void reportsMenu() {
    while (true) {
        std::cout << "\n===== REPORTS =====\n";
        std::cout << "1. Merit List\n";
        std::cout << "2. Attendance Defaulters\n";
        std::cout << "3. Fee Defaulters\n";
        std::cout << "4. Semester Result\n";
        std::cout << "5. Department Summary\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 5);

        if (choice == 0) return;

        std::string semester = "";
        if (choice == 4) semester = readLine("Semester: ");

        if      (choice == 1) printMeritList();
        else if (choice == 2) printAttendanceDefaulters();
        else if (choice == 3) printFeeDefaulters();
        else if (choice == 4) printSemesterResult(semester);
        else if (choice == 5) printDepartmentSummary();

        std::cout << "\n  Export this report to file? (1 = Yes, 0 = No): ";
        int exportChoice = readIntInRange(0, 1);
        if (exportChoice == 1) exportReportToFile(choice, semester);
    }
}

static void mainMenu() {
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "      CAMPUS ANALYTICS ENGINE\n";
        std::cout << "========================================\n";
        std::cout << "1. Student Management\n";
        std::cout << "2. Course Management\n";
        std::cout << "3. Attendance Management\n";
        std::cout << "4. Grades Management\n";
        std::cout << "5. Fee Management\n";
        std::cout << "6. Reports\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        int choice = readIntInRange(0, 6);

        if (choice == 0) {
            std::cout << "\nExiting Campus Analytics Engine. Goodbye!\n";
            return;
        }

        if      (choice == 1) studentMenu();
        else if (choice == 2) courseMenu();
        else if (choice == 3) attendanceMenu();
        else if (choice == 4) gradesMenu();
        else if (choice == 5) feeMenu();
        else if (choice == 6) reportsMenu();
    }
}

int main() {
    mainMenu();
    return 0;
}