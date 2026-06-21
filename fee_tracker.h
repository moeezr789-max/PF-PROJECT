#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>

struct FeeDefaulter {
    std::string roll;
    std::string semester;
    double      balance;
    int         weeksOverdue;
};

void   recordPayment(const std::string& roll,
                     const std::string& semester,
                     double amount,
                     const std::string& method,
                     const std::string& paymentDate);

double computeLateFine(const std::string& roll, const std::string& semester);

int    daysBetween(const std::string& date1, const std::string& date2);

void   generateReceipt(const std::string& roll, const std::string& semester);

std::vector<FeeDefaulter> getDefaulters();

#endif