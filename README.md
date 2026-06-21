Campus Analytics Engine
A multi-file, menu-driven C++ console application for managing student records, course enrollments, attendance, grades, and fee transactions for a university campus. Built using only fundamental C++ constructs (no STL algorithms, no OOP/classes, no <ctime>) — all data is read from and written to flat .txt files.
Features
Student Management — add, search (by roll or name), update, and soft-delete students with validation (roll format, CGPA range, no digits in name)
Course Management — enroll/drop students with checks for seat capacity, credit load (max 21 hrs), and prerequisites
Attendance — mark daily attendance (P/A/L), compute attendance percentage, flag students below 75%, undo last session
Grades — enter quiz/assignment/mid/final marks, compute weighted totals, letter grades, semester GPA, and class statistics
Fee Tracking — record payments, calculate late fines (manual date arithmetic, no <ctime>), generate receipts, list defaulters
Reports — merit list, attendance defaulters, fee defaulters, semester result sheets, department summary, and export to file
File Structure
Code
How to Compile
Code
How to Run
Code
(On Windows: .\campus or campus.exe)
Sample Run
Code
Notes
All data files must be in the same directory as the compiled executable.
grades.txt starts empty (header row only) and is populated as marks are entered.
No external libraries beyond the C++ standard library (<iostream>, <fstream>, <iomanip>, <string>, <vector>) are used.

A multi-file, menu-driven C++ console application for managing student records, course enrollments, attendance, grades, and fee transactions for a university campus. Built using only fundamental C++ constructs (no STL algorithms, no OOP/classes, no `<ctime>`) — all data is read from and written to flat `.txt` files.

## Features

- **Student Management** — add, search (by roll or name), update, and soft-delete students with validation (roll format, CGPA range, no digits in name)
- **Course Management** — enroll/drop students with checks for seat capacity, credit load (max 21 hrs), and prerequisites
- **Attendance** — mark daily attendance (P/A/L), compute attendance percentage, flag students below 75%, undo last session
- **Grades** — enter quiz/assignment/mid/final marks, compute weighted totals, letter grades, semester GPA, and class statistics
- **Fee Tracking** — record payments, calculate late fines (manual date arithmetic, no `<ctime>`), generate receipts, list defaulters
- **Reports** — merit list, attendance defaulters, fee defaulters, semester result sheets, department summary, and export to file

## File Structure

```
main.cpp                          (entry point, 3-level nested menu)
filehandler.h / filehandler.cpp   (all .txt read/write operations)
student_ops.h / student_ops.cpp   (student CRUD operations)
course_ops.h / course_ops.cpp     (enrollment, drop, prerequisite checks)
attendance.h / attendance.cpp     (attendance marking and reporting)
grades.h / grades.cpp             (marks entry, GPA, class stats)
fee_tracker.h / fee_tracker.cpp   (payments, late fines, receipts)
reports.h / reports.cpp           (consolidated reports)

students.txt
courses.txt
enrollments.txt
attendance_log.txt
fees.txt
grades.txt
```

## How to Compile

```
g++ main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp -o campus
```

## How to Run

```
./campus
```
(On Windows: `.\campus` or `campus.exe`)

## Sample Run

```
========================================
      CAMPUS ANALYTICS ENGINE
========================================
1. Student Management
2. Course Management
3. Attendance Management
4. Grades Management
5. Fee Management
6. Reports
0. Exit
Choice: 1

===== STUDENT MANAGEMENT =====
1. Add Student
2. Search by Roll
3. Search by Name
4. Update Student
5. Soft Delete Student
6. List Active Students
0. Back to Main Menu
Choice: 6

BSAI-23-001 | Ahmed Raza | Artificial Intelligence | 2 | 3.75 | active
BSAI-23-002 | Fatima Malik | Artificial Intelligence | 2 | 3.20 | active
...
Total: 29
```

## Notes

- All data files must be in the same directory as the compiled executable.
- `grades.txt` starts empty (header row only) and is populated as marks are entered.
- No external libraries beyond the C++ standard library (`<iostream>`, `<fstream>`, `<iomanip>`, `<string>`, `<vector>`) are used.
-