#include <iostream>
#include "formula.hpp"
#include <utility>
#include <numeric>

using namespace logic;

#define VAR(x) logic::Formula x(#x)

size_t count_sat(Interpretation& I, std::vector<Formula>& clauses) {
	return std::accumulate(
		clauses.begin(), clauses.end(),
		0, [&](size_t num_sat, const Formula& clause) {
			return std::move(num_sat) + clause.eval(I);
		}
	);
}

Interpretation flip(Interpretation I, std::string p) {
	I[p] = not I[p];
	return I;
}

int main() {
	std::cout << std::boolalpha;
	std::cout << "Question 1:" << std::endl;
	VAR(p_0); VAR(p_1); VAR(p_2);

	std::vector variables = { "p_0", "p_1", "p_2" };

	std::vector clauses = {
		not p_0 or not p_1 or p_2,
		p_0 or p_2,
		not p_0 or p_1,
		p_1 or not p_2,
		not p_0 or not p_1 or not p_2,
	};

	Interpretation I({
		{ "p_0", true },
		{ "p_1", false },
		{ "p_2", false },
	});

	std::cout << count_sat(I, clauses) << " Initially satisfied." << std::endl;
	for (const auto & var : variables) {
		auto I_flipped = flip(I, var);
		std::cout << "flipping " << var << " satisfies: " << count_sat(I_flipped, clauses) << std::endl;
	}

	std::cout << "Flipping p_1" << std::endl;
	I = flip(I, "p_1");
	for (const auto & var : variables) {
		auto I_flipped = flip(I, var);
		std::cout << "flipping " << var << " satisfies: " << count_sat(I_flipped, clauses) << std::endl;
	}

	std::cout << "Flipping p_0" << std::endl;
	I = flip(I, "p_0");
	for (const auto & var : variables) {
		auto I_flipped = flip(I, var);
		std::cout << "flipping " << var << " satisfies: " << count_sat(I_flipped, clauses) << std::endl;
	}

	std::cout << "Flipping p_2" << std::endl;
	I = flip(I, "p_2");

	std::cout << "Found satisfying interpretation: " << I << std::endl;



	std::cout << "\nQuestion 2:" << std::endl;

	I["p_0"] = true;
	I["p_1"] = false;
	I["p_2"] = false;

	std::cout << "Unsatisfied:" << std::endl;
	for (const auto & clause : clauses)
		if (!clause.eval(I))
			std::cout << clause << std::endl;

	std::cout << "Flipping p_0" << std::endl;
	I = flip(I, "p_0");

	std::cout << "Unsatisfied:" << std::endl;
	for (const auto & clause : clauses)
		if (!clause.eval(I))
			std::cout << clause << std::endl;

	std::cout << "Flipping p_2" << std::endl;
	I = flip(I, "p_2");

	std::cout << "Unsatisfied:" << std::endl;
	for (const auto & clause : clauses)
		if (!clause.eval(I))
			std::cout << clause << std::endl;

	std::cout << "Flipping p_1" << std::endl;
	I = flip(I, "p_1");

	if (count_sat(I, clauses) == 5) {
		std::cout << "Satisfied" << std::endl;
	} else {
		std::cout << "Unsatisfied" << std::endl;
	}


	std::cout << "\nQuestion 3:" << std::endl;

	VAR(p); VAR(q);
	auto expr = not ((p or q).implies((p and q) or p or (not q)));

	std::cout << expr << std::endl;
	auto model = expr.satisfy_naive();
	if (model.has_value()) {
		std::cout << "Is satisfied by: " << model.value() << std::endl;
	} else {
		std::cout << "Is unsatisfiable." << std::endl;
	}


	std::cout << "\nQuestion 4:" << std::endl;
	auto form1 = p.implies(q);
	auto form2 = (not q).implies(not p);

	std::cout << (form1 == form2) << " == " << form1.semantically_equivalent_naive(form2) << std::endl;
}
