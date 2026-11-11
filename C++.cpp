#include <iomanip>
#include <iostream>
enum class Zone { Inner, Middle, Outer, Airport, COUNT };
constexpr long long FARE_PAISE[] = {1000, 2000, 3000, 6000};   // one place to revise
static_assert(sizeof(FARE_PAISE) / sizeof(FARE_PAISE[0]) == static_cast<int>(Zone::COUNT), "fare table must cover every zone");
constexpr int PEAK_PERCENT = 20, WEEKEND_DISCOUNT_PERCENT = 10, CONCESSION_PERCENT = 25;
constexpr long long MIN_BALANCE_PAISE = 5000;
bool isPeak(int h) { return (h >= 8 && h < 10) || (h >= 17 && h < 20); }
// Separate rule: weekend discount and peak surcharge never combine (weekend wins)
long long timeAdjust(long long base, bool peak, bool weekend)
{
 if (weekend) return base * (100 - WEEKEND_DISCOUNT_PERCENT) / 100;
 if (peak)    return base * (100 + PEAK_PERCENT) / 100;
 return base;
}
class Pass {                       // monthly pass: unlimited travel inside two chosen zones
 unsigned mask_ = 0;
public:
 Pass() = default;
 Pass(Zone a, Zone b) : mask_((1u << static_cast<int>(a)) | (1u << static_cast<int>(b))) {}
 bool covers(Zone from, Zone to) const {
  return (mask_ >> static_cast<int>(from) & 1u) && (mask_ >> static_cast<int>(to) & 1u);
 }
};
long long fare(Zone from, Zone to, int hour, bool weekend, bool concession, const Pass& pass)
{
 if (pass.covers(from, to)) return 0;
 Zone far = static_cast<int>(from) > static_cast<int>(to) ? from : to;  // farthest zone
 long long f = timeAdjust(FARE_PAISE[static_cast<int>(far)], isPeak(hour), weekend);
 if (concession) f = f * (100 - CONCESSION_PERCENT) / 100;
 return f;
}
class SmartCard {
 long long bal_;
public:
 explicit SmartCard(long long b) : bal_(b >= 0 ? b : 0) {}
 long long balance() const { return bal_; }
 bool canTravel(long long f) const { return f >= 0 && bal_ - f >= MIN_BALANCE_PAISE; }
 bool deduct(long long f) { if (!canTravel(f)) return false; bal_ -= f; return true; }
};
int main()
{
 Pass none, pass(Zone::Inner, Zone::Middle);
 struct T { const char* n; Zone a, b; int h; bool we, co; const Pass* p; long long exp; };
 T t[] = {
  {"Airport off-peak weekday",   Zone::Inner,  Zone::Airport, 12, false, false, &none, 6000},
  {"Airport peak weekday",       Zone::Inner,  Zone::Airport, 9,  false, false, &none, 7200},
  {"Airport weekend",            Zone::Inner,  Zone::Airport, 12, true,  false, &none, 5400},
  {"Outer weekend at peak hour", Zone::Inner,  Zone::Outer,   8,  true,  false, &none, 2700},
  {"Middle weekend student",     Zone::Inner,  Zone::Middle,  12, true,  true,  &none, 1350},
  {"Pass: Inner->Middle",        Zone::Inner,  Zone::Middle,  9,  false, false, &pass, 0},
  {"Pass: Middle->Outer (not covered)", Zone::Middle, Zone::Outer, 12, false, false, &pass, 3000},
 };
 int ok = 0;
 for (const T& x : t) {
  long long f = fare(x.a, x.b, x.h, x.we, x.co, *x.p);
  bool good = f == x.exp; ok += good;
  std::cout << std::left << std::setw(36) << x.n << std::right << " expected " << std::setw(5) << x.exp
            << " actual " << std::setw(5) << f << (good ? "  PASS" : "  FAIL") << '\n';
 }
 SmartCard c(6000);
 std::cout << "Airport peak on Rs 60 card allowed? " << std::boolalpha << c.canTravel(7200) << '\n';
 std::cout << ok << " / 7 passed\n";
}
