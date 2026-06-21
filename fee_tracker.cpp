#include "fee_tracker.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

static const int FEE_ID      = 0;
static const int FEE_ROLL    = 1;
static const int FEE_SEM     = 2;
static const int FEE_TOTAL   = 3;
static const int FEE_PAID    = 4;
static const int FEE_DUEDATE = 5;
static const int FEE_PAYDATE = 6;
static const int FEE_METHOD  = 7;
static const int FEE_STATUS  = 8;

static const int STU_NAME    = 1;
static const int STU_ROLL    = 0;

static std::vector<std::string> feesHeader() {
    std::vector<std::string> h;
    h.push_back("fee_id");
    h.push_back("roll_no");
    h.push_back("semester");
    h.push_back("total_fee");
    h.push_back("amount_paid");
    h.push_back("due_date");
    h.push_back("payment_date");
    h.push_back("payment_method");
    h.push_back("status");
    return h;
}

static int stringToInt(const std::string& s) {
    int val = 0;
    for (int i = 0; i < (int)s.length(); i++)
        if (s[i] >= '0' && s[i] <= '9') val = val * 10 + (s[i] - '0');
    return val;
}

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

static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int daysInMonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) return 29;
    if (month < 1 || month > 12) return 30;
    return days[month - 1];
}

static bool isValidDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }
    int d = stringToInt(date.substr(0, 2));
    int m = stringToInt(date.substr(3, 2));
    int y = stringToInt(date.substr(6, 4));
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > daysInMonth(m, y)) return false;
    return true;
}

static long dateToDays(const std::string& date) {
    int d = stringToInt(date.substr(0, 2));
    int m = stringToInt(date.substr(3, 2));
    int y = stringToInt(date.substr(6, 4));

    long total = 0;
    for (int yr = 1; yr < y; yr++) {
        total += isLeapYear(yr) ? 366 : 365;
    }
    for (int mo = 1; mo < m; mo++) {
        total += daysInMonth(mo, y);
    }
    total += d;
    return total;
}

int daysBetween(const std::string& date1, const std::string& date2) {
    if (!isValidDate(date1) || !isValidDate(date2)) return 0;
    long d1 = dateToDays(date1);
    long d2 = dateToDays(date2);
    int diff = (int)(d2 - d1);
    return diff < 0 ? -diff : diff;
}

void recordPayment(const std::string& roll,
                   const std::string& semester,
                   double amount,
                   const std::string& method,
                   const std::string& paymentDate) {
    if (!isValidDate(paymentDate)) {
        std::cout << "Error: Invalid date format. Use DD-MM-YYYY.\n";
        return;
    }
    if (amount <= 0) {
        std::cout << "Error: Payment amount must be greater than 0.\n";
        return;
    }

    std::vector<std::vector<std::string>> rows = readTXT("fees.txt");
    bool found = false;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() <= FEE_STATUS) continue;
        if (rows[i][FEE_ROLL] != roll)         continue;
        if (rows[i][FEE_SEM]  != semester)     continue;

        double total      = stringToDouble(rows[i][FEE_TOTAL]);
        double alreadyPaid = stringToDouble(rows[i][FEE_PAID]);
        double newPaid    = alreadyPaid + amount;
        if (newPaid > total) newPaid = total;

        rows[i][FEE_PAID]    = doubleToString(newPaid, 0);
        rows[i][FEE_PAYDATE] = paymentDate;
        rows[i][FEE_METHOD]  = method;

        if (newPaid >= total)
            rows[i][FEE_STATUS] = "paid";
        else if (newPaid > 0)
            rows[i][FEE_STATUS] = "partial";

        found = true;
        break;
    }

    if (!found) {
        std::cout << "Error: Fee record not found for " << roll
                  << " semester " << semester << ".\n";
        return;
    }

    writeTXT("fees.txt", feesHeader(), rows);
    std::cout << "Payment of Rs." << doubleToString(amount, 0)
              << " recorded for " << roll << ".\n";
}

double computeLateFine(const std::string& roll, const std::string& semester) {
    std::vector<std::string> row = findRow("fees.txt", FEE_ROLL, roll);

    if (row.empty() || row[FEE_SEM] != semester) return 0.0;

    std::string dueDate = row[FEE_DUEDATE];
    std::string payDate = row[FEE_PAYDATE];

    if (payDate == "00-00-0000" || !isValidDate(payDate)) return 0.0;
    if (!isValidDate(dueDate)) return 0.0;

    int days = (int)(dateToDays(payDate) - dateToDays(dueDate));
    if (days <= 0) return 0.0;

    int    weeksLate  = days / 7;
    double totalFee   = stringToDouble(row[FEE_TOTAL]);
    double fine       = totalFee * 0.02 * weeksLate;
    return fine;
}

void generateReceipt(const std::string& roll, const std::string& semester) {
    std::vector<std::vector<std::string>> rows = readTXT("fees.txt");
    std::vector<std::string> row;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() <= FEE_STATUS) continue;
        if (rows[i][FEE_ROLL] != roll)         continue;
        if (rows[i][FEE_SEM]  != semester)     continue;
        row = rows[i];
        break;
    }

    if (row.empty()) {
        std::cout << "Error: Fee record not found.\n";
        return;
    }

    std::vector<std::string> student = searchByRoll(roll);
    std::string name = (student.size() > STU_NAME) ? student[STU_NAME] : roll;

    double totalFee  = stringToDouble(row[FEE_TOTAL]);
    double paid      = stringToDouble(row[FEE_PAID]);
    double fine      = computeLateFine(roll, semester);
    double balance   = (totalFee + fine) - paid;

    std::string border(52, '=');
    std::string divider(52, '-');

    std::cout << "\n" << border << "\n";
    std::cout << std::setw(36) << std::right << "CAMPUS FEE RECEIPT\n";
    std::cout << border << "\n";
    std::cout << std::left << std::setw(20) << "Student:"
              << name << "\n";
    std::cout << std::left << std::setw(20) << "Roll No:"
              << roll << "\n";
    std::cout << std::left << std::setw(20) << "Semester:"
              << semester << "\n";
    std::cout << std::left << std::setw(20) << "Due Date:"
              << row[FEE_DUEDATE] << "\n";
    std::cout << std::left << std::setw(20) << "Payment Date:"
              << row[FEE_PAYDATE] << "\n";
    std::cout << divider << "\n";
    std::cout << std::left  << std::setw(32) << "Tuition Fee:"
              << std::right << std::setw(10) << std::setfill(' ')
              << "Rs." + doubleToString(totalFee, 0) << "\n";
    if (fine > 0) {
        std::cout << std::left  << std::setw(32) << "Late Fine:"
                  << std::right << std::setw(10)
                  << "Rs." + doubleToString(fine, 0) << "\n";
    }
    std::cout << std::left  << std::setw(32) << "Total Due:"
              << std::right << std::setw(10)
              << "Rs." + doubleToString(totalFee + fine, 0) << "\n";
    std::cout << std::left  << std::setw(32) << "Amount Paid:"
              << std::right << std::setw(10)
              << "Rs." + doubleToString(paid, 0) << "\n";
    std::cout << divider << "\n";
    std::cout << std::left  << std::setw(32) << "Balance:"
              << std::right << std::setw(10)
              << "Rs." + doubleToString(balance < 0 ? 0 : balance, 0) << "\n";
    std::cout << std::left  << std::setw(32) << "Status:"
              << std::right << std::setw(10) << row[FEE_STATUS] << "\n";
    std::cout << border << "\n\n";
}

std::vector<FeeDefaulter> getDefaulters() {
    std::vector<std::vector<std::string>> rows = readTXT("fees.txt");
    std::vector<FeeDefaulter> defaulters;

    for (int i = 0; i < (int)rows.size(); i++) {
        if ((int)rows[i].size() <= FEE_STATUS) continue;

        double total  = stringToDouble(rows[i][FEE_TOTAL]);
        double paid   = stringToDouble(rows[i][FEE_PAID]);
        double balance = total - paid;

        if (balance <= 0) continue;

        std::string dueDate = rows[i][FEE_DUEDATE];
        std::string payDate = rows[i][FEE_PAYDATE];
        if (!isValidDate(dueDate)) continue;

        int daysLate = 0;
        if (payDate == "00-00-0000" || !isValidDate(payDate)) {
            daysLate = (int)(dateToDays("19-06-2026") - dateToDays(dueDate));
        } else {
            daysLate = (int)(dateToDays(payDate) - dateToDays(dueDate));
        }

        if (daysLate <= 0) continue;

        FeeDefaulter d;
        d.roll        = rows[i][FEE_ROLL];
        d.semester    = rows[i][FEE_SEM];
        d.balance     = balance;
        d.weeksOverdue = daysLate / 7;
        defaulters.push_back(d);
    }

    for (int i = 0; i < (int)defaulters.size() - 1; i++) {
        for (int j = 0; j < (int)defaulters.size() - 1 - i; j++) {
            if (defaulters[j].balance < defaulters[j + 1].balance) {
                FeeDefaulter tmp    = defaulters[j];
                defaulters[j]       = defaulters[j + 1];
                defaulters[j + 1]   = tmp;
            }
        }
    }

    return defaulters;
}