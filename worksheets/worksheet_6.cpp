#include <iostream>
#include "formula.hpp"
#include <utility>
#include <numeric>

using namespace logic;

#define VAR(x) logic::Formula x(#x)

int main() {
	std::cout << std::boolalpha;
	std::cout << "Question 1:" << std::endl;
	VAR(p); VAR(q); VAR(r);
	auto T = Formula::Tautology();
	auto F = Formula::Contradiction();

	auto orig = (not q and r).implies(p) and (r or not p);

	std::cout << orig << std::endl;
	std::cout << orig.tabulate() << std::endl;
}
