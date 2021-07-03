#include <iostream>
#include "formula.hpp"
#include <utility>
#include <numeric>

using namespace logic;

#define VAR(x) logic::Formula x(#x)

int main() {
	std::cout << std::boolalpha;
	std::cout << "Question 1:" << std::endl;
	VAR(p);

	std::vector variables = { "p" };

	auto Contra = Formula::Contradiction();
	auto F = p.implies(p);
	auto G = p;

	auto expr = F.implies(G);

	std::cout << expr << std::endl;
	std::cout << expr.tabulate() << std::endl;
}
