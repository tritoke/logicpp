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

	auto expr = q == T;

	std::cout << expr << std::endl;
	std::cout << expr.tabulate() << std::endl;

	std::cout << "Question 2:" << std::endl;
	VAR(s);
	
	std::vector clauses = {
		q or not p or s,
		not q or not p or not r or not s,
		not q or not p or r or not s,
	};

	expr = clauses[0] and clauses[1] and clauses[2];

	std::cout << expr << std::endl;
	std::cout << expr.tabulate() << std::endl;

	Interpretation I;

	I["p"] = 0;
	I["q"] = 0;
	I["r"] = 0;
	I["s"] = 0;


	std::cout << "Question 3:" << std::endl;
	expr = (q or s).implies(not p);

	std::cout << expr << std::endl;
	std::cout << expr.tabulate() << std::endl;
}
