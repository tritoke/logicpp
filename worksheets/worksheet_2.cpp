#include <iostream>
#include <vector>
#include "formula.hpp"

#define BOOL2STR(x) ((x) ? "true" : "false")
#define VAR(x) logic::Formula x(#x)

using namespace logic;

int main() {
	VAR(P); VAR(Q); VAR(R);
	auto T = Formula::Tautology();
	auto F = Formula::Contradiction();

	std::vector steps = {
		// initial formula
		not ((P or R).implies(Q)).implies(P and (Q == R)),
		// P is pure negative atom
		not ((F or R).implies(Q)).implies(F and (Q == R)),
		not (R.implies(Q)).implies(F),
		not (not (R.implies(Q))),
		R.implies(Q),
		// Q and R are now pure
		F.implies(Q),
		T,
	};

	for (const auto & step : steps) {
		std::cout << step << std::endl;
	}
}
