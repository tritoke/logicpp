#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace logic {
	/* anonymous namespace to hide Atom struct */
	namespace {
		/* atom struct - an atomic formulae
		 * this is either Top / Bottom or a variable */
		struct Atom {
			/* if the atom is a variable it has a name */
			std::string name;
			/* indicates what type of atom this is */
			enum AtomType {
				variable,
				tautology,
				contradiction,
			} type;

			/* atomic variable constructor */
			Atom(std::string _name, AtomType _type) : name(_name), type(_type) { }

			std::string to_string() const {
				switch (type) {
					case variable:
						return name;
					case tautology:
						return "⊤ ";
					case contradiction:
						return "⊥ ";
				}
			}
		};

		enum Connective {
			negation,
			conjunction,
			disjunction,
			implication,
			biimplication,
		};
	}

	/* expose AtomType from Atom */
	using AtomType = Atom::AtomType;

	/* a wrapper around std::unordered map */
	class Interpretation {
		/* mapping from atom.name to its valuation */
		std::unordered_map<std::string, bool> mapping;

	public:
		/* default constructor */
		Interpretation() = default;

		Interpretation(std::vector<std::pair<std::string, bool>>);

		/* operator[] overloads */
		bool& operator[](const std::string&);
		bool& operator[](const Atom&);

		/* .at overloads */
		bool at(const std::string&) const;
		bool at(const Atom&) const;

		friend std::ostream& operator<<(std::ostream&, const Interpretation&);

		std::size_t count_satisfied() const;
	};

	/*
	 * formula rules:
	 *
	 * 0. All atomic formulas are formulas
	 * 1. Tautology / Contradict - true / false are formulas
	 * 2. Conjunction:
	 *      A1 && ... && AN is a formula if A1 .. AN are formulas
	 * 3. Disjunction:
	 *      A1 || ... || AN is a formula if A1 .. AN are formulas
	 * 4. Negation:
	 *      !A is formula if A is a formula
	 * 5. Implication:
	 *      A->B is a formula if A and B are formulas
	 * 6. Equivalence:
	 *      A==B is a formula if A and B are formulas
	 * 
	 */
	class Formula {
		/* 
		 * if the formula is atomic this holds the atom
		 * lsf, connective and lsf should be nullptr,
		 * and connective should be empty
		 */
		std::optional<Atom> atom;

		/*
		 * Non-atomic formulas use these.
		 * atom should be empty if these hold a value.
		 *
		 * if connective is negation, only rsf holds a value
		 */
		std::shared_ptr<const Formula> lsf;
		std::optional<Connective> connective;
		std::shared_ptr<const Formula> rsf;

		/* a set of all variables used - constructed as the tree gets built */
		std::vector<std::string> variables;

		Formula(const Formula&, Connective, const Formula&); /* normal left side connective right side constructor */
		Formula(Connective, const Formula&); /* constructor for negation */
		Formula(Atom); /* atom constructor */

	public:
		/* atomic variable constructor */
		Formula(const char*);

		/* propositional variable constructors */
		static Formula PropVar(const char*);
		static Formula Tautology();
		static Formula Contradiction();

		/* Parse a formula from a string expression */
		/* static Formula Parse(std::string&); */

		/* operator backing functions */
		Formula negation() const;
		Formula conjunction(const Formula&) const;
		Formula disjunction(const Formula& rsf) const;
		Formula implication(const Formula& rsf) const;
		Formula biimplication(const Formula& rsf) const;

		/* convenient aliases */
		Formula implies(const Formula& rsf) const;
		Formula equals(const Formula& rsf) const;

		/* Formula construction operator overloads */
		friend Formula operator!(const Formula&);
		friend Formula operator&&(const Formula&, const Formula&);
		friend Formula operator||(const Formula&, const Formula&);
		friend Formula operator>>(const Formula&, const Formula&);
		friend Formula operator==(const Formula&, const Formula&);

		/* composite construction operators */
		friend Formula operator^(const Formula&, const Formula&);
		friend Formula operator!=(const Formula&, const Formula&);

		/* other functions returning a new formula */
		Formula replace(const std::string& varname, const Formula& replacement) const;

		/* stream output operator */
		friend std::ostream& operator<<(std::ostream&, const Formula&);

		/* ASCII string output */
		std::string to_ascii_string() const;

		/* evaluate the formula under a given interpretaton */
		bool eval(const Interpretation&) const;

		/* product a truth table for the formula */
		std::string tabulate() const;

		/* use a naive method to attempt to produce a satisfying interpretation */
		std::optional<Interpretation> satisfy_naive() const;
		
		/* functions constructed using satisfy_naive */
		bool satisfiable_naive() const;
		bool unsatisfiable_naive() const;
		bool is_tautology_naive() const;
		bool semantically_equivalent_naive(const Formula& formula) const;

		/* counts the number of satisfying interpretations */
		std::size_t count_satisfying() const;

		/* evaluates whether the formula is a parity check formula */
		bool is_parity_check() const;
	};
}
