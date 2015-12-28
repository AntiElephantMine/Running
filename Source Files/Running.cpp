#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> //for for_each
#include <ctime> //for date
#include <iomanip>
#include <exception>
#include <stdexcept>
#include "Record.h"

using namespace std;

const string recordfile = "D:\\HDD Documents\\HDD Users\\Stefan\\Documents\\C++\\Side Projects\\Running\\Record Files\\runningrecords.txt";
const string backupfile = "D:\\HDD Documents\\HDD Users\\Stefan\\Documents\\C++\\Side Projects\\Running\\Record Files\\runningrecordsbackup.txt";

istream& clearAndIgnoreLine(istream&);

int main(int argc, char* argv[]){

    if(argc > 2 && (tolower(argv[1][1]) == 'd' && (tolower(argv[2][1]) == 'm' || tolower(argv[2][1]) == 'k'))){ //DISPLAY MODE
        ifstream input(recordfile);
        if(!input){
            cout << "Display failed: Could not open " + recordfile + "." << endl;
            return -1;
        }

        vector<Record> inputRecords;

        for(string temp; getline(input, temp); ){ //every line should be a record
            istringstream is(temp);
            Record tempRec;
            if(!(is >> tempRec)){
                cout << "\nWARNING: I/O error occurred while reading file. Some data not displayed." << endl;
                break;
            }
            inputRecords.push_back(tempRec.convertUnits(argv[2][1]));
        }

        if(argc > 3) sortby(inputRecords, argv[3][1]);

        cout << "\n" << setw(15) << "DATE" << setw(10) << "CLASS" << setw(15)
             << "DISTANCE (" + string(tolower(argv[2][1]) == 'm' ? "M" : "KM") + ")" << setw(10)
             << "TIME" << setw(13) << "MIN/" + string(tolower(argv[2][1]) == 'm' ? "MILE" : "KM")
             << "\n" << endl;

        Record sum;
        for_each(inputRecords.cbegin(), inputRecords.cend(), [&sum](const Record& r){ sum += r; r.tableDisplay(cout) << endl;});
        sum.convertUnits(argv[2][1]).tableDisplay(cout << endl) << endl;
        return 0;

    }








    if(argc > 1 && tolower(argv[1][1]) == 'r'){ //CONTINUOUS RECORDING MODE FROM USER OR FILE

        ofstream output(recordfile, ofstream::app);
        if(!output){
            cout << "\nRecording failed: Could not open " + recordfile + "." << endl;
            return -1;
        }


        if(argc > 2){ //AUTO-RECORD OPTION
            if(argv[2] == recordfile){
                cout << "\nSERIOUS ERROR: Output and Input file are the same." << endl;
                return -1;
            }
            ifstream autoinput(argv[2]);
            if(!autoinput){
                cout << "\nAuto-recording failed: Could not open " + string(argv[2]) + "." << endl;
                return -1;
            }

            for(string temp; getline(autoinput, temp); )
                output << temp << endl;

            cout << "\nAuto-record complete." << endl;
            return 0;
        }



        cout << "\nContinuous recording mode, format required: units distance hours minutes seconds.\n" << endl;

        string units;
        double distance = 0;
        unsigned hours = 0, minutes = 0, seconds = 0;

        for(string temp; getline(cin, temp); ){ //every line should be a record
            istringstream is(temp);
            if(!(is >> units >> distance >> hours >> minutes >> seconds)){
                cout << "INPUT STREAM ERROR." << endl;
                continue;
            }
            try{
                output << Record(units[0], distance, hours, minutes, seconds) << endl;
                cout << "RECORD SAVED." << endl;
            } catch (runtime_error err) {
                cout << "ERROR: " << err.what() << endl;
            }

        }
        return 0;
    }







    if(argc > 1 && tolower(argv[1][1]) == 'b'){ //BACK UP DATA MODE
        ifstream input(recordfile);
        if(!input){
            cout << "\nBackup failed: Could not open " + recordfile + "." << endl;
            return -1;
        }

        ofstream output(backupfile); //erases everything in backupfile, so make sure recordfile is open first
        if(!output){
            cout << "\nBackup failed: Could not open " + backupfile + "." << endl;
            return -1;
        }

        for(string temp; getline(input, temp); )
            output << temp << endl;

        cout << "\nBackup to " + backupfile + " complete." << endl;
        return 0;
    }






    if(argc > 1 && tolower(argv[1][1]) == 'h'){ //DISPLAY HELP MODE
        cout << "\nOptions include:" << endl;
        cout << "\n-d for data display: \n\t-m: Output in miles \n\t-k: Output in km"
                "\n\t\t-s: Sort by speed \n\t\t-t: Sort by time \n\t\t-d: Sort by distance"
             << "\n\n-r for continuous recording mode (input format: units distance hour minute seconds):"
                "\n\t-optional filename for auto-recording (e.g. ProgrammingFiles\\backup.txt)"
             << "\n\n-b to backup currently saved data." << endl;

        return 0;
    }





    cout << "Welcome, Stefan, to your own running tracker. (Add -h to display additional options)." << endl;

    while(true){ //NORMAL OPERATION IF HERE IS REACHED

        try{

            string units; //ASK USER FOR RECORD DATA
            cout << "\nAre you working in kilometers or miles?" << endl;
            while(!(cin >> units) || (tolower(units[0]) != 'm' && tolower(units[0]) != 'k')){
                clearAndIgnoreLine(cin);
                cout << "Please enter valid units (kilometers or miles)." << endl;
            }
            clearAndIgnoreLine(cin);

            double distance = 0;
            cout << "\nHow far did you run?" << endl;
            while(!(cin >> distance) || (distance < 0)){
                clearAndIgnoreLine(cin);
                cout << "Please enter a valid distance." << endl;
            }
            clearAndIgnoreLine(cin);

            unsigned hours = 0, minutes = 0, seconds = 0;
            cout << "\nIn what time? (Enter hours, minutes, seconds separated by spaces)" << endl;
            while(!(cin >> hours >> minutes >> seconds) || ((minutes >= 60) || (seconds >= 60))){
                clearAndIgnoreLine(cin);
                cout << "Please enter valid hours, minutes and seconds." << endl;
            }
            clearAndIgnoreLine(cin);



            Record currentRecord(units[0], distance, hours, minutes, seconds); //CREATE RECORD



            currentRecord.plainResult(cout << endl) << endl; //SHOW PLAIN RESULTS
            cout << "\nAt this pace you would: " << endl;
            cout << "\nRun 5 kilometers in: ";
            currentRecord.raceTime(cout, 5, 'k');
            cout << "\nRun a half-marathon in: ";
            currentRecord.raceTime(cout, 13.1094, 'm');
            cout << "\nRun a marathon in: ";
            currentRecord.raceTime(cout, 26.2188, 'm') << endl;




            string raceAnswer = "n"; //RACETIMES
            cout << "\nCalculate a race time using this mile/min? (Y or N)" << endl;
            while(!(cin >> raceAnswer) || (tolower(raceAnswer[0]) != 'n' && tolower(raceAnswer[0]) != 'y')){
                clearAndIgnoreLine(cin);
                cout << "Enter a valid answer." << endl;
            }
            clearAndIgnoreLine(cin);

            while(tolower(raceAnswer[0]) == 'y'){
                cout << "\nEnter the race distance and the units (m or k)." << endl;

                double raceDistance = 0;
                string raceUnits;
                while(!(cin >> raceDistance >> raceUnits) || (raceDistance < 0) || (tolower(raceUnits[0]) != 'm' && tolower(raceUnits[0]) != 'k')){
                      clearAndIgnoreLine(cin);
                      cout << "Please enter valid distance and units." << endl;
                }
                clearAndIgnoreLine(cin);

                cout << "\nAt this pace you would run " << raceDistance << " "
                     << (tolower(raceUnits[0]) == 'k' ? "kilometer" : "mile")
                     << (raceDistance == 1 ? "" : "s") << " in ";

                currentRecord.raceTime(cout, raceDistance, raceUnits[0]) << endl;

                cout << "\nCalculate another race time? (Y or N)" << endl;
                while(!(cin >> raceAnswer) || (tolower(raceAnswer[0]) != 'n' && tolower(raceAnswer[0]) != 'y')){
                    clearAndIgnoreLine(cin);
                    cout << "Enter a valid answer." << endl;
                }
                clearAndIgnoreLine(cin);
            }




            string recordAnswer = "n"; //RECORDING
            cout << "\nRecord this data? (Y or N)" << endl;
            while(!(cin >> recordAnswer) || (tolower(recordAnswer[0]) != 'n' && tolower(recordAnswer[0]) != 'y')){
                clearAndIgnoreLine(cin);
                cout << "Enter a valid answer." << endl;
            }
            clearAndIgnoreLine(cin);

            if(tolower(recordAnswer[0]) == 'y'){
                ofstream output(recordfile, ofstream::app);
                if(output) output << currentRecord << endl;
                else cout << "\nRecording failed: Could not open " + recordfile + "." << endl;
            }




            string repeatAnswer = "n"; //REPEATING
            cout << "\nEnter another record? (Y or N)" << endl;
            while(!(cin >> repeatAnswer) || (tolower(repeatAnswer[0]) != 'n' && tolower(repeatAnswer[0]) != 'y')){
                clearAndIgnoreLine(cin);
                cout << "Enter a valid answer." << endl;
            }
            clearAndIgnoreLine(cin);
            if(tolower(repeatAnswer[0]) != 'y') return 0;




        } catch (runtime_error err){
            cout << "\nERROR: " << err.what() << endl;
            string repeatAnswer = "n";
            cout << "\nTry again? (Y or N)" << endl;
            while(!(cin >> repeatAnswer) || (tolower(repeatAnswer[0]) != 'n' && tolower(repeatAnswer[0]) != 'y')){
                clearAndIgnoreLine(cin);
                cout << "Enter a valid answer." << endl;
            }
            clearAndIgnoreLine(cin);
            if(tolower(repeatAnswer[0]) != 'y') return 0;
        }
    }
}

istream& clearAndIgnoreLine(istream& is){
    is.clear();
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return is;
}


/* IMPROVEMENTS:

1: Edit/Delete records mode.
2. Make this interactive (ncurses diplay, select certain records for editing or calculating)
3: Improve Exception handling.
4: Search for errors.
*/

