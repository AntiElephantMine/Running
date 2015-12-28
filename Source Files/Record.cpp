#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip> //for setw(n)
#include <algorithm>
#include <numeric>
#include <exception>
#include <stdexcept>
#include "Record.h"

using namespace std;

const double kmpermile = 1.609344;
const double mileperkm = 0.621371192;
const double epsilon = 0.00001;






double absolute(const double& x){
    return x < 0 ? -x : x;
}

bool doubleCompare(const double& x, const double& y, const char& c){
    if(c == '<'){
        return (x < y && absolute(x-y) >= epsilon);
    }
    if(c == '>'){
        return (x > y && absolute(x-y) >= epsilon);
    }
    if(c == '=='){
        return (absolute(x-y) < epsilon);
    }
    else throw runtime_error("Invalid units specified for doubleCompare().");
} //needed to compare doubles because floats may be declared non-equal when logically we believe they are

int round(int x, const double& y){ //y to the nearest x

    if(y > std::numeric_limits<int>::max()) return std::numeric_limits<int>::max();

    int interval = x;
    while(x < y)
        x += interval;

    if((x - static_cast<double>(interval)/2) <= y) return x;
    else return (x-interval);
}

string makePlural(const string& word, double ctr = 0, const string& ending = "s"){
    return (ctr == 1 ? word : word+ending);
}

string unitWord(const char& c){
    if(c == 'm') return "mile";
    if(c == 'k') return "kilometer";
    else throw runtime_error("Invalid units specified for unitWord()");
}









ostream& operator<<(ostream& os, const Record& rhs) {
    os << rhs.date << " " << rhs.classify << " " << rhs.distance << " " << rhs.hours << " " << rhs.minutes
       << " " << rhs.seconds << " " << rhs.minperunit << " " << rhs.units;

    return os;
}

std::istream& operator>>(istream& is, Record& rhs){
    Record backUp(rhs);
    is >> rhs.date >> rhs.classify >> rhs.distance >> rhs.hours >> rhs.minutes >> rhs.seconds >> rhs.minperunit >> rhs.units;
    if(!is)
        rhs = backUp;
    else if(!rhs.validRecord()){
        is.setstate(istream::failbit);
        rhs = backUp;
    }
    return is;
}

bool operator==(const Record& lhs, const Record& rhs){
    return (lhs.distance == rhs.distance) && (lhs.minperunit == rhs.minperunit) && (lhs.hours == rhs.hours)
        && (lhs.minutes == rhs.minutes) && (lhs.seconds == rhs.seconds) && (lhs.date == rhs.date)
        && (lhs.classify == rhs.classify) && (lhs.units == rhs.units);
}

bool operator!=(const Record& lhs, const Record& rhs){
    return !(lhs == rhs);
}

Record operator+(const Record& lhs, const Record& rhs){
    Record sum = lhs;
    sum += rhs;
    return sum;
}






void sortby(vector<Record>& inputrecords, const char& c){
    if(tolower(c) == 'd') //distance
        sort(inputrecords.begin(), inputrecords.end(),
             [](const Record& a, const Record& b){return doubleCompare(a.distance, b.distance, '>');});

    else if(tolower(c) == 't') //time
        sort(inputrecords.begin(), inputrecords.end(),
             [](const Record& a, const Record& b){return doubleCompare(a.distance*a.minperunit, b.distance*b.minperunit, '>');});

    else if(tolower(c) == 's') //speed
        sort(inputrecords.begin(), inputrecords.end(),
             [](const Record& a, const Record& b){return doubleCompare(a.minperunit, b.minperunit, '<');});
}







Record::Record(char u, double d, unsigned h, unsigned m, unsigned s): distance(d), hours(h), minutes(m), seconds(s), units(tolower(u)) {
    if(distance == 0) minperunit = 0;
    else minperunit = (hours*60 + minutes + static_cast<double>(seconds)/60)/distance;

    time_t now = time(0); //WORK OUT TODAY'S DATE
    tm* timeinfo = localtime(&now);
    char datearray[80];
    strftime(datearray, 80, "%d-%b-%Y", timeinfo); //APPEALING DATE FORMAT
    date = datearray;

    classify = "V.LONG";
    if(distance < 12.0) classify = "LONG";
    if(distance < 8.0) classify = "MEDIUM";
    if(distance < 4.0) classify = "SHORT";

    if(!validRecord())
        throw runtime_error("Invalid initialisation of Record.");
}









Record& Record::operator+=(const Record& rhs){
    Record temp = rhs; //we need to convert the record to the appropriate units
    if(units != temp.units)
        temp.convertUnits(units);

    date = "";
    classify = "TOTAL";

    if(distance+temp.distance == 0) minperunit = 0;
    else minperunit = ((distance*minperunit) + (temp.distance*temp.minperunit))/(distance+temp.distance);

    distance += temp.distance;

    seconds += temp.seconds;
    minutes += (seconds/60);
    seconds %= 60;

    minutes += temp.minutes;
    hours += (minutes/60);
    minutes %= 60;

    hours += temp.hours;

    if(!validRecord()) //it should be valid but just for good measure
        throw runtime_error("Invalid initialisation of Record.");

    return *this;
}


ostream& Record::plainResult(ostream& os) const {
    os << "You ran " << distance << " " << makePlural(unitWord(units), distance) << " in " << hours
       << ":" << (minutes < 10 ? "0" : "") << minutes << ":" << (seconds < 10 ? "0" : "") << seconds
       << " giving a pace of " << minperunit << " minutes per " << unitWord(units) << ".";

    os << "\nAlternatively this is " << 1/minperunit << " "
       << makePlural(unitWord(units)) << " per minute.";

    return os;
}

ostream& Record::raceTime(ostream& os, double racedistance, const char& raceunits) const { //raceunits is the units of the desired race

    if(racedistance < 0) throw runtime_error("Invalid distance specified for function Record::raceTime().");
    if((tolower(raceunits) != 'm') && (tolower(raceunits) != 'k')) throw runtime_error("Invalid units specified for function Record::raceTime().");

    if(tolower(raceunits) != units){
        if(tolower(raceunits) == 'm') racedistance *= kmpermile;
        else racedistance *= mileperkm;
    }

    if(racedistance*minperunit >= 60){
        int roundedTime = round(1, racedistance*minperunit); //round nearest minute
        os << roundedTime/60 << makePlural(" hour", roundedTime/60) << " and "
           << roundedTime - (roundedTime/60)*60 << makePlural(" minute", roundedTime - (roundedTime/60)*60)
           << ".";
    }

    else{
        int roundedTime = round(1, racedistance*minperunit*60); //round nearest second
        os << roundedTime/60 << makePlural(" minute", roundedTime/60) << " and "
           << roundedTime - (roundedTime/60)*60 << makePlural(" second", roundedTime - (roundedTime/60)*60)
           << ".";
    }

    return os;
}

ostream& Record::tableDisplay(ostream& os) const {
    ostringstream holder;
    holder << hours << ":" << (minutes < 10 ? "0" : "") << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;

    os << setprecision(2) << fixed << setw(15) << date << setw(10) << classify << setw(15) << distance << setw(10)
       << holder.str() << setw(13) << minperunit;

    return os;
}

Record& Record::convertUnits(const char& c){
    if((tolower(c) != 'm') && (tolower(c) != 'k')) throw runtime_error("Invalid units specified for Record::convertUnits().");

    if(tolower(c) != units){
        if(tolower(c) == 'k'){
            units = 'k';
            minperunit *= mileperkm;
            distance *= kmpermile;
        }
        else if (tolower(c) == 'm'){
            units = 'm';
            minperunit *= kmpermile;
            distance *= mileperkm;
        }
    }

    return *this;
}

bool Record::validRecord() const {
    return !((minutes >= 60) || (seconds >= 60) || (units != 'm' && units != 'k')
            || (distance < 0) || (distance == 0 && minperunit != 0));
}




