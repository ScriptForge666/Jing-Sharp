#include "version.h"
#include <chrono>
#include <string>
#include <sstream>

std::string getVersion() {
    return std::string(ProjectName) + " version " + std::string(ProjectVersion);
}

int getYear() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto dp = floor<days>(now);
    year_month_day ymd{dp};
    return int(ymd.year());
}

std::string getCopyright() {
    std::ostringstream oss;
    oss << "Copyright " << getYearInterval() << " " << ProjectContributor
        << "\nLicensed under the " << ProjectLicense;
    return oss.str();
}
std::string getYearInterval() {
    int currentYear = getYear();
    int startYear = std::stoi(std::string(ProjectStartYear));
    if (currentYear == startYear) {
        return std::string(ProjectStartYear);
    } else {
        return std::string(ProjectStartYear) + "~" + std::to_string(currentYear);
    }
}