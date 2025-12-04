#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>

using namespace std;

// Abstract Base Class
class Person {
protected:
    string name;
    int age;
    string contact;
public:
    Person(string n, int a, string c) : name(n), age(a), contact(c) {}
    virtual void display() = 0;
    virtual ~Person() {}
    string getName() const { return name; }
};

// Patient Class
class Patient : public Person {
private:
    int patientID;
    string bloodGroup;
    vector<string> medicalHistory;
    static int nextID;
public:
    Patient(string n, int a, string c, string bg) 
        : Person(n, a, c), patientID(nextID++), bloodGroup(bg) {}
    
    void addRecord(string record) { medicalHistory.push_back(record); }
    
    void display() override {
        cout << "\nPatient ID: " << patientID
             << "\nName: " << name
             << "\nAge: " << age
             << "\nBlood Group: " << bloodGroup
             << "\nMedical History:\n";
        for (auto& record : medicalHistory) cout << "- " << record << endl;
    }
    
    int getID() const { return patientID; }
};
int Patient::nextID = 1000;

// Doctor Class
class Doctor : public Person {
private:
    string specialization;
    map<string, bool> schedule; // time -> available
public:
    Doctor(string n, int a, string c, string spec) 
        : Person(n, a, c), specialization(spec) {
        // Initialize schedule (9AM-5PM)
        for (int h = 9; h <= 17; h++) {
            schedule[to_string(h) + ":00"] = true;
        }
    }
    
    bool bookSlot(string time) {
        if (schedule.find(time) != schedule.end() && schedule[time]) {
            schedule[time] = false;
            return true;
        }
        return false;
    }
    
    void display() override {
        cout << "\nDr. " << name 
             << "\nSpecialization: " << specialization
             << "\nAvailable Slots:\n";
        for (auto& slot : schedule) {
            if (slot.second) cout << "- " << slot.first << endl;
        }
    }
    
    string getSpecialization() const { return specialization; }
};

// Hospital System
class Hospital {
private:
    vector<Patient> patients;
    vector<Doctor> doctors;
    priority_queue<pair<int, pair<int, string>>> emergencyQueue; // priority, patientID, spec
    
public:
    Hospital() {
        // Initialize with sample doctors
        doctors.emplace_back("Dr. Smith", 45, "111-2222", "Cardiology");
        doctors.emplace_back("Dr. Johnson", 38, "333-4444", "Neurology");
        doctors.emplace_back("Dr. Williams", 50, "555-6666", "General");
    }
    
    void addPatient() {
        string name, contact, bg;
        int age;
        cout << "Enter patient details:\nName: ";
        getline(cin >> ws, name);
        cout << "Age: "; cin >> age;
        cout << "Contact: "; cin >> contact;
        cout << "Blood Group: "; cin >> bg;
        
        patients.emplace_back(name, age, contact, bg);
        cout << "Patient registered! ID: " << patients.back().getID() << endl;
    }
    
    void scheduleAppointment() {
        if (patients.empty()) {
            cout << "No patients registered!\n";
            return;
        }
        
        int patientID;
        cout << "Enter patient ID: "; cin >> patientID;
        
        auto it = find_if(patients.begin(), patients.end(), 
            [patientID](const Patient& p) { return p.getID() == patientID; });
        
        if (it == patients.end()) {
            cout << "Patient not found!\n";
            return;
        }
        
        cout << "Available Specializations:\n";
        for (auto& doc : doctors) cout << "- " << doc.getSpecialization() << endl;
        
        string spec;
        cout << "Enter required specialization: ";
        cin >> spec;
        
        bool emergency;
        cout << "Is this an emergency? (1/0): ";
        cin >> emergency;
        
        if (emergency) {
            emergencyQueue.push({10, {patientID, spec}}); // Highest priority
            cout << "Added to emergency queue!\n";
        } else {
            for (auto& doc : doctors) {
                if (doc.getSpecialization() == spec) {
                    doc.display();
                    string time;
                    cout << "Enter preferred time: ";
                    cin >> time;
                    if (doc.bookSlot(time)) {
                        cout << "Appointment booked!\n";
                        return;
                    }
                }
            }
            cout << "No available slots found!\n";
        }
    }
    
    void processEmergency() {
        if (emergencyQueue.empty()) {
            cout << "No emergencies in queue!\n";
            return;
        }
        
        auto emergency = emergencyQueue.top();
        emergencyQueue.pop();
        
        int patientID = emergency.second.first;
        string spec = emergency.second.second;
        
        cout << "Processing emergency for patient ID: " << patientID << endl;
        
        for (auto& doc : doctors) {
            if (doc.getSpecialization() == spec) {
                for (int h = 9; h <= 17; h++) {
                    string time = to_string(h) + ":00";
                    if (doc.bookSlot(time)) {
                        cout << "Emergency appointment with " << doc.getName() 
                             << " at " << time << endl;
                        return;
                    }
                }
            }
        }
        cout << "No doctors available for emergency!\n";
    }
    
    void displayPatients() {
        if (patients.empty()) {
            cout << "No patients registered!\n";
            return;
        }
        for (auto& p : patients) p.display();
    }
};

// Menu Interface
void showMenu() {
    cout << "\n=== HOSPITAL MANAGEMENT SYSTEM ==="
         << "\n1. Register Patient"
         << "\n2. Schedule Appointment"
         << "\n3. Process Emergency"
         << "\n4. View All Patients"
         << "\n5. Exit"
         << "\nEnter choice: ";
}

int main() {
    Hospital hms;
    
    while (true) {
        showMenu();
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1: hms.addPatient(); break;
            case 2: hms.scheduleAppointment(); break;
            case 3: hms.processEmergency(); break;
            case 4: hms.displayPatients(); break;
            case 5: return 0;
            default: cout << "Invalid choice!\n";
        }
    }
}
