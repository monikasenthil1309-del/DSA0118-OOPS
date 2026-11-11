#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
enum class Zone { Inner, Middle, Outer };
constexpr long long BASE_PAISE[] = {1000, 2000, 3000};
constexpr int PEAK_PERCENT = 20;
constexpr int CONCESSION_PERCENT = 25;   // assumption: students/seniors pay 75%
constexpr long long MIN_BALANCE_PAISE = 5000;
bool isPeak(int h) { return (h >= 8 && h < 10) || (h >= 17 && h < 20); }
long long fare(Zone z, bool peak, bool concession)
{
 long long f = 0;
 switch (z) {
 case Zone::Inner:  f = BASE_PAISE[0]; break;
 case Zone::Middle: f = BASE_PAISE[1]; break;
 case Zone::Outer:  f = BASE_PAISE[2]; break;
 default: return -1;
 }
 if (peak) f = f * (100 + PEAK_PERCENT) / 100;
 if (concession) f = f * (100 - CONCESSION_PERCENT) / 100;
 return f;
}
class SmartCard {
 long long balance_;
public:
 explicit SmartCard(long long b) : balance_(b >= 0 ? b : 0) {}
 long long balance() const { return balance_; }
 bool canTravel(long long f) const { return f >= 0 && balance_ - f >= MIN_BALANCE_PAISE; }
 bool deduct(long long f) { if (!canTravel(f)) return false; balance_ -= f; return true; }
 bool recharge(long long a) { if (a <= 0) return false; balance_ += a; return true; }
};
// returns fare paise, -1 invalid input, -2 low balance
long long journey(SmartCard& c, const std::string& zoneStr, const std::string& hourStr, bool conc)
{
 int zi, h;
 try { zi = std::stoi(zoneStr); h = std::stoi(hourStr); }
 catch (...) { return -1; }
 if (zi < 0 || zi > 2 || h < 0 || h > 23) return -1;
 long long f = fare(static_cast<Zone>(zi), isPeak(h), conc);
 return c.deduct(f) ? f : -2;
}
struct Test { const char* name; const char* zone; const char* hour; bool conc; long long startBal; long long expected; };
int main()
{
 std::vector<Test> tests = {
  {"Inner off-peak 12:00",      "0", "12", false, 10000, 1000},
  {"Middle off-peak 12:00",     "1", "12", false, 10000, 2000},
  {"Outer off-peak 12:00",      "2", "12", false, 10000, 3000},
  {"Inner peak 09:00",          "0", "9",  false, 10000, 1200},
  {"Middle peak 08:00 (start)", "1", "8",  false, 10000, 2400},
  {"Outer peak 17:00 (start)",  "2", "17", false, 10000, 3600},
  {"Outer 10:00 (end, off)",    "2", "10", false, 10000, 3000},
  {"Outer 20:00 (end, off)",    "2", "20", false, 10000, 3000},
  {"Outer 07:00 off-peak",      "2", "7",  false, 10000, 3000},
  {"Student Middle off-peak",   "1", "12", true,  10000, 1500},
  {"Senior Outer peak",         "2", "18", true,  10000, 2700},
  {"Leaves exactly Rs 50",      "0", "12", false, 6000,  1000},
  {"Leaves Rs 49.99 (denied)",  "0", "12", false, 5999,  -2},
  {"Unknown zone 5",            "5", "12", false, 10000, -1},
  {"Letters for hour",          "0", "ab", false, 10000, -1},
  {"Hour 25",                   "0", "25", false, 10000, -1},
 };
 int pass = 0;
 std::cout << std::left << std::setw(28) << "Case" << std::right << std::setw(10) << "Expected"
           << std::setw(10) << "Actual" << "  Result\n";
 for (const Test& t : tests) {
  SmartCard c(t.startBal);
  long long a = journey(c, t.zone, t.hour, t.conc);
  bool ok = (a == t.expected) && c.balance() >= 0;
