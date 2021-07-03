#include <iostream>
#include "formula.hpp"

#define BOOL2STR(x) ((x) ? "true" : "false")

using namespace logic;

int main() {
	std::cout << std::endl
		        << "Question 3:" << std::endl;

	auto P = Formula::PropVar("P");
	auto Q = Formula::PropVar("Q");
	auto q1 = (P >> (Q and not P)) or (P == (not Q or P));

	std::cout << q1 << std::endl;
	std::cout << q1.tabulate() << std::endl;


	std::cout << std::endl
		        << "Question 4:" << std::endl;
	auto target = P >> Q;

	std::vector tests = {
		(not P).implies(not Q),
		(not P) or Q,
		Q.implies(P),
		not Q or P,
		(not Q).implies(not P)
	};

	for (const auto & test : tests) {
		if (test.semantically_equivalent_naive(target)) {
			std::cout << test << " is semantically equivalent to " << target << std::endl;
		}
	}


	std::cout << std::endl
		        << "Question 5:" << std::endl;
	auto S = Formula::PropVar("S");
	auto U = Formula::PropVar("U");
	auto W = Formula::PropVar("W");

	auto formula_5 = (not P) or (not Q) or S or (not U) or W;
	std::cout << formula_5 << std::endl;
	std::cout << formula_5.count_satisfying() << " interpretations satisfy the formula." << std::endl;


	std::cout << std::endl
		        << "Question 6:" << std::endl;

	Formula p_1("p_1"), p_2("p_2"), p_3("p_3"), p_4("p_4");

	auto test_1 = !p_1;
	auto test_2 = !(!test_1 ^ p_2);
	auto test_3 = !(!test_2 ^ p_3);
	auto test_4 = !(!test_3 ^ p_4);

	std::cout << "Test 1: " << test_1 << std::endl;
	std::cout << test_1.tabulate();
	std::cout << "Parity Check: " << BOOL2STR(test_1.is_parity_check()) << std::endl;

	std::cout << "\nTest 2: " << test_2 << std::endl;
	std::cout << test_2.tabulate();
	std::cout << "Parity Check: " << BOOL2STR(test_2.is_parity_check()) << std::endl;

	std::cout << "\nTest 3: " << test_3 << std::endl;
	std::cout << test_3.tabulate();
	std::cout << "Parity Check: " << BOOL2STR(test_3.is_parity_check()) << std::endl;

	std::cout << "\nTest 4: " << test_4 << std::endl;
	std::cout << test_4.tabulate();
	std::cout << "Parity Check: " << BOOL2STR(test_4.is_parity_check()) << std::endl;

	return 0;
}
