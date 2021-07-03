#include <iostream>
#include "formula.hpp"

using namespace logic;

#define BOOL2STR(x) ((x) ? "true" : "false")
#define VAR(x) logic::Formula x(#x)

int main() {
	std::cout << "Question 3:" << std::endl;
	VAR(p); VAR(q); VAR(r);

	auto rhs = (not q and not p) or r;
	auto original = (p == not q).implies(r);
	std::vector steps = {
		(p == not q).implies(r),
		((not p or not q) and (not not q or p)).implies(r),
		not ((not p or not q) and (not not q or p)) or r,
		not (not p or not q) or not (not not q or p) or r,
		not not p and not not q or not not not q and not p or r,
		p and q or not q and not p or r,
		(p or (not q and not p) or r) and (q or (not q and not p) or r),
		(not p or p or r) and (not q or p or r) and (not p or q or r) and (not q or q or r)
	};

	for (const auto & step : steps) {
		std::cout << step.to_ascii_string() << std::endl;

		if (not original.semantically_equivalent_naive(step)) {
			std::cout << "OI WHAT" << std::endl;
			std::cout << original.tabulate() << std::endl;
			std::cout << step.tabulate() << std::endl;
			break;
		}
	}
}
