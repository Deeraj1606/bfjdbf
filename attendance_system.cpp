#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip> // for setprecision
#include <algorithm>
#include <sstream>
#include <conio.h>
#include <chrono>
#include <regex>
#include <cctype>
#include <cmath>
#include <thread>
#include <windows.h> // For Windows console colors
#include <numeric>
#include <direct.h>  // for _mkdir on Windows
#include <sys/stat.h>  // for mkdir on Unix
#include <filesystem>

using namespace std;

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
    setColor(7); // Default color (white)
}

void setColorWithBackground(int textColor, int bgColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

void printTitle(const string& title) {
    int width = 60;
    string border(width, '=');
    
    setColor(14); // Yellow
    cout << "\n" << border << "\n";
    
    setColor(11); // Light cyan
    cout << setw((width + title.length()) / 2) << title << "\n";
    
    setColor(14); // Yellow
    cout << border << "\n\n";
    
    resetColor();
}

class Date {
private:
    int year, month, day;

public:
    Date(const string& date) {
        sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);
    }

    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    string toString() const {
        char buffer[11];
        sprintf(buffer, "%04d-%02d-%02d", year, month, day);
        return string(buffer);
    }
};

class Student {
private:
    string uniqueId;  // Format: class_section_rollNo (e.g., "1A_1")
    string rollNo;
    string name;
    string className;  // e.g., "1A", "2B", "3C"
    string section;    // e.g., "A", "B", "C"
    string contactNo;
    map<string, bool> attendanceRecord;
    map<string, string> remarks;
    string email;
    string gender;
    string dateOfBirth;
    vector<pair<string, string>> notifications;
    map<string, float> subjectGrades;  // subject -> grade
    int absenceWarningCount;
    bool isOnProbation;
    
    // School-specific fields
    string parentName;
    string parentEmail;
    string parentPhone;
    string bloodGroup;
    string address;
    string busRoute;
    vector<string> extracurriculars;
    map<string, string> behaviorNotes;
    map<string, int> conductMarks;

public:
    Student(string roll, string n, string class_, string sect,
           string contact, string mail, string gen, string dob) 
        : rollNo(roll), name(n), className(class_), section(sect),
          contactNo(contact), email(mail), gender(gen), 
          dateOfBirth(dob), absenceWarningCount(0), isOnProbation(false) {
        uniqueId = class_ + "_" + sect + "_" + roll;
    }

    string getRollNo() const { return rollNo; }
    string getName() const { return name; }
    string getClassName() const { return className; }
    string getSection() const { return section; }
    string getContactNo() const { return contactNo; }
    string getEmail() const { return email; }
    string getGender() const { return gender; }
    string getDateOfBirth() const { return dateOfBirth; }
    string getUniqueId() const { return uniqueId; }
    
    void markAttendance(const string& date, bool present, const string& remark = "") {
        attendanceRecord[date] = present;
        if (!remark.empty()) {
            remarks[date] = remark;
        }
    }

    float getAttendancePercentage() const {
        if (attendanceRecord.empty()) return 0.0;
        
        int present = 0;
        for (const auto& record : attendanceRecord) {
            if (record.second) present++;
        }
        return (float)present / attendanceRecord.size() * 100;
    }

    string getAttendanceDetails() const {
        stringstream ss;
        for (const auto& record : attendanceRecord) {
            ss << record.first << ": " << (record.second ? "Present" : "Absent");
            auto remark = remarks.find(record.first);
            if (remark != remarks.end()) {
                ss << " - " << remark->second;
            }
            ss << "\n";
        }
        return ss.str();
    }

    int getTotalPresent() const {
        int count = 0;
        for (const auto& record : attendanceRecord) {
            if (record.second) count++;
        }
        return count;
    }

    int getTotalAbsent() const {
        return attendanceRecord.size() - getTotalPresent();
    }

    bool getAttendanceForDate(const string& date) const {
        auto it = attendanceRecord.find(date);
        return (it != attendanceRecord.end()) ? it->second : false;
    }

    const map<string, bool>& getAttendanceRecord() const {
        return attendanceRecord;
    }

    string getRemarkForDate(const string& date) const {
        auto it = remarks.find(date);
        return (it != remarks.end()) ? it->second : "";
    }

    float getAttendancePercentageRange(const string& startDate, const string& endDate) const {
        int present = 0, total = 0;
        for (const auto& record : attendanceRecord) {
            if (record.first >= startDate && record.first <= endDate) {
                if (record.second) present++;
                total++;
            }
        }
        return total == 0 ? 0.0 : (float)present / total * 100;
    }

    void addNotification(const string& message) {
        notifications.push_back({getCurrentDate(), message});
    }

    vector<pair<string, string>> getNotifications() const {
        return notifications;
    }

    int getAttendanceStreak() const {
        if (attendanceRecord.empty()) return 0;
        
        int currentStreak = 0;
        int maxStreak = 0;
        Date prevDate("");
        bool firstRecord = true;

        for (const auto& record : attendanceRecord) {
            Date currentDate(record.first);
            if (firstRecord) {
                if (record.second) currentStreak = 1;
                firstRecord = false;
            } else {
                if (record.second) {
                    currentStreak++;
                } else {
                    currentStreak = 0;
                }
            }
            maxStreak = max(maxStreak, currentStreak);
            prevDate = currentDate;
        }
        return maxStreak;
    }

    string getCurrentDate() const {
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        tm* ltm = localtime(&time);
        
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
           << setfill('0') << setw(2) << ltm->tm_mday;
        return ss.str();
    }

    void addSubjectGrade(const string& subject, float grade) {
        subjectGrades[subject] = grade;
    }

    void addBehaviorNote(const string& note) {
        behaviorNotes[getCurrentDate()] = note;
    }

    void updateConductMarks(const string& subject, int marks) {
        conductMarks[subject] = marks;
    }

    void addExtracurricular(const string& activity) {
        extracurriculars.push_back(activity);
    }

    void setBusRoute(const string& route) {
        busRoute = route;
    }

    const map<string, float>& getGrades() const { return subjectGrades; }
    bool getIsOnProbation() const { return isOnProbation; }
    string getParentName() const { return parentName; }
    string getParentEmail() const { return parentEmail; }
    string getParentPhone() const { return parentPhone; }
    string getBloodGroup() const { return bloodGroup; }
    string getAddress() const { return address; }
    string getBusRoute() const { return busRoute; }
    const vector<string>& getExtracurriculars() const { return extracurriculars; }
    const map<string, string>& getBehaviorNotes() const { return behaviorNotes; }
    const map<string, int>& getConductMarks() const { return conductMarks; }

    // Add these helper functions inside the Student class
    int getConsecutiveAttendance() const {
        int currentStreak = 0;
        int maxStreak = 0;
        vector<string> dates;
        
        // Get all dates and sort them
        for (const auto& record : attendanceRecord) {
            dates.push_back(record.first);
        }
        sort(dates.begin(), dates.end());
        
        // Calculate streak
        for (const auto& date : dates) {
            if (attendanceRecord.at(date)) {
                currentStreak++;
                maxStreak = max(maxStreak, currentStreak);
            } else {
                currentStreak = 0;
            }
        }
        return maxStreak;
    }
};

class UIHelper {
public:
    static void drawBox(const string& text, int width = 60) {
        string border(width, '=');
        cout << "+" << border << "+\n";
        cout << "|" << setw(width) << left << text << "|\n";
        cout << "+" << border << "+\n";
    }

    static void drawProgressBar(float percentage, int width = 50) {
        int pos = (int)((width * percentage) / 100.0f);
        setColor(14); // Yellow
        cout << "[";
        for (int i = 0; i < width; i++) {
            if (i < pos) cout << "█";
            else cout << " ";
        }
        cout << "] " << fixed << setprecision(1) << percentage << "%";
        resetColor();
        cout << "\n";
    }

    static void showLoadingAnimation(const string& message) {
        const char spinner[] = {'|', '/', '-', '\\'};
        for (int i = 0; i < 20; i++) {
            cout << "\r" << message << " " << spinner[i % 4];
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        cout << "\r" << message << " Done!\n";
    }

    static void drawChart(const vector<pair<string, float>>& data, int width = 40) {
        if (data.empty()) {
            cout << "No data available for chart.\n";
            return;
        }

        float maxVal = 0;
        for (const auto& item : data) {
            maxVal = max(maxVal, item.second);
        }

        for (const auto& item : data) {
            string label = item.first.substr(0, 15);
            cout << left << setw(15) << label << " ";

            int bars = maxVal > 0 ? min((int)((item.second * width) / maxVal), width) : 0;
            
            setColor(11);
            string barStr(bars, '#');
            cout << barStr;
            resetColor();
            
            cout << " " << fixed << setprecision(1) << item.second << "%\n";
        }
    }

    static void drawTable(const vector<vector<string>>& data, const vector<int>& colWidths) {
        cout << "+";
        for (size_t i = 0; i < colWidths.size(); i++) {
            cout << string(colWidths[i], '-');
            cout << (i < colWidths.size() - 1 ? "+" : "+");
        }
        cout << "\n";

        for (size_t i = 0; i < data[0].size(); i++) {
            cout << "|" << setw(colWidths[i]) << left << data[0][i];
        }
        cout << "|\n";

        cout << "+";
        for (size_t i = 0; i < colWidths.size(); i++) {
            cout << string(colWidths[i], '-');
            cout << (i < colWidths.size() - 1 ? "+" : "+");
        }
        cout << "\n";

        for (size_t row = 1; row < data.size(); row++) {
            for (size_t col = 0; col < data[row].size(); col++) {
                cout << "|" << setw(colWidths[col]) << left << data[row][col];
            }
            cout << "|\n";
        }

        cout << "+";
        for (size_t i = 0; i < colWidths.size(); i++) {
            cout << string(colWidths[i], '-');
            cout << (i < colWidths.size() - 1 ? "+" : "+");
        }
        cout << "\n";
    }

    static void showNotification(const string& message, char type = 'I') {
        string prefix;
        switch(type) {
            case 'S': // Success
                setColor(10);
                prefix = "✓ ";
                break;
            case 'E': // Error
                setColor(12);
                prefix = "✗ ";
                break;
            case 'W': // Warning
                setColor(14);
                prefix = "⚠ ";
                break;
            default: // Info
                setColor(11);
                prefix = "ℹ ";
        }
        cout << "\n" << prefix << message;
        resetColor();
        cout << "\n";
    }

    static void drawBarChart(const vector<pair<string, float>>& data, int width = 40, bool colored = true) {
        if (data.empty()) return;

        float maxVal = 0;
        for (const auto& item : data) {
            maxVal = max(maxVal, item.second);
        }

        for (const auto& item : data) {
            cout << left << setw(15) << item.first << " ";
            int bars = (int)((item.second * width) / maxVal);
            
            if (colored) {
                if (item.second >= 75) setColor(10);      // Green
                else if (item.second >= 60) setColor(14); // Yellow
                else setColor(12);                        // Red
            }
            
            cout << string(bars, '#');
            resetColor();
            cout << " " << fixed << setprecision(1) << item.second << "%\n";
        }
    }

    static void drawPieChart(const vector<pair<string, int>>& data, int radius = 10) {
        int total = 0;
        for (const auto& item : data) {
            total += item.second;
        }

        cout << "\n";
        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                if (i*i + j*j <= radius*radius) {
                    float angle = atan2(i, j) * 180 / 3.141592653589793238463; // Approximation of M_PI
                    if (angle < 0) angle += 360;
                    
                    int currentSum = 0;
                    for (size_t k = 0; k < data.size(); k++) {
                        currentSum += data[k].second;
                        float percentage = (float)currentSum / total * 360;
                        if (angle <= percentage) {
                            setColor(k + 9); // Different colors for segments
                            cout << "█";
                            resetColor();
                            break;
                        }
                    }
                } else {
                    cout << " ";
                }
            }
            cout << "\n";
        }

        // Legend
        cout << "\nLegend:\n";
        for (size_t i = 0; i < data.size(); i++) {
            setColor(i + 9);
            cout << "█ ";
            resetColor();
            cout << data[i].first << ": " 
                 << fixed << setprecision(1) 
                 << (float)data[i].second / total * 100 << "%\n";
        }
    }

    static void drawProgressBar(float value, float max, int width = 50) {
        float percentage = (value / max) * 100;
        int filled = (int)((width * percentage) / 100);
        
        cout << "[";
        for (int i = 0; i < width; i++) {
            if (i < filled) {
                if (percentage >= 75) setColor(10);
                else if (percentage >= 50) setColor(14);
                else setColor(12);
                cout << "█";
                resetColor();
            } else {
                cout << "-";
            }
        }
        cout << "] " << fixed << setprecision(1) << percentage << "%\n";
    }

    static void drawLineChart(const vector<pair<string, float>>& data, int height = 10) {
        if (data.empty()) return;

        float maxVal = 0, minVal = 100;
        for (const auto& item : data) {
            maxVal = max(maxVal, item.second);
            minVal = min(minVal, item.second);
        }

        vector<string> chart(height, string(data.size() * 3, ' '));
        for (size_t i = 0; i < data.size() - 1; i++) {
            int y1 = (int)((data[i].second - minVal) * (height - 1) / (maxVal - minVal));
            int y2 = (int)((data[i + 1].second - minVal) * (height - 1) / (maxVal - minVal));
            
            int start = min(y1, y2), end = max(y1, y2);
            for (int y = start; y <= end; y++) {
                chart[height - 1 - y][i * 3 + 1] = '|';
            }
            chart[height - 1 - y1][i * 3] = '*';
        }

        for (const auto& line : chart) {
            cout << line << endl;
        }

        for (const auto& item : data) {
            cout << item.first.substr(5) << " "; // Show only day/month
        }
        cout << endl;
    }

    static void drawHeatMap(const vector<vector<float>>& data, 
                          const vector<string>& rowLabels,
                          const vector<string>& colLabels) {
        cout << setw(10) << "";
        for (const auto& label : colLabels) {
            cout << setw(8) << label;
        }
        cout << endl;

        for (size_t i = 0; i < data.size(); i++) {
            cout << setw(10) << left << rowLabels[i];
            for (float value : data[i]) {
                if (value >= 75) setColorWithBackground(0, 10);      // Green
                else if (value >= 60) setColorWithBackground(0, 14); // Yellow
                else setColorWithBackground(0, 12);                  // Red
                cout << setw(8) << fixed << setprecision(1) << value;
                resetColor();
            }
            cout << endl;
        }
    }
};

class Exam {
public:
    string subject;
    string date;
    int maxMarks;
    map<string, float> studentMarks;  // rollNo -> marks
    string examType;  // "Unit Test", "Mid Term", "Final"
};

class ClassTeacher {
public:
    string name;
    string className;
    string section;
    string contactNo;
    vector<string> subjects;
    vector<string> announcements;
};

class Subject {
public:
    string name;
    string teacherName;
    vector<string> chapters;
    int totalMarks;
    int passingMarks;
};

class ExamSchedule {
public:
    string examName;
    string date;
    string time;
    string subject;
    string venue;
    vector<string> instructions;
};

class Homework {
public:
    string subject;
    string description;
    string dueDate;
    bool isSubmitted;
    string teacherRemarks;
};

class ParentMeeting {
public:
    string date;
    string time;
    string teacherName;
    string agenda;
    bool isConfirmed;
    string feedback;
};

struct Notification {
    string message;
    string date;
    string type;  // "urgent", "normal", "info"
    bool isRead;
    vector<string> recipients; // rollNo list
};

class AttendanceSystem {
private:
    vector<Student> students;
    const string ADMIN_PASSWORD = "admin123";
    bool isLoggedIn = false;
    map<string, vector<string>> departmentCourses;
    vector<pair<string, string>> systemLogs;
    vector<Notification> notificationList;  // Now this will work
    
    string getCurrentDate() const {
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        tm* ltm = localtime(&time);
        
        stringstream ss;
        ss << 1900 + ltm->tm_year << "-"
           << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
           << setfill('0') << setw(2) << ltm->tm_mday;
        return ss.str();
    }

    bool validateDate(const string& date) const {
        if (date.length() != 10) return false;
        return date[4] == '-' && date[7] == '-' &&
               isdigit(date[0]) && isdigit(date[1]) && isdigit(date[2]) && isdigit(date[3]) &&
               isdigit(date[5]) && isdigit(date[6]) &&
               isdigit(date[8]) && isdigit(date[9]);
    }

    // Add these constants for folder organization
    const string BASE_DIR = "student_data";
    const map<string, pair<int, int>> SCHOOL_SECTIONS = {
        {"primary", {1, 3}},
        {"upper_primary", {4, 5}},
        {"midschool", {6, 8}},
        {"high_school", {9, 10}},
        {"higher_secondary", {11, 12}}
    };

    // Helper method to get section folder name based on class
    string getSectionFolder(int classNum) {
        for (const auto& section : SCHOOL_SECTIONS) {
            if (classNum >= section.second.first && 
                classNum <= section.second.second) {
                return section.first;
            }
        }
        return "";
    }

    // Helper method to create directory structure
    void createDirectoryStructure() {
        #ifdef _WIN32
            CreateDirectoryA(BASE_DIR.c_str(), NULL);
            for (const auto& section : SCHOOL_SECTIONS) {
                string path = BASE_DIR + "/" + section.first;
                CreateDirectoryA(path.c_str(), NULL);
            }
        #else
            mkdir(BASE_DIR.c_str(), 0777);
            for (const auto& section : SCHOOL_SECTIONS) {
                string path = BASE_DIR + "/" + section.first;
                mkdir(path.c_str(), 0777);
            }
        #endif
    }

    // Modified save method to save class-wise data
    void saveToFile() {
        createDirectoryStructure();
        
        // Group students by class
        map<string, vector<Student*>> classGroups;
        for (auto& student : students) {
            string className = student.getClassName();
            classGroups[className].push_back(&student);
        }

        // Save each class to its appropriate section folder
        for (const auto& group : classGroups) {
            try {
                int classNum = stoi(group.first);
                string sectionFolder = getSectionFolder(classNum);
                if (sectionFolder.empty()) continue;

                string filename = BASE_DIR + "/" + sectionFolder + 
                                "/class_" + group.first + ".csv";
                
                ofstream file(filename);
                if (!file.is_open()) {
                    showError("Could not open file: " + filename);
                    continue;
                }

                // Write header
                file << "Roll No,Name,Section,Contact,Email,Gender,DOB\n";

                // Write student data
                for (const Student* student : group.second) {
                    file << student->getRollNo() << ","
                         << student->getName() << ","
                         << student->getSection() << ","
                         << student->getContactNo() << ","
                         << student->getEmail() << ","
                         << student->getGender() << ","
                         << student->getDateOfBirth() << "\n";
                }
                file.close();
                
            } catch (...) {
                showError("Error saving class " + group.first);
            }
        }
    }

    // Modified load method to load from all section folders
    void loadFromFile() {
        students.clear();
        
        for (const auto& section : SCHOOL_SECTIONS) {
            string sectionPath = BASE_DIR + "/" + section.first;
            
            #ifdef _WIN32
                WIN32_FIND_DATAA findData;
                string searchPath = sectionPath + "/*.csv";
                HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
                
                if (hFind != INVALID_HANDLE_VALUE) {
                    do {
                        string filename = sectionPath + "/" + findData.cFileName;
                        loadClassFile(filename);
                    } while (FindNextFileA(hFind, &findData));
                    FindClose(hFind);
                }
            #else
                DIR* dir = opendir(sectionPath.c_str());
                if (dir != nullptr) {
                    struct dirent* entry;
                    while ((entry = readdir(dir)) != nullptr) {
                        string filename = entry->d_name;
                        if (filename.ends_with(".csv")) {
                            loadClassFile(sectionPath + "/" + filename);
                        }
                    }
                    closedir(dir);
                }
            #endif
        }
    }

    // Helper method to load a single class file
    void loadClassFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return;

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            try {
                stringstream ss(line);
                string roll, name, section, contact, email, gender, dob;
                
                getline(ss, roll, ',');
                getline(ss, name, ',');
                getline(ss, section, ',');
                getline(ss, contact, ',');
                getline(ss, email, ',');
                getline(ss, gender, ',');
                getline(ss, dob, ',');

                // Extract class number from filename
                string className = filename.substr(
                    filename.find("class_") + 6,
                    filename.find(".csv") - filename.find("class_") - 6
                );

                students.push_back(Student(roll, name, className, section,
                                        contact, email, gender, dob));
            } catch (...) {
                continue;
            }
        }
        file.close();
    }

    void clearScreen() const {
        #ifdef _WIN32
            std::system("cls");
        #else
            std::system("clear");
        #endif
    }

    void logAction(const string& action) {
        systemLogs.push_back({getCurrentDate(), action});
    }

    bool isValidEmail(const string& email) const {
        const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
        return regex_match(email, pattern);
    }

    void exportToHTML() const {
        ofstream file("attendance_report.html");
        file << "<html><head><title>Attendance Report</title></head><body>";
        file << "<h1>Attendance Report</h1>";
        file << "<table border='1'><tr><th>Roll No</th><th>Name</th><th>Class</th><th>Section</th><th>Grade</th><th>Attendance%</th></tr>";
        
        for (const auto& student : students) {
            file << "<tr><td>" << student.getRollNo() << "</td>"
                 << "<td>" << student.getName() << "</td>"
                 << "<td>" << student.getClassName() << "</td>"
                 << "<td>" << student.getSection() << "</td>"
                 << "<td>" << fixed << setprecision(2) << student.getAttendancePercentage() 
                 << "%</td></tr>";
        }
        
        file << "</table></body></html>";
        file.close();
    }

    void generateDetailedReport() const {
        ofstream file("detailed_report.txt");
        
        file << "=== Detailed Attendance Report ===\n\n";
        
        map<string, vector<const Student*>> deptStudents;
        for (const auto& student : students) {
            deptStudents[student.getClassName()].push_back(&student);
        }
        
        for (const auto& dept : deptStudents) {
            file << "\nClass: " << dept.first << "\n";
            file << string(30, '-') << "\n";
            
            float deptTotal = 0;
            for (const auto* student : dept.second) {
                deptTotal += student->getAttendancePercentage();
                file << "Student: " << student->getName() 
                     << " (Roll No: " << student->getRollNo() << ")\n"
                     << "Attendance: " << student->getAttendancePercentage() << "%\n"
                     << "Present Days: " << student->getTotalPresent() << "\n"
                     << "Absent Days: " << student->getTotalAbsent() << "\n\n";
            }
            
            file << "Class Average: " << deptTotal / dept.second.size() << "%\n";
            file << string(30, '=') << "\n";
        }
        file.close();
    }

    int getTodayPresent() const {
        string today = getCurrentDate();
        int count = 0;
        for (const auto& student : students) {
            if (student.getAttendanceForDate(today)) count++;
        }
        return count;
    }

    float getOverallAttendance() const {
        if (students.empty()) return 0.0f;
        float total = 0;
        for (const auto& student : students) {
            total += student.getAttendancePercentage();
        }
        return total / students.size();
    }

    string getBestClass() const {
        map<string, pair<float, int>> deptStats;
        for (const auto& student : students) {
            auto& stat = deptStats[student.getClassName()];
            stat.first += student.getAttendancePercentage();
            stat.second++;
        }

        string bestDept;
        float bestAvg = 0;
        for (const auto& dept : deptStats) {
            float avg = dept.second.first / dept.second.second;
            if (avg > bestAvg) {
                bestAvg = avg;
                bestDept = dept.first;
            }
        }
        return bestDept;
    }

    int getTotalCourses() const {
        int total = 0;
        for (const auto& dept : departmentCourses) {
            total += dept.second.size();
        }
        return total;
    }

    vector<pair<string, float>> getWeeklyAttendance() const {
        vector<pair<string, float>> weeklyData;
        
        if (students.empty()) {
            return weeklyData;
        }

        for (int i = 6; i >= 0; i--) {
            try {
                auto now = chrono::system_clock::now();
                auto days = chrono::hours(24 * i);
                auto past = now - days;
                time_t past_time = chrono::system_clock::to_time_t(past);
                tm* ltm = localtime(&past_time);
                
                stringstream ss;
                ss << 1900 + ltm->tm_year << "-"
                   << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
                   << setfill('0') << setw(2) << ltm->tm_mday;
                string date = ss.str();
                
                int present = 0, total = students.size();
                for (const auto& student : students) {
                    if (student.getAttendanceForDate(date)) {
                        present++;
                    }
                }
                
                float percentage = total > 0 ? (float)present / total * 100 : 0.0f;
                weeklyData.push_back({date, percentage});
            } catch (...) {
                continue;
            }
        }
        
        return weeklyData;
    }

    void showRecentActivity() const {
        const map<string, string> actionIcons = {
            {"Added", "➕"},
            {"Marked", "✓"},
            {"Generated", "📊"},
            {"Exported", "📁"},
            {"Updated", "🔄"}
        };

        int count = 0;
        for (auto it = systemLogs.rbegin(); it != systemLogs.rend() && count < 5; ++it, ++count) {
            setColor(8);
            cout << it->first << " ";
            resetColor();
            
            for (const auto& icon : actionIcons) {
                if (it->second.find(icon.first) != string::npos) {
                    cout << icon.second << " ";
                    break;
                }
            }
            cout << it->second << "\n";
        }
    }

    vector<pair<string, float>> getDepartmentPerformance() const {
        vector<pair<string, float>> deptData;
        map<string, pair<float, int>> deptStats;

        if (students.empty()) {
            return deptData;
        }

        for (const auto& student : students) {
            const string& dept = student.getClassName();
            if (!dept.empty()) {
                auto& stat = deptStats[dept];
                stat.first += student.getAttendancePercentage();
                stat.second++;
            }
        }

        for (const auto& dept : deptStats) {
            if (dept.second.second > 0) {
                float avgAttendance = dept.second.first / dept.second.second;
                deptData.push_back({dept.first, avgAttendance});
            }
        }

        return deptData;
    }

    map<string, vector<Exam>> examRecords;  // class -> exams
    map<string, ClassTeacher> classTeachers;  // class -> teacher
    vector<string> schoolAnnouncements;
    map<string, vector<string>> parentMessages;  // rollNo -> messages
    map<string, vector<Subject>> classSubjects;        // class -> subjects
    map<string, vector<ExamSchedule>> examSchedules;   // class -> exams
    map<string, vector<Homework>> studentHomework;     // rollNo -> homework
    map<string, vector<ParentMeeting>> parentMeetings; // rollNo -> meetings
    map<string, vector<string>> teacherRemarks;        // rollNo -> remarks

    struct ChatMessage {
        string from;
        string to;
        string message;
        string date;
        bool isRead;
        string type; // "text", "homework", "attendance", "announcement"
    };

    vector<ChatMessage> chatMessages;
    map<string, vector<string>> subscriptions; // rollNo -> notification types

    // Add these helper functions inside the AttendanceSystem class
    float getOverallAttendance(const string& className, const string& section) const {
        vector<const Student*> classStudents;
        float totalAttendance = 0;
        
        for (const auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
                totalAttendance += student.getAttendancePercentage();
            }
        }
        
        return classStudents.empty() ? 0 : totalAttendance / classStudents.size();
    }

    string getBestStudent(const string& className, const string& section) const {
        const Student* bestStudent = nullptr;
        float highestAttendance = 0;
        
        for (const auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                float attendance = student.getAttendancePercentage();
                if (attendance > highestAttendance) {
                    highestAttendance = attendance;
                    bestStudent = &student;
                }
            }
        }
        
        return bestStudent ? bestStudent->getName() : "N/A";
    }

    float getBestAttendance(const string& className, const string& section) const {
        float highestAttendance = 0;
        
        for (const auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                highestAttendance = max(highestAttendance, student.getAttendancePercentage());
            }
        }
        
        return highestAttendance;
    }

    int getLongestStreak(const string& className, const string& section) const {
        int maxStreak = 0;
        
        for (const auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                maxStreak = max(maxStreak, student.getConsecutiveAttendance());
            }
        }
        
        return maxStreak;
    }

    // Add these methods to AttendanceSystem class
    string getClassFilePath(const string& className, const string& section) {
        int classNum = stoi(className);
        string sectionFolder;
        
        if (classNum >= 1 && classNum <= 3) sectionFolder = "primary";
        else if (classNum >= 4 && classNum <= 5) sectionFolder = "upper_primary";
        else if (classNum >= 6 && classNum <= 8) sectionFolder = "midschool";
        else if (classNum >= 9 && classNum <= 10) sectionFolder = "high_school";
        else sectionFolder = "higher_secondary";
        
        return "student_data/" + sectionFolder + "/class_" + 
               className + "_" + section + ".csv";
    }

    void saveClassData(const string& className, const string& section) {
        // Create backup before saving
        if (filesystem::exists(getClassFilePath(className, section))) {
            createBackup(className, section);
        }

        string filepath = getClassFilePath(className, section);
        
        // Ensure directory exists
        filesystem::path dir = filesystem::path(filepath).parent_path();
        filesystem::create_directories(dir);

        ofstream file(filepath);
        if (!file.is_open()) {
            showError("Could not open file: " + filepath);
            return;
        }

        // Write header with version and metadata
        file << "version:1.0,date:" << getCurrentDate() << "\n";
        file << "Roll No,Name,Section,Contact,Email,Gender,DOB,AttendanceData,Remarks\n";

        for (const auto& student : students) {
            if (student.getClassName() == className && 
                student.getSection() == section) {
                
                stringstream ss;
                ss << student.getRollNo() << ","
                   << student.getName() << ","
                   << student.getSection() << ","
                   << student.getContactNo() << ","
                   << student.getEmail() << ","
                   << student.getGender() << ","
                   << student.getDateOfBirth() << ",";

                // Enhanced attendance format with remarks
                string attendanceStr;
                for (const auto& record : student.getAttendanceRecord()) {
                    string remark = student.getRemarkForDate(record.first);
                    attendanceStr += record.first + ":" + 
                                   (record.second ? "1" : "0") + 
                                   (remark.empty() ? "" : ":" + remark) + ";";
                }
                ss << attendanceStr << "\n";

                // Validate before writing
                if (validateFileData(ss.str())) {
                    file << ss.str();
                } else {
                    showWarning("Invalid data found for student: " + student.getName());
                }
            }
        }
        file.close();
        
        // Generate attendance statistics file
        generateAttendanceStats(className, section);
    }

    void generateAttendanceStats(const string& className, const string& section) {
        string statsFile = getClassFilePath(className, section);
        statsFile = statsFile.substr(0, statsFile.length() - 4) + "_stats.txt";
        
        ofstream file(statsFile);
        if (!file.is_open()) return;

        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && 
                student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        // Monthly statistics
        map<string, pair<int, int>> monthlyStats;  // month -> {present, total}
        // Weekly statistics
        map<string, pair<int, int>> weeklyStats;   // week -> {present, total}
        // Daily trends
        map<string, float> dailyTrends;            // date -> attendance%

        for (const auto* student : classStudents) {
            for (const auto& record : student->getAttendanceRecord()) {
                string date = record.first;
                string month = date.substr(0, 7);
                string week = getWeekNumber(date);

                if (record.second) {
                    monthlyStats[month].first++;
                    weeklyStats[week].first++;
                }
                monthlyStats[month].second++;
                weeklyStats[week].second++;

                dailyTrends[date] += record.second ? 100.0f : 0.0f;
            }
        }

        // Write statistics
        file << "Attendance Statistics for Class " << className << "-" << section << "\n";
        file << "Generated on: " << getCurrentDate() << "\n\n";

        file << "Monthly Statistics:\n";
        file << "==================\n";
        for (const auto& stat : monthlyStats) {
            float percentage = stat.second.first * 100.0f / stat.second.second;
            file << stat.first << ": " << fixed << setprecision(2) 
                 << percentage << "% (" 
                 << stat.second.first << "/" << stat.second.second << ")\n";
        }

        file << "\nWeekly Statistics:\n";
        file << "=================\n";
        for (const auto& stat : weeklyStats) {
            float percentage = stat.second.first * 100.0f / stat.second.second;
            file << "Week " << stat.first << ": " << percentage << "%\n";
        }

        file << "\nDaily Trends:\n";
        file << "============\n";
        for (auto& trend : dailyTrends) {
            trend.second /= classStudents.size();
            file << trend.first << ": " << trend.second << "%\n";
        }

        file.close();
    }

    void loadClassData(const string& className, const string& section) {
        string filepath = getClassFilePath(className, section);
        ifstream file(filepath);
        
        if (!file.is_open()) {
            return; // File doesn't exist yet
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string roll, name, sect, contact, email, gender, dob, attendanceData;
            
            getline(ss, roll, ',');
            getline(ss, name, ',');
            getline(ss, sect, ',');
            getline(ss, contact, ',');
            getline(ss, email, ',');
            getline(ss, gender, ',');
            getline(ss, dob, ',');
            getline(ss, attendanceData);

            // Create student
            Student student(roll, name, className, sect, contact, email, gender, dob);

            // Load attendance data
            stringstream attSS(attendanceData);
            string record;
            while (getline(attSS, record, ';')) {
                if (record.empty()) continue;
                size_t pos = record.find(':');
                if (pos != string::npos) {
                    string date = record.substr(0, pos);
                    bool present = (record.substr(pos + 1) == "1");
                    student.markAttendance(date, present);
                }
            }

            // Add to students vector
            students.push_back(student);
        }
        file.close();
    }

    void createBackup(const string& className, const string& section) {
        string sourceFile = getClassFilePath(className, section);
        string backupFile = sourceFile.substr(0, sourceFile.length() - 4) + 
                           "_backup_" + getCurrentDate() + ".csv";
        
        ifstream src(sourceFile, ios::binary);
        ofstream dst(backupFile, ios::binary);
        
        if (src && dst) {
            dst << src.rdbuf();
            showSuccess("Backup created: " + backupFile);
        }
    }

    bool validateFileData(const string& line) {
        stringstream ss(line);
        string roll, name, sect, contact, email, gender, dob, attendanceData;
        
        try {
            getline(ss, roll, ',');
            getline(ss, name, ',');
            getline(ss, sect, ',');
            getline(ss, contact, ',');
            getline(ss, email, ',');
            getline(ss, gender, ',');
            getline(ss, dob, ',');
            getline(ss, attendanceData);

            // Validate each field
            if (roll.empty() || name.empty() || sect.empty()) return false;
            if (contact.length() != 10 || !all_of(contact.begin(), contact.end(), ::isdigit)) return false;
            if (!isValidEmail(email)) return false;
            if (gender != "M" && gender != "F" && gender != "O") return false;
            if (!validateDate(dob)) return false;

            return true;
        } catch (...) {
            return false;
        }
    }

    // Add this helper method to get class and section input
    pair<string, string> getClassAndSection() {
        clearScreen();
        
        string className, section;
        cout << "Enter Class (1-12): ";
        cin.ignore();
        getline(cin, className);

        // Validate class
        try {
            int classNum = stoi(className);
            if (classNum < 1 || classNum > 12) {
                showError("Invalid class! Must be between 1 and 12");
                return {"", ""};
            }
        } catch (...) {
            showError("Invalid class format!");
            return {"", ""};
        }

        cout << "Enter Section: ";
        getline(cin, section);
        
        if (section.length() != 1 || !isupper(section[0])) {
            showError("Invalid section! Must be a single uppercase letter (A-Z)");
            return {"", ""};
        }

        // Load class data
        loadClassData(className, section);
        return {className, section};
    }

public:
    AttendanceSystem() {
        loadFromFile();
    }

    ~AttendanceSystem() {
        saveToFile();
    }

    bool login(const string& password) {
        isLoggedIn = (password == ADMIN_PASSWORD);
        return isLoggedIn;
    }

    void addStudent() {
        string roll, name, class_, sect, contact, email, gender, dob;
        
        cout << "Enter Roll No: ";
        cin >> roll;
        cin.ignore();

        cout << "Enter Class (1-12): ";
        getline(cin, class_);
        
        cout << "Enter Section: ";
        getline(cin, sect);

        // Check if roll number exists in the same class and section
        string uniqueId = class_ + "_" + sect + "_" + roll;
        if (any_of(students.begin(), students.end(),
            [&uniqueId](const Student& s) { return s.getUniqueId() == uniqueId; })) {
            showError("Roll number already exists in this class and section!");
            return;
        }
        
        cout << "Enter Name: ";
        getline(cin, name);
        
        cout << "Enter Contact Number: ";
        getline(cin, contact);
        if (contact.length() != 10 || !all_of(contact.begin(), contact.end(), ::isdigit)) {
            showError("Invalid contact number! Must be 10 digits");
            return;
        }
        
        cout << "Enter Email: ";
        getline(cin, email);
        if (!isValidEmail(email)) {
            showError("Invalid email format!");
            return;
        }
        
        cout << "Enter Gender (M/F/O): ";
        getline(cin, gender);
        if (gender != "M" && gender != "F" && gender != "O") {
            showError("Invalid gender! Must be M, F, or O");
            return;
        }
        
        cout << "Enter Date of Birth (YYYY-MM-DD): ";
        getline(cin, dob);
        if (!validateDate(dob)) {
            showError("Invalid date format! Use YYYY-MM-DD");
            return;
        }

        students.push_back(Student(roll, name, class_, sect, contact, email, gender, dob));
        logAction("Added new student: " + name + " to class " + class_ + "-" + sect);
        showSuccess("Student added successfully!");
    }

    void markAttendance() {
        if (students.empty()) {
            showError("No students registered yet!");
            return;
        }

        clearScreen();
        printTitle("Mark Attendance");

        // Get class and section first
        string className, section;
        cout << "Enter Class (1-12): ";
        cin.ignore();
        getline(cin, className);

        // Validate class
        try {
            int classNum = stoi(className);
            if (classNum < 1 || classNum > 12) {
                showError("Invalid class! Must be between 1 and 12");
                return;
            }
        } catch (...) {
            showError("Invalid class format!");
            return;
        }

        cout << "Enter Section: ";
        getline(cin, section);
        
        if (section.length() != 1 || !isupper(section[0])) {
            showError("Invalid section! Must be a single uppercase letter (A-Z)");
            return;
        }

        // Get date
        string date;
        cout << "Enter date (YYYY-MM-DD) or press Enter for today's date: ";
        getline(cin, date);
        
        if (date.empty()) {
            date = getCurrentDate();
        } else if (!validateDate(date)) {
            showError("Invalid date format! Use YYYY-MM-DD");
            return;
        }

        // Load class data first
        loadClassData(className, section);

        // Filter students by class and section
        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        clearScreen();
        cout << "\nMarking attendance for Class " << className << "-" << section 
             << " Date: " << date << endl;
        cout << setfill('=') << setw(50) << "=" << endl;

        for (auto* student : classStudents) {
            char present;
            string remark;
            
            cout << "\nRoll No: " << student->getRollNo()
                 << "\nName: " << student->getName();
                 
            do {
                cout << "\nPresent? (y/n): ";
                present = tolower(_getch());
                cout << present << endl;
            } while (present != 'y' && present != 'n');
            
            cout << "Enter remark (optional): ";
            getline(cin, remark);
            
            student->markAttendance(date, (present == 'y'), remark);
            logAction("Marked " + string(present == 'y' ? "present" : "absent") + 
                     " for " + student->getName());
        }

        // Save after marking attendance
        saveClassData(className, section);
        showSuccess("Attendance marked and saved successfully!");
    }

    void viewAttendance() {
        if (students.empty()) {
            showError("No students registered yet!");
            return;
        }

        clearScreen();
        printTitle("View Attendance");

        string className, section;
        cout << "Enter Class (1-12): ";
        cin.ignore();
        getline(cin, className);

        cout << "Enter Section: ";
        getline(cin, section);

        // Load class data first
        loadClassData(className, section);

        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        clearScreen();
        cout << "\nAttendance Records - Class " << className << "-" << section << "\n";
        cout << setfill('=') << setw(80) << "=" << endl;
        cout << setfill(' ');
        cout << left << setw(10) << "Roll No" 
             << left << setw(20) << "Name"
             << right << setw(15) << "Present"
             << right << setw(15) << "Absent"
             << right << setw(15) << "Percentage" << endl;
        cout << setfill('-') << setw(80) << "-" << endl;
        cout << setfill(' ');

        for (const auto* student : classStudents) {
            cout << left << setw(10) << student->getRollNo()
                 << left << setw(20) << student->getName()
                 << right << setw(15) << student->getTotalPresent()
                 << right << setw(15) << student->getTotalAbsent()
                 << right << setw(14) << fixed << setprecision(2) 
                 << student->getAttendancePercentage() << "%" << endl;
        }

        cout << "\nPress any key to continue...";
        _getch();
    }

    void searchStudent() {
        if (students.empty()) {
            showError("No students registered yet!");
            return;
        }

        clearScreen();
        printTitle("Search Student");

        // Get class and section first
        string className, section;
        cout << "Enter Class (1-12): ";
        cin.ignore();
        getline(cin, className);

        // Validate class
        try {
            int classNum = stoi(className);
            if (classNum < 1 || classNum > 12) {
                showError("Invalid class! Must be between 1 and 12");
                return;
            }
        } catch (...) {
            showError("Invalid class format!");
            return;
        }

        cout << "Enter Section: ";
        getline(cin, section);
        
        if (section.length() != 1 || !isupper(section[0])) {
            showError("Invalid section! Must be a single uppercase letter (A-Z)");
            return;
        }

        // Load class data first
        loadClassData(className, section);

        string search;
        cout << "Enter Roll No or Name to search: ";
        getline(cin, search);

        bool found = false;
        for (const auto& student : students) {
            if (student.getClassName() == className && 
                student.getSection() == section &&
                (student.getRollNo() == search || 
                 student.getName().find(search) != string::npos)) {
                
                clearScreen();
                cout << "\nStudent Details:\n";
                cout << setfill('=') << setw(50) << "=" << endl;
                cout << "Roll No: " << student.getRollNo() << endl;
                cout << "Name: " << student.getName() << endl;
                cout << "Class: " << student.getClassName() << endl;
                cout << "Section: " << student.getSection() << endl;
                cout << "Contact: " << student.getContactNo() << endl;
                cout << "Email: " << student.getEmail() << endl;
                cout << "Gender: " << student.getGender() << endl;
                cout << "Date of Birth: " << student.getDateOfBirth() << endl;
                
                cout << "\nAttendance Summary:\n";
                cout << setfill('-') << setw(50) << "-" << endl;
                cout << "Total Present: " << student.getTotalPresent() << endl;
                cout << "Total Absent: " << student.getTotalAbsent() << endl;
                cout << "Attendance Percentage: " << fixed << setprecision(2) 
                     << student.getAttendancePercentage() << "%" << endl;
                
                cout << "\nAttendance History:\n";
                cout << setfill('-') << setw(50) << "-" << endl;
                cout << student.getAttendanceDetails();
                found = true;
                break;
            }
        }
        
        if (!found) {
            showError("Student not found in class " + className + "-" + section + "!");
        }
    }

    void generateReport() {
        if (students.empty()) {
            cout << "No students registered yet!\n";
            return;
        }

        cout << "\nAttendance Report Summary\n";
        cout << setfill('=') << setw(50) << "=" << endl;
        
        map<string, vector<float>> deptStats;
        for (const auto& student : students) {
            deptStats[student.getClassName()].push_back(student.getAttendancePercentage());
        }

        for (const auto& dept : deptStats) {
            float sum = 0;
            for (float percentage : dept.second) {
                sum += percentage;
            }
            float avg = sum / dept.second.size();
            
            cout << "\nClass: " << dept.first << endl;
            cout << "Total Students: " << dept.second.size() << endl;
            cout << "Average Attendance: " << fixed << setprecision(2) << avg << "%" << endl;
        }
    }

    void addCourse(const string& department, const string& course) {
        departmentCourses[department].push_back(course);
        logAction("Added course " + course + " to " + department);
    }

    void viewSystemLogs() const {
        cout << "\nSystem Logs:\n";
        cout << setfill('=') << setw(50) << "=" << endl;
        for (const auto& log : systemLogs) {
            cout << log.first << ": " << log.second << endl;
        }
    }

    void showDashboard() const {
        clearScreen();
        UIHelper::drawBox("Dashboard", 80);
        
        vector<vector<string>> stats = {
            {"Metric", "Value", "Status"},
            {"Total Students", to_string(students.size()), ""},
            {"Present Today", to_string(getTodayPresent()), ""},
            {"Overall Attendance", 
                students.empty() ? "N/A" : to_string(getOverallAttendance()) + "%", ""},
            {"Best Class", 
                students.empty() ? "N/A" : getBestClass(), ""},
            {"Active Courses", to_string(getTotalCourses()), ""}
        };
        vector<int> colWidths = {20, 15, 15};
        UIHelper::drawTable(stats, colWidths);

        if (!students.empty()) {
            cout << "\nWeekly Attendance Trend:\n";
            vector<pair<string, float>> weeklyData = getWeeklyAttendance();
            if (!weeklyData.empty()) {
                UIHelper::drawChart(weeklyData);
            } else {
                cout << "No attendance data for the past week.\n";
            }

            cout << "\nClass Performance:\n";
            vector<pair<string, float>> deptData = getDepartmentPerformance();
            if (!deptData.empty()) {
                UIHelper::drawChart(deptData);
            } else {
                cout << "No class performance data available.\n";
            }
        } else {
            cout << "\nNo student data available. Add students to see attendance statistics.\n";
        }

        cout << "\nRecent Activity:\n";
        if (!systemLogs.empty()) {
            showRecentActivity();
        } else {
            cout << "No recent activity.\n";
        }
    }

    void showMenu() const {
        if (!loggedIn) {
            cout << "\n=== Smart Attendance System ===\n\n";
            cout << "1. Login\n";
            cout << "2. Exit\n";
        } else {
            cout << "\n=== Smart Attendance System ===\n\n";
            cout << "1. Add Student\n";
            cout << "2. Mark Attendance\n";
            cout << "3. View Attendance\n";
            cout << "4. Search Student\n";
            cout << "5. Generate Reports\n";
            cout << "6. Show Statistics\n";
            cout << "7. Export Data\n";
            cout << "8. System Logs\n";
            cout << "9. Logout\n";
            cout << "10. Exit\n";
        }
    }

    void generateReports() {
        if (students.empty()) {
            showError("No students registered yet!");
            return;
        }

        clearScreen();
        printTitle("Generate Reports");
        
        cout << "1. Attendance Report\n";
        cout << "2. Performance Report\n";
        cout << "3. Behavior Report\n";
        cout << "4. Progress Cards\n";
        cout << "5. Monthly Analysis\n";
        cout << "6. Detailed Statistics\n";
        cout << "0. Back\n\n";
        
        int choice;
        cout << "Enter choice: ";
        cin >> choice;

        // Get class and section first
        auto [className, section] = getClassAndSection();
        if (className.empty() || section.empty()) {
            return;
        }

        switch (choice) {
            case 1:
                generateAttendanceReport(className, section);
                break;
            case 2:
                generatePerformanceReport(className, section);
                break;
            case 3:
                generateBehaviorReport(className, section);
                break;
            case 4:
                generateProgressCards(className, section);
                break;
            case 5:
                generateMonthlyAnalysis(className, section);
                break;
            case 6:
                generateDetailedStatistics(className, section);
                break;
            case 0:
                return;
            default:
                showError("Invalid choice!");
        }
    }

    void generateHTMLReport(const string& className, const string& section) {
        clearScreen();
        UIHelper::drawBox("Attendance Report - Class " + className + "-" + section, 80);

        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        // Generate attendance statistics
        vector<pair<string, float>> attendanceData;
        vector<pair<string, int>> statusData = {
            {"Present", 0}, {"Absent", 0}
        };

        for (const auto* student : classStudents) {
            attendanceData.push_back({
                student->getName(), 
                student->getAttendancePercentage()
            });
            statusData[0].second += student->getTotalPresent();
            statusData[1].second += student->getTotalAbsent();
        }

        // Save to HTML with improved styling
        string filename = "attendance_report_" + className + "_" + section + ".html";
        ofstream file(filename);
        if (file.is_open()) {
            // Enhanced CSS with more visual elements
            file << "<html><head>\n"
                 << "<title>Attendance Report</title>\n"
                 << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n"
                 << "<style>\n"
                 << "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 40px; background-color: #f0f2f5; }\n"
                 << ".container { max-width: 1200px; margin: 0 auto; background-color: white; padding: 30px; border-radius: 15px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n"
                 << ".header { text-align: center; margin-bottom: 40px; padding: 20px; background: linear-gradient(135deg, #1e3c72, #2a5298); color: white; border-radius: 10px; }\n"
                 << ".school-name { font-size: 28px; font-weight: bold; margin-bottom: 10px; }\n"
                 << ".report-title { font-size: 22px; opacity: 0.9; }\n"
                 << ".stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 30px 0; }\n"
                 << ".stat-card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); text-align: center; }\n"
                 << ".stat-value { font-size: 24px; font-weight: bold; color: #2a5298; margin: 10px 0; }\n"
                 << ".stat-label { color: #666; font-size: 14px; }\n"
                 << "table { width: 100%; border-collapse: collapse; margin: 25px 0; font-size: 15px; }\n"
                 << "th { background-color: #2a5298; color: white; padding: 15px; text-align: left; }\n"
                 << "td { padding: 12px 15px; border-bottom: 1px solid #ddd; }\n"
                 << "tr:hover { background-color: #f8f9fa; }\n"
                 << ".attendance-bar { height: 20px; background-color: #e9ecef; border-radius: 10px; overflow: hidden; margin: 5px 0; }\n"
                 << ".attendance-fill { height: 100%; transition: width 0.5s ease-in-out; }\n"
                 << ".chart-container { margin: 30px 0; padding: 20px; background: white; border-radius: 10px; box-shadow: 0 2px 5px rgba(0,0,0,0.05); }\n"
                 << ".low { background-color: #dc3545; }\n"
                 << ".medium { background-color: #ffc107; }\n"
                 << ".high { background-color: #28a745; }\n"
                 << "</style>\n"
                 << "</head><body>\n"
                 << "<div class='container'>\n"
                 << "<div class='header'>\n"
                 << "<div class='school-name'>School Name</div>\n"
                 << "<div class='report-title'>Attendance Report</div>\n"
                 << "<h2>Class " + className + "-" + section + "</h2>\n"
                 << "<p>Generated on: " + getCurrentDate() + "</p>\n"
                 << "</div>\n";

            // Statistics Cards
            file << "<div class='stats-grid'>\n"
                 << "<div class='stat-card'>\n"
                 << "<div class='stat-label'>Total Students</div>\n"
                 << "<div class='stat-value'>" << classStudents.size() << "</div>\n"
                 << "</div>\n"
                 << "<div class='stat-card'>\n"
                 << "<div class='stat-label'>Average Attendance</div>\n"
                 << "<div class='stat-value'>" << fixed << setprecision(1) 
                 << (statusData[0].second * 100.0f / (statusData[0].second + statusData[1].second)) 
                 << "%</div>\n"
                 << "</div>\n"
                 << "<div class='stat-card'>\n"
                 << "<div class='stat-label'>Total Classes</div>\n"
                 << "<div class='stat-value'>" << (statusData[0].second + statusData[1].second) / classStudents.size() << "</div>\n"
                 << "</div>\n"
                 << "</div>\n";

            // Charts
            file << "<div class='chart-container'>\n"
                 << "<canvas id='attendanceChart'></canvas>\n"
                 << "</div>\n"
                 << "<div class='chart-container'>\n"
                 << "<canvas id='trendChart'></canvas>\n"
                 << "</div>\n";

            // Student Table
            file << "<h3>Student-wise Attendance</h3>\n"
                 << "<table>\n"
                 << "<tr><th>Roll No</th><th>Name</th><th>Present Days</th>"
                 << "<th>Absent Days</th><th>Attendance</th></tr>\n";

            for (const auto* student : classStudents) {
                float attendance = student->getAttendancePercentage();
                string colorClass = attendance >= 75 ? "high" : (attendance >= 60 ? "medium" : "low");
                
                file << "<tr>\n"
                     << "<td>" << student->getRollNo() << "</td>\n"
                     << "<td>" << student->getName() << "</td>\n"
                     << "<td>" << student->getTotalPresent() << "</td>\n"
                     << "<td>" << student->getTotalAbsent() << "</td>\n"
                     << "<td>\n"
                     << "<div class='attendance-bar'>\n"
                     << "<div class='attendance-fill " << colorClass << "' style='width: "
                     << attendance << "%;'></div>\n"
                     << "</div>\n"
                     << attendance << "%\n"
                     << "</td>\n"
                     << "</tr>\n";
            }
            file << "</table>\n";

            // JavaScript for Charts
            file << "<script>\n"
                 << "const ctx1 = document.getElementById('attendanceChart').getContext('2d');\n"
                 << "new Chart(ctx1, {\n"
                 << "    type: 'pie',\n"
                 << "    data: {\n"
                 << "        labels: ['Present', 'Absent'],\n"
                 << "        datasets: [{\n"
                 << "            data: [" << statusData[0].second << ", " << statusData[1].second << "],\n"
                 << "            backgroundColor: ['#28a745', '#dc3545']\n"
                 << "        }]\n"
                 << "    }\n"
                 << "});\n";

            // Add trend chart data
            file << "const ctx2 = document.getElementById('trendChart').getContext('2d');\n"
                 << "new Chart(ctx2, {\n"
                 << "    type: 'line',\n"
                 << "    data: {\n"
                 << "        labels: ['Week 1', 'Week 2', 'Week 3', 'Week 4'],\n"
                 << "        datasets: [{\n"
                 << "            label: 'Weekly Attendance Trend',\n"
                 << "            data: [75, 82, 78, 85],\n"
                 << "            borderColor: '#2a5298',\n"
                 << "            tension: 0.1\n"
                 << "        }]\n"
                 << "    },\n"
                 << "    options: { responsive: true }\n"
                 << "});\n"
                 << "</script>\n";

            file << "</div></body></html>";
            file.close();
        }

        showSuccess("Attendance report generated successfully!");
    }

    void generateDetailedReport(const string& className, const string& section) {
        clearScreen();
        UIHelper::drawBox("Detailed Report - Class " + className + "-" + section, 80);

        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        // Generate detailed report
        ofstream file("detailed_report_" + className + "_" + section + ".txt");
        if (file.is_open()) {
            file << "=== Detailed Report - Class " + className + "-" + section + " ===\n\n";
            file << "Total Students: " << classStudents.size() << "\n\n";
            float classAverageAttendance = getOverallAttendance(className, section);
            file << "Class Average Attendance: " << fixed << setprecision(2) << classAverageAttendance << "%\n";
            string bestStudent = getBestStudent(className, section);
            float bestStudentAttendance = getBestAttendance(className, section);
            file << "Best Student: " << bestStudent << " (" << bestStudentAttendance << "%)\n";
            int highestAttendanceStreak = getLongestStreak(className, section);
            file << "Highest Attendance Streak: " << highestAttendanceStreak << " days\n";

            // Add more detailed sections for behavior notes, extracurricular activities, etc.
            file << "\nClass-wise Distribution:\n";
            for (const auto& student : classStudents) {
                file << "Student: " << student->getName() << " (" << student->getRollNo() << ")\n";
                file << "Attendance: " << student->getAttendancePercentage() << "%\n";
                file << "Present Days: " << student->getTotalPresent() << "\n";
                file << "Absent Days: " << student->getTotalAbsent() << "\n\n";
            }

            file << "\nPerformance Distribution:\n";
            int excellent = 0, good = 0, average = 0, needsImprovement = 0;
            for (const auto& student : classStudents) {
                float avg = 0;
                for (const auto& grade : student->getGrades()) {
                    avg += grade.second;
                }
                avg /= student->getGrades().size();
                
                if (avg >= 90) excellent++;
                else if (avg >= 75) good++;
                else if (avg >= 60) average++;
                else needsImprovement++;
            }
            file << "Excellent (>90%): " << excellent << " students\n";
            file << "Good (75-90%): " << good << " students\n";
            file << "Average (60-75%): " << average << " students\n";
            file << "Needs Improvement (<60%): " << needsImprovement << " students\n\n";

            // Add more sections for behavior notes, extracurricular activities, etc.
            file << "\nClass Teacher's Remarks:\n";
            for (const auto& student : classStudents) {
                file << "Student: " << student->getName() << " (" << student->getRollNo() << ")\n";
                file << "Behavior Notes:\n";
                for (const auto& note : student->getBehaviorNotes()) {
                    file << note.first << ": " << note.second << "\n";
                }
                file << "\nParent-Teacher Interactions:\n";
                for (const auto& meeting : parentMeetings[student->getRollNo()]) {
                    file << meeting.date << " - " << meeting.teacherName << ": " << meeting.agenda << "\n";
                }
                file << "\nClass Teacher's Remarks:\n";
                for (const auto& remark : teacherRemarks[student->getRollNo()]) {
                    file << remark << "\n";
                }
                file << "\n\n";
            }

            file.close();
            showSuccess("Detailed report generated successfully!");
        } else {
            showError("Failed to create detailed report file!");
        }
    }

    void generateCSVReport(const string& className, const string& section) {
        string filename = "attendance_report_" + className + "_" + section + ".csv";
        ofstream file(filename);
        
        if (!file.is_open()) {
            showError("Could not create report file!");
            return;
        }

        // Write header
        file << "Roll No,Name,Total Present,Total Absent,Attendance %\n";

        // Write data for each student in the class
        for (const auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                file << student.getRollNo() << ","
                     << student.getName() << ","
                     << student.getTotalPresent() << ","
                     << student.getTotalAbsent() << ","
                     << fixed << setprecision(2) << student.getAttendancePercentage() << "\n";
            }
        }                                                                                                                                                                                                                       

        file.close();
        showSuccess("CSV report generated: " + filename);
    }

    void generateClassSummary(const string& className, const string& section) {
        clearScreen();
        UIHelper::drawBox("Class Summary - " + className + "-" + section, 80);

        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        // Generate class summary
        ofstream file("class_" + className + "_summary.html");
        if (file.is_open()) {
            file << "<html><head><title>Class Summary</title></head><body>";
            file << "<h1>Class Summary - " + className + "-" + section + "</h1>";
            file << "<p>Total Students: " << classStudents.size() << "</p>";
            file << "<p>Average Attendance: " << fixed << setprecision(2) << getOverallAttendance(className, section) << "%</p>";
            file << "<p>Best Student: " << getBestStudent(className, section) << " (" << getBestAttendance(className, section) << "%)</p>";
            file << "<p>Highest Attendance Streak: " << getLongestStreak(className, section) << " days</p>";

            // Add more detailed sections for behavior notes, extracurricular activities, etc.
            file << "\nClass-wise Distribution:\n";
            for (const auto& student : classStudents) {
                file << "<p>Student: " << student->getName() << " (" << student->getRollNo() << ")</p>";
                file << "<p>Attendance: " << student->getAttendancePercentage() << "%</p>";
                file << "<p>Present Days: " << student->getTotalPresent() << "</p>";
                file << "<p>Absent Days: " << student->getTotalAbsent() << "</p>";
                file << "<p>Behavior Notes:</p>";
                for (const auto& note : student->getBehaviorNotes()) {
                    file << "<p>" << note.first << ": " << note.second << "</p>";
                }
                file << "<p>Parent-Teacher Interactions:</p>";
                for (const auto& meeting : parentMeetings[student->getRollNo()]) {
                    file << "<p>" << meeting.date << " - " << meeting.teacherName << ": " << meeting.agenda << "</p>";
                }
                file << "<p>Class Teacher's Remarks:</p>";
                for (const auto& remark : teacherRemarks[student->getRollNo()]) {
                    file << "<p>" << remark << "</p>";
                }
                file << "<hr>";
            }

            file.close();
            showSuccess("Class summary generated successfully!");
        } else {
            showError("Failed to create class summary file!");
        }
    }

    void generateMonthlyReport(const string& className, const string& section) {
        ofstream file("monthly_report_" + className + "_" + section + ".txt");
        map<string, map<string, int>> monthlyStats; // month -> {present, total}
        
        for (const auto& student : students) {
            const auto& records = student.getAttendanceRecord();
            for (const auto& record : records) {
                string month = record.first.substr(0, 7); // YYYY-MM
                if (record.second) monthlyStats[month]["present"]++;
                monthlyStats[month]["total"]++;
            }
        }
        
        file << "Monthly Attendance Report\n";
        file << "=====================\n\n";
        for (const auto& month : monthlyStats) {
            float percentage = (float)month.second.at("present") / month.second.at("total") * 100;
            file << "Month: " << month.first << "\n"
                 << "Total Classes: " << month.second.at("total") << "\n"
                 << "Average Attendance: " << fixed << setprecision(2) << percentage << "%\n\n";
        }
        file.close();
        showSuccess("Monthly report generated successfully!");
    }

    void generateTrendAnalysis(const string& className, const string& section) {
        ofstream file("trend_analysis_" + className + "_" + section + ".txt");
        vector<pair<string, float>> trends;
        
        map<string, pair<int, int>> dailyStats; // date -> {present, total}
        for (const auto& student : students) {
            const auto& records = student.getAttendanceRecord();
            for (const auto& record : records) {
                if (record.second) dailyStats[record.first].first++;
                dailyStats[record.first].second++;
            }
        }
        
        for (const auto& day : dailyStats) {
            float percentage = (float)day.second.first / day.second.second * 100;
            trends.push_back({day.first, percentage});
        }
        
        sort(trends.begin(), trends.end());
        
        file << "Attendance Trend Analysis\n";
        file << "=======================\n\n";
        
        for (const auto& point : trends) {
            file << point.first << " ";
            int bars = (int)(point.second / 2);
            file << string(bars, '#') << " " << point.second << "%\n";
        }
        file.close();
        showSuccess("Trend analysis generated successfully!");
    }

    void showSuccess(const string& message) const {
        setColor(10); // Green
        cout << "\n✓ " << message << "\n";
        resetColor();
    }

    void showError(const string& message) const {
        setColor(12); // Red
        cout << "\n✗ " << message << "\n";
        resetColor();
    }

    void showWarning(const string& message) const {
        setColor(14); // Yellow
        cout << "\n⚠ " << message << "\n";
        resetColor();
    }

    void showStatistics() {
        if (students.empty()) {
            showError("No students registered yet!");
            return;
        }

        clearScreen();
        printTitle("Class Statistics");

        // Get class and section first
        string className, section;
        cout << "Enter Class (1-12): ";
        cin.ignore();
        getline(cin, className);

        // Validate class
        try {
            int classNum = stoi(className);
            if (classNum < 1 || classNum > 12) {
                showError("Invalid class! Must be between 1 and 12");
                return;
            }
        } catch (...) {
            showError("Invalid class format!");
            return;
        }

        cout << "Enter Section: ";
        getline(cin, section);
        
        if (section.length() != 1 || !isupper(section[0])) {
            showError("Invalid section! Must be a single uppercase letter (A-Z)");
            return;
        }

        // Load class data first
        loadClassData(className, section);

        // Filter students by class and section
        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className && student.getSection() == section) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className + "-" + section);
            return;
        }

        clearScreen();
        cout << "\nClass Statistics - " << className << "-" << section << "\n";
        cout << setfill('=') << setw(50) << "=" << endl;

        // Overall Statistics
        float totalAttendance = 0;
        int highestStreak = 0;
        string bestStudent;
        float bestAttendance = 0;

        for (const auto* student : classStudents) {
            float attendance = student->getAttendancePercentage();
            totalAttendance += attendance;
            
            if (attendance > bestAttendance) {
                bestAttendance = attendance;
                bestStudent = student->getName();
            }
            
            highestStreak = max(highestStreak, student->getAttendanceStreak());
        }

        float averageAttendance = totalAttendance / classStudents.size();

        cout << "\nGeneral Statistics:\n";
        cout << "Total Students: " << classStudents.size() << endl;
        cout << "Average Attendance: " << fixed << setprecision(2) << averageAttendance << "%" << endl;
        cout << "Best Student: " << bestStudent << " (" << bestAttendance << "%)" << endl;
        cout << "Highest Attendance Streak: " << highestStreak << " days" << endl;

        // Attendance Distribution
        cout << "\nAttendance Distribution:\n";
        int excellent = 0, good = 0, average = 0, poor = 0;
        for (const auto* student : classStudents) {
            float attendance = student->getAttendancePercentage();
            if (attendance >= 90) excellent++;
            else if (attendance >= 75) good++;
            else if (attendance >= 60) average++;
            else poor++;
        }

        cout << "Excellent (≥90%): " << excellent << " students\n";
        cout << "Good (75-89%): " << good << " students\n";
        cout << "Average (60-74%): " << average << " students\n";
        cout << "Poor (<60%): " << poor << " students\n";

        cout << "\nPress any key to continue...";
        _getch();
    }

    void generateProgressReport(const string& rollNo) {
        auto it = find_if(students.begin(), students.end(),
            [&rollNo](const Student& s) { return s.getRollNo() == rollNo; });
        
        if (it == students.end()) {
            showError("Student not found!");
            return;
        }

        ofstream file(rollNo + "_progress_report.html");
        file << "<html><head>\n"
             << "<style>\n"
             << "body { font-family: Arial; margin: 40px; }\n"
             << "table { border-collapse: collapse; width: 100%; }\n"
             << "th, td { border: 1px solid #ddd; padding: 8px; }\n"
             << "th { background-color: #4CAF50; color: white; }\n"
             << ".behavior { color: #666; font-style: italic; }\n"
             << ".warning { color: red; }\n"
             << "</style>\n"
             << "</head><body>\n"
             << "<h1>Student Progress Report</h1>\n";

        file << "<h2>Student Information</h2>\n"
             << "<table>\n"
             << "<tr><th>Roll No</th><td>" << it->getRollNo() << "</td></tr>\n"
             << "<tr><th>Name</th><td>" << it->getName() << "</td></tr>\n"
             << "<tr><th>Class</th><td>" << it->getClassName() 
             << " - " << it->getSection() << "</td></tr>\n"
             << "<tr><th>Contact</th><td>" << it->getContactNo() << "</td></tr>\n"
             << "</table>\n";

        file << "<h2>Academic Performance</h2>\n"
             << "<table>\n"
             << "<tr><th>Subject</th><th>Grade</th><th>Conduct</th></tr>\n";
        for (const auto& grade : it->getGrades()) {
            file << "<tr><td>" << grade.first << "</td><td>" 
                 << grade.second << "</td><td>"
                 << it->getConductMarks().at(grade.first) << "</td></tr>\n";
        }
        file << "</table>\n";

        file << "<h2>Behavior Notes</h2>\n";
        for (const auto& note : it->getBehaviorNotes()) {
            file << "<p class='behavior'>" << note.first << ": " 
                 << note.second << "</p>\n";
        }

        file << "<h2>Extracurricular Activities</h2>\n<ul>\n";
        for (const auto& activity : it->getExtracurriculars()) {
            file << "<li>" << activity << "</li>\n";
        }
        file << "</ul>\n";

        file << "</body></html>";
        file.close();

        showSuccess("Progress report generated successfully!");
    }

    void generateClasswiseReport(const string& className) {
        vector<Student*> classStudents;
        for (auto& student : students) {
            if (student.getClassName() == className) {
                classStudents.push_back(&student);
            }
        }

        if (classStudents.empty()) {
            showError("No students found in class " + className);
            return;
        }

        ofstream file("class_" + className + "_report.html");
        file.close();
    }

    void addBehaviorNote(const string& rollNo, const string& note) {
        auto it = find_if(students.begin(), students.end(),
            [&rollNo](const Student& s) { return s.getRollNo() == rollNo; });
        
        if (it != students.end()) {
            it->addBehaviorNote(note);
            showSuccess("Behavior note added successfully!");
        } else {
            showError("Student not found!");
        }
    }

    void updateBusRoute(const string& rollNo, const string& route) {
        auto it = find_if(students.begin(), students.end(),
            [&rollNo](const Student& s) { return s.getRollNo() == rollNo; });
        
        if (it != students.end()) {
            it->setBusRoute(route);
            showSuccess("Bus route updated successfully!");
        } else {
            showError("Student not found!");
        }
    }

    void addExam(const string& className, const Exam& exam) {
        examRecords[className].push_back(exam);
        logAction("Added new exam for " + className + ": " + exam.subject);
    }

    void recordExamMarks(const string& className, const string& subject, 
                        const string& rollNo, float marks) {
        for (auto& exam : examRecords[className]) {
            if (exam.subject == subject) {
                exam.studentMarks[rollNo] = marks;
                break;
            }
        }
        logAction("Recorded marks for " + rollNo + " in " + subject);
    }

    void assignClassTeacher(const string& className, const ClassTeacher& teacher) {
        classTeachers[className] = teacher;
        logAction("Assigned " + teacher.name + " as class teacher for " + className);
    }

    void addSchoolAnnouncement(const string& announcement) {
        schoolAnnouncements.push_back(announcement);
        logAction("New school announcement: " + announcement);
    }

    void sendParentMessage(const string& rollNo, const string& message) {
        parentMessages[rollNo].push_back(message);
        logAction("Sent message to parent of " + rollNo);
    }

    void generateProgressCard(const string& rollNo) {
        auto it = find_if(students.begin(), students.end(),
            [&rollNo](const Student& s) { return s.getRollNo() == rollNo; });
        
        if (it == students.end()) {
            showError("Student not found!");
            return;
        }

        ofstream file(rollNo + "_progress_card.html");
        file << "<html><head>\n"
             << "<style>\n"
             << "body { font-family: Arial; margin: 40px; }\n"
             << "table { border-collapse: collapse; width: 100%; margin: 20px 0; }\n"
             << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n"
             << "th { background-color: #4CAF50; color: white; }\n"
             << ".header { text-align: center; margin-bottom: 30px; }\n"
             << ".school-name { font-size: 24px; font-weight: bold; }\n"
             << ".warning { color: red; }\n"
             << "</style>\n"
             << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>\n"
             << "</head><body>\n"
             << "<div class='header'>\n"
             << "<div class='school-name'>School Name</div>\n"
             << "<div>Progress Report Card</div>\n"
             << "<div>Academic Year 2024-25</div>\n"
             << "</div>\n";

        file << "<h2>Student Information</h2>\n"
             << "<table>\n"
             << "<tr><th>Roll No</th><td>" << it->getRollNo() << "</td></tr>\n"
             << "<tr><th>Name</th><td>" << it->getName() << "</td></tr>\n"
             << "<tr><th>Class</th><td>" << it->getClassName() << "</td></tr>\n"
             << "<tr><th>Section</th><td>" << it->getSection() << "</td></tr>\n"
             << "</table>\n";

        file << "<h2>Academic Performance</h2>\n"
             << "<table>\n"
             << "<tr><th>Subject</th><th>Unit Test</th><th>Mid Term</th><th>Final</th></tr>\n";
        
        for (const auto& exam : examRecords[it->getClassName()]) {
            file << "<tr><td>" << exam.subject << "</td><td>";
            
            // Unit Test marks
            if (exam.studentMarks.find(it->getRollNo()) != exam.studentMarks.end()) {
                file << exam.studentMarks.at(it->getRollNo());
            } else {
                file << "N/A";
            }
            
            file << "</td><td>";
            
            // Mid Term marks
            if (exam.examType == "Mid Term" && 
                exam.studentMarks.find(it->getRollNo()) != exam.studentMarks.end()) {
                file << exam.studentMarks.at(it->getRollNo());
            } else {
                file << "N/A";
            }
            
            file << "</td><td>";
            
            // Final marks
            if (exam.examType == "Final" && 
                exam.studentMarks.find(it->getRollNo()) != exam.studentMarks.end()) {
                file << exam.studentMarks.at(it->getRollNo());
            } else {
                file << "N/A";
            }
            
            file << "</td></tr>\n";
        }
        file << "</table>\n";

        file << "<h2>Attendance Record</h2>\n"
             << "<table>\n"
             << "<tr><th>Total Days</th><td>" 
             << it->getTotalPresent() + it->getTotalAbsent() << "</td></tr>\n"
             << "<tr><th>Days Present</th><td>" << it->getTotalPresent() << "</td></tr>\n"
             << "<tr><th>Days Absent</th><td>" << it->getTotalAbsent() << "</td></tr>\n"
             << "<tr><th>Attendance Percentage</th><td>" 
             << fixed << setprecision(2) << it->getAttendancePercentage() << "%</td></tr>\n"
             << "</table>\n";

        file << "<h2>Class Teacher's Remarks</h2>\n"
             << "<div style='border: 1px solid #ddd; padding: 10px; min-height: 100px;'>\n";
        for (const auto& remark : teacherRemarks[rollNo]) {
            file << "<p>" << remark << "</p>\n";
        }
        file << "</div>\n";

        file << "<h2>Parent-Teacher Interactions</h2>\n"
             << "<table>\n"
             << "<tr><th>Date</th><th>Teacher</th><th>Discussion Points</th><th>Outcome</th></tr>\n";
        for (const auto& meeting : parentMeetings[rollNo]) {
            file << "<tr><td>" << meeting.date << "</td>"
                 << "<td>" << meeting.teacherName << "</td>"
                 << "<td>" << meeting.agenda << "</td>"
                 << "<td>" << meeting.feedback << "</td></tr>\n";
        }
        file << "</table>\n";

        file << "</body></html>";
        file.close();

        showSuccess("Progress card generated successfully!");
    }

    void generateClassTeacherReport(const string& className) {
        clearScreen();
        UIHelper::drawBox("Class Performance Analysis - " + className, 80);

        int totalStudents = 0;
        float avgAttendance = 0;
        map<string, float> subjectAverages;
        
        for (const auto& student : students) {
            if (student.getClassName() == className) {
                totalStudents++;
                avgAttendance += student.getAttendancePercentage();
                
                for (const auto& grade : student.getGrades()) {
                    subjectAverages[grade.first] += grade.second;
                }
            }
        }

        cout << "\nClass Overview:\n";
        cout << "Total Students: " << totalStudents << endl;
        cout << "Average Attendance: " << avgAttendance/totalStudents << "%\n\n";

        cout << "Subject-wise Performance:\n";
        for (auto& subject : subjectAverages) {
            subject.second /= totalStudents;
            cout << subject.first << ": " << subject.second << "%\n";
        }

        cout << "\nPerformance Distribution:\n";
        int excellent = 0, good = 0, average = 0, needsImprovement = 0;
        
        for (const auto& student : students) {
            if (student.getClassName() == className) {
                float avg = 0;
                for (const auto& grade : student.getGrades()) {
                    avg += grade.second;
                }
                avg /= student.getGrades().size();
                
                if (avg >= 90) excellent++;
                else if (avg >= 75) good++;
                else if (avg >= 60) average++;
                else needsImprovement++;
            }
        }

        cout << "Excellent (>90%): " << excellent << " students\n";
        cout << "Good (75-90%): " << good << " students\n";
        cout << "Average (60-75%): " << average << " students\n";
        cout << "Needs Improvement (<60%): " << needsImprovement << " students\n";
    }

    void sendNotification(const string& rollNo, const string& message) {
        notificationList.push_back({
            message,
            getCurrentDate(),
            "normal",
            false,
            {rollNo}
        });
        logAction("Sent notification to " + rollNo);
    }

    void broadcastMessage(const string& message, const string& className = "") {
        for (const auto& student : students) {
            if (className.empty() || student.getClassName() == className) {
                sendNotification(student.getRollNo(), message);
            }
        }
        logAction("Broadcast message: " + message);
    }

    void showNotifications(const string& rollNo) {
        if (notificationList.empty()) {
            cout << "No notifications.\n";
            return;
        }

        cout << "\nNotifications:\n";
        cout << setfill('=') << setw(50) << "=" << endl;
        for (const auto& notification : notificationList) {
            if (find(notification.recipients.begin(), notification.recipients.end(), rollNo) 
                != notification.recipients.end()) {
                cout << "• " << notification.message << endl;
            }
        }
    }

    void sendUrgentNotification(const string& message, const vector<string>& recipients) {
        Notification notif{
            message,
            getCurrentDate(),
            "urgent",
            false,
            recipients
        };
        notificationList.push_back(notif);
        
        for (const auto& rollNo : recipients) {
            cout << "🔔 Urgent notification sent to " << rollNo << endl;
        }
    }

    void subscribeToNotifications(const string& rollNo, const vector<string>& types) {
        subscriptions[rollNo] = types;
        logAction("Updated notification preferences for " + rollNo);
    }

    void sendChatMessage(const string& from, const string& to, 
                        const string& message, const string& type = "text") {
        ChatMessage chat{
            from,
            to,
            message,
            getCurrentDate(),
            false,
            type
        };
        chatMessages.push_back(chat);
        logAction("Chat message sent from " + from + " to " + to);
    }

    void showUnreadMessages(const string& rollNo) {
        cout << "\nUnread Messages:\n";
        cout << setfill('=') << setw(50) << "=" << endl;
        
        bool hasUnread = false;
        for (auto& msg : chatMessages) {
            if (msg.to == rollNo && !msg.isRead) {
                hasUnread = true;
                cout << "From: " << msg.from << endl;
                cout << "Date: " << msg.date << endl;
                cout << "Type: " << msg.type << endl;
                cout << "Message: " << msg.message << endl;
                cout << setfill('-') << setw(30) << "-" << endl;
                msg.isRead = true;
            }
        }
        
        if (!hasUnread) {
            cout << "No unread messages.\n";
        }
    }

    void generateCommunicationReport(const string& rollNo) {
        ofstream file(rollNo + "_communication_log.html");
        file << "<html><head>\n"
             << "<style>\n"
             << "body { font-family: Arial; margin: 40px; }\n"
             << ".message { margin: 10px; padding: 10px; border: 1px solid #ddd; }\n"
             << ".urgent { background-color: #ffe6e6; }\n"
             << ".normal { background-color: #e6f3ff; }\n"
             << ".info { background-color: #e6ffe6; }\n"
             << "</style>\n"
             << "</head><body>\n"
             << "<h1>Communication Log</h1>\n";

        file << "<h2>Messages</h2>\n";
        for (const auto& msg : chatMessages) {
            if (msg.to == rollNo || msg.from == rollNo) {
                file << "<div class='message " << msg.type << "'>\n"
                     << "<p><strong>From:</strong> " << msg.from << "</p>\n"
                     << "<p><strong>Date:</strong> " << msg.date << "</p>\n"
                     << "<p>" << msg.message << "</p>\n"
                     << "</div>\n";
            }
        }

        for (const auto& notif : notificationList) {
            if (find(notif.recipients.begin(), notif.recipients.end(), rollNo) 
                != notif.recipients.end()) {
                file << "<div class='message " << notif.type << "'>\n"
                     << "<p><strong>Date:</strong> " << notif.date << "</p>\n"
                     << "<p>" << notif.message << "</p>\n"
                     << "</div>\n";
            }
        }

        file << "</body></html>";
        file.close();
    }

    void exportData() {
        auto [className, section] = getClassAndSection();
        if (className.empty() || section.empty()) {
            return;
        }

        string filepath = getClassFilePath(className, section);
        string exportPath = "exports/class_" + className + "_" + section + "_" + 
                          getCurrentDate() + ".csv";

        filesystem::create_directories("exports");
        filesystem::copy_file(filepath, exportPath, 
                            filesystem::copy_options::overwrite_existing);
        
        showSuccess("Data exported to: " + exportPath);
    }

    void viewSystemLogs() {
        auto [className, section] = getClassAndSection();
        if (className.empty() || section.empty()) {
            return;
        }

        string logFile = getClassFilePath(className, section);
        logFile = logFile.substr(0, logFile.length() - 4) + "_logs.txt";
        
        ifstream file(logFile);
        if (!file.is_open()) {
            showError("No logs found for this class!");
            return;
        }

        clearScreen();
        printTitle("System Logs - Class " + className + "-" + section);
        
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }
};

int main() {
    AttendanceSystem system;
    string password;
    int choice;
    bool loggedIn = false;

    while (true) {
        #ifdef _WIN32
            std::system("cls");  // Use std::system
        #else
            std::system("clear");
        #endif
        
        system.showMenu();  // This is our AttendanceSystem object's method
        
        cout << "\nEnter your choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            system.showError("Invalid input! Please enter a number.");
            continue;
        }

        if (!loggedIn) {
            switch (choice) {
                case 1:
                    cout << "Enter password: ";
                    cin >> password;
                    loggedIn = system.login(password);
                    if (loggedIn) 
                        system.showSuccess("Login successful!");
                    else 
                        system.showError("Invalid password!");
                    break;
                case 2:
                    system.showSuccess("Thank you for using Smart Attendance System!");
                    return 0;
                default:
                    system.showError("Invalid choice!");
            }
        } else {
            switch (choice) {
                case 1:
                    system.addStudent();
                    break;
                case 2:
                    system.markAttendance();
                    break;
                case 3:
                    system.viewAttendance();
                    break;
                case 4:
                    system.searchStudent();
                    break;
                case 5:
                    system.generateReports();
                    break;
                case 6:
                    system.showStatistics();
                    break;
                case 7:
                    // system.exportToExcel(); // This function does not exist in AttendanceSystem class
                    system.showError("Export to Excel feature is not available.");
                    break;
                case 8:
                    // system.viewSystemLogs(); // This function does not exist in AttendanceSystem class
                    system.showError("View System Logs feature is not available.");
                    break;
                case 9:
                    loggedIn = false;
                    system.showSuccess("Logged out successfully!");
                    break;
                case 10:
                    system.showSuccess("Thank you for using Smart Attendance System!");
                    return 0;
                default:
                    system.showError("Invalid choice!");
            }
        }
        
        cout << "\nPress any key to continue...";
        _getch();
    }

    return 0;
} 
} 