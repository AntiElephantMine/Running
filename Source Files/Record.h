#ifndef RECORD
#define RECORD

#include <iostream>
#include <string>
#include <vector>

/* NOTES

  1) explicit operator bool() const { return (*this != Record());}

  Conversion to bool was removed. The null record's (default) units are defaulted to miles, but why shouldn't a record with
  default units as kilometers and everything else zero also test equal to the null record? Since this could be misleading
  the bool conversion was removed.

  2) All operations should attempt to prevent creating invalid records. A record should be considered invalid if:

  I. Units is not 'm' or 'k'.
  II. Seconds or Minutes are not < 60.
  III. Distance is < 0.
  IV. Distance is zero but minperunit is non-zero. This is because processing infinity is not simple.

*/

class Record{
    friend std::ostream& operator<<(std::ostream&, const Record&);
    friend std::istream& operator>>(std::istream&, Record&);
    friend bool operator==(const Record&, const Record&);
    friend bool operator!=(const Record&, const Record&);
    friend Record operator+(const Record&, const Record&);
    friend void sortby(std::vector<Record>&, const char&);

public:
    Record() = default;
    Record(char, double, unsigned, unsigned, unsigned);

    Record& operator+=(const Record&);

    std::ostream& plainResult(std::ostream&) const;
    std::ostream& tableDisplay(std::ostream&) const;
    std::ostream& raceTime(std::ostream&, double, const char&) const;
    Record& convertUnits(const char&);
private:
    bool validRecord() const; //this checks the record is valid

    double distance = 0, minperunit = 0;
    unsigned hours = 0, minutes = 0, seconds = 0;
    char units = 'm';
    std::string date, classify;
};

std::ostream& operator<<(std::ostream&, const Record&);
std::istream& operator>>(std::istream&, Record&);
bool operator==(const Record&, const Record&);
bool operator!=(const Record&, const Record&);
Record operator+(const Record&, const Record&);
void sortby(std::vector<Record>&, const char&);

#endif // RECORD
