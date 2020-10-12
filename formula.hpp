#include <algorithm>
#include <bitset>
#include <memory>
#include <optional>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace logic {
	/* anonymouse namespace to hide Atom struct */
	namespace {
		/* atom struct - an atomic formulae
		 * this is either Top / Bottom or a variable */
		struct Atom {
			/* if the atom is a variable it has a name */
			std::string name;
			/* one-hot encoding of the atom type */
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
			equivalence
		};
	}

	/* expose AtomType from Atom */
	using AtomType = Atom::AtomType;

	/* a wrapper around std::unordered map */
	class Interpretation {
		/* mapping from atom.name to interpretation */
		std::unordered_map<std::string, bool> mapping;

	public:
		/* default constructor */
		Interpretation() = default;

		Interpretation(std::vector<std::pair<std::string, bool>> valuation) {
			/* initialize the interpretation from a list of pairs - name to valuation */
			std::copy(
				valuation.begin(), valuation.end(),
				std::inserter(mapping, mapping.end())
			);
		}

		uint64_t count_satisfied() const {
			uint64_t count = 0;
			for (const auto & [name, value]: mapping) {
				if (value) count++;
			}

			return count;
		}

		/* operator[] overloads */
		bool& operator[](std::string& name) {
			return mapping[name];
		}

		bool& operator[](const std::string& name) {
			return mapping[name];
		}

		bool& operator[](Atom& atom) {
			if (atom.type != AtomType::variable)
					throw std::runtime_error("Mutable references can only be taken for propositional variables.");

			return mapping[atom.name];
		}

		bool& operator[](const Atom& atom) {
			if (atom.type != AtomType::variable)
					throw std::runtime_error("Mutable references can only be taken for propositional variables.");

			return mapping[atom.name];
		}

		/* .at overloads */
		bool at(std::string& name) const {
			return mapping.at(name);
		}

		bool at(const std::string& name) const {
			return mapping.at(name);
		}

		bool at(Atom& atom) const {
			switch (atom.type) {
				case AtomType::variable:
					return mapping.at(atom.name);
				case AtomType::tautology:
					return true;
				case AtomType::contradiction:
					return false;
			}
		}

		bool at(const Atom& atom) const {
			switch (atom.type) {
				case AtomType::variable:
					return mapping.at(atom.name);
				case AtomType::tautology:
					return true;
				case AtomType::contradiction:
					return false;
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const Interpretation& interp) {
			os << '[';
			bool first_elem = true;
			for (const auto& [name, value]: interp.mapping) {
				if (!first_elem) {
					os << ", ";
				} else {
					first_elem = false;
				}

				os << name << " ↦ " << value;
			}
			os << ']';
			return os;
		}
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

		Formula(const Formula& _lsf, Connective _connective, const Formula& _rsf)
			: lsf(new Formula(_lsf)), connective(_connective), rsf(new Formula(_rsf)) { }

		/* constructor for negation */
		Formula(Connective _connective, const Formula& _rsf)
			: connective(_connective), rsf(new Formula(_rsf)) { }

		/* atom constructor */
		Formula(Atom _atom) : atom(_atom) { }

	public:
		/* atomic variable constructors */
		Formula(const char * name) : atom(Atom(name, AtomType::variable)) { }

		static Formula PropVar(const char * name) {
			return Formula(Atom(name, AtomType::variable));
		}

		static Formula Tautology() {
			return Formula(Atom("", AtomType::tautology));
		}

		static Formula Contradiction() {
			return Formula(Atom("", AtomType::contradiction));
		}

		/* negation */
		Formula negation() const {
			return Formula(Connective::negation, *this);
		}

		friend Formula operator!(const Formula& formula) {
			return formula.negation();
		}

		/* conjunction */
		Formula conjunction(const Formula& rsf) const {
			return Formula(*this, Connective::conjunction, rsf);
		}

		friend Formula operator&&(const Formula& lsf, const Formula& rsf) {
			return Formula(lsf, Connective::conjunction, rsf);
		}

		/* disjunction */
		Formula disjunction(const Formula& rsf) const {
			return Formula(*this, Connective::disjunction, rsf);
		}

		friend Formula operator||(const Formula& lsf, const Formula& rsf) {
			return lsf.disjunction(rsf);
		}

		/* exclusive disjunction */
		Formula exclusive_disjunction(const Formula& rsf) const {
			/* this is much simpler to define using operators*/
			return *this ^ rsf;
		}

		friend Formula operator^(const Formula& lsf, const Formula& rsf) {
			return (lsf and not rsf) or (rsf and not lsf);
		}

		/* implication */
		Formula implies(const Formula& rsf) const {
			return Formula(*this, Connective::implication, rsf);
		}

		friend Formula operator>>(const Formula& lsf, const Formula& rsf) {
			return lsf.implies(rsf);
		}

		/* equivalence */
		Formula equals(const Formula& rsf) const {
			return Formula(*this, Connective::equivalence, rsf);
		}

		friend Formula operator==(const Formula& lsf, const Formula& rsf) {
			return lsf.equals(rsf);
		}

		/* stream output operator */
		friend std::ostream& operator<<(std::ostream& os, const Formula& formula) {
			if (formula.atom.has_value()) {
				/* formula is an atom - output it as a string */
				os << formula.atom->to_string();
			} else {
				/* 
				 * formula is a sub-expression
				 * recursively construct it's representation
				 */

				switch (*formula.connective) {
					case Connective::negation:
						os << "¬" << *formula.rsf;
						break;
					case Connective::conjunction:
						os << "(" << *formula.lsf << "∧" << *formula.rsf << ")";
						break;
					case Connective::disjunction:
						os << "(" << *formula.lsf << "∨" << *formula.rsf << ")";
						break;
					case Connective::implication:
						os << "(" << *formula.lsf << "→" << *formula.rsf << ")";
						break;
					case Connective::equivalence:
						os << "(" << *formula.lsf << "↔" << *formula.rsf << ")";
						break;
				}
			}
			return os;
		}

		bool eval(const Interpretation& I) const {
			if (atom.has_value()) {
				/* atomic variable */
				return I.at(*atom);
			} else {
				/* perform connective logic operations */
				switch (*connective) {
					case Connective::negation:
						return not rsf->eval(I);
					case Connective::conjunction:
						return lsf->eval(I) and rsf->eval(I);
					case Connective::disjunction:
						return lsf->eval(I) or rsf->eval(I);
					case Connective::implication:
						return (not lsf->eval(I)) or rsf->eval(I);
					case Connective::equivalence:
						return lsf->eval(I) == rsf->eval(I);
				}
			}
		}

	private:
		void traverse_variables(std::unordered_set<std::string>& names) const {
			if (atom.has_value()) {
				/* atomic variable */
				if (atom->type == AtomType::variable)
					names.insert(atom->name);

			} else {
				/* perform connective logic operations */
				switch (*connective) {
					case Connective::negation:
						rsf->traverse_variables(names);
						break;
					default:
						rsf->traverse_variables(names);
						lsf->traverse_variables(names);
				}
			}
		}


	public:
		/* traverse the formula to get the variables used */
		std::unordered_set<std::string> variables() const {
			std::unordered_set<std::string> names;

			traverse_variables(names);

			return names;
		}

		std::string tabulate() const {
			const auto varnames = variables();
			std::vector<std::string> variables;

			if (varnames.size() > 64) {
				throw std::out_of_range("Formula contains too many variables to tabulate.");
			}

			/* copy the names out of the set */
			std::copy(
				varnames.begin(), varnames.end(),
				std::back_inserter(variables)
			);

			Interpretation I;
			std::stringstream repr;

			for (uint64_t i = 0; i < 1 << variables.size(); i++) {
				std::bitset<64> valuations(i);
				size_t index = 0;
				for (const auto & variable : variables) {
					I[variable] = valuations[index++];
				}

				repr << I << ": " << eval(I) << "\n";
			}

			return repr.str();
		}

		bool is_tautology() const {
			/* test whether the formula is a tautology  */
			const auto varnames = variables();
			std::vector<std::string> variables;

			if (varnames.size() > 64) {
				throw std::out_of_range("Formula contains too many variables to test like this.");
			}

			/* copy the names out of the set */
			std::copy(
				varnames.begin(), varnames.end(),
				std::back_inserter(variables)
			);

			Interpretation I;
			for (uint64_t i = 0; i < 1 << variables.size(); i++) {
				std::bitset<64> valuations(i);
				size_t index = 0;
				for (const auto & variable : variables) {
					I[variable] = valuations[index++];
				}

				/* this interpretation results in a false statement therefore not a tautology */
				if (not eval(I)) return false;
			}

			return true;
		}

		bool semantically_equivalent(const Formula& formula) const {
			auto equivalence_formula = *this == formula;

			return equivalence_formula.is_tautology();
		}

		uint64_t count_satisfying() const {
			/* test whether the formula is a tautology  */
			const auto varnames = variables();
			std::vector<std::string> variables;
		  uint64_t count = 0;

			if (varnames.size() > 64) {
				throw std::out_of_range("Formula contains too many variables to test like this.");
			}

			/* copy the names out of the set */
			std::copy(
				varnames.begin(), varnames.end(),
				std::back_inserter(variables)
			);

			Interpretation I;
			for (uint64_t i = 0; i < 1 << variables.size(); i++) {
				std::bitset<64> valuations(i);
				size_t index = 0;
				for (const auto & variable : variables) {
					I[variable] = valuations[index++];
				}

				if (eval(I)) count++;
			}

			return count;
		}

		bool is_parity_check() {
			/* test whether the formula is a tautology  */
			const auto varnames = variables();
			std::vector<std::string> variables;

			if (varnames.size() > 64) {
				throw std::out_of_range("Formula contains too many variables to test like this.");
			}

			/* copy the names out of the set */
			std::copy(
				varnames.begin(), varnames.end(),
				std::back_inserter(variables)
			);

			Interpretation I;
			for (uint64_t i = 0; i < 1 << variables.size(); i++) {
				std::bitset<64> valuations(i);
				size_t index = 0;
				for (const auto & variable : variables) {
					I[variable] = valuations[index++];
				}

				if (eval(I) && I.count_satisfied() % 2 != 0) return false;
			}

			return true;
		}
	};
}

