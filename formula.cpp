#include "formula.hpp"
#include <iostream>
#include <algorithm>
#include <bitset>
#include <sstream>

#include <codecvt>
#include <string>
#include <locale>

namespace logic {
	Interpretation::Interpretation(std::vector<std::pair<std::string, bool>> valuation) {
		/* initialize the interpretation from a list of pairs - name to valuation */
		std::copy(
			valuation.begin(), valuation.end(),
			std::inserter(mapping, mapping.end())
		);
	}

	bool& Interpretation::operator[](const std::string& name) {
		return mapping[name];
	}

	bool& Interpretation::operator[](const Atom& atom) {
		if (atom.type != AtomType::variable)
				throw std::runtime_error("Mutable references can only be taken for propositional variables.");

		return mapping[atom.name];
	}

	/* .at overloads */
	bool Interpretation::at(const std::string& name) const {
		return mapping.at(name);
	}

	bool Interpretation::at(const Atom& atom) const {
		switch (atom.type) {
			case AtomType::variable:
				return mapping.at(atom.name);
			case AtomType::tautology:
				return true;
			case AtomType::contradiction:
				return false;
		}
	}

	std::ostream& operator<<(std::ostream& os, const Interpretation& interp) {
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

	std::size_t Interpretation::count_satisfied() const {
		std::size_t count = 0;
		for (const auto & [name, value]: mapping) {
			if (value) count++;
		}

		return count;
	}

	/* normal left side connective right side constructor */
	Formula::Formula(const Formula& _lsf, Connective _connective, const Formula& _rsf)
		: lsf(new Formula(_lsf)), connective(_connective), rsf(new Formula(_rsf))
	{
		std::set_union(
			_lsf.variables.begin(), _lsf.variables.end(),
			_rsf.variables.begin(), _rsf.variables.end(),
			std::back_inserter(this->variables)
		);
		/* merge varnames from left and right side
		 * by first copying the bigger set, then inserting the smaller one */
		/*if (_lsf.varnames.size() >= _rsf.varnames.size()) {
			varnames = _lsf.varnames;
			varnames.insert(_rsf.varnames.begin(), _rsf.varnames.end());
		} else {
			varnames = _rsf.varnames;
			varnames.insert(_lsf.varnames.begin(), _lsf.varnames.end());
		}*/
	}

	/* constructor for negation */
	Formula::Formula(Connective _connective, const Formula& _rsf)
		: connective(_connective), rsf(new Formula(_rsf)), variables(_rsf.variables) { }

	/* atom constructor */
	Formula::Formula(Atom _atom) : atom(_atom), variables({ _atom.name }) { }

	/* atomic variable constructor */
	Formula::Formula(const char * name) : atom({ name, AtomType::variable }), variables({ name }) { }

	/* propositional variable constructors */
	Formula Formula::PropVar(const char * name) { return Formula(Atom(name, AtomType::variable)); }
	Formula Formula::Tautology() { return Formula(Atom("", AtomType::tautology)); }
	Formula Formula::Contradiction() { return Formula(Atom("", AtomType::contradiction)); }

	/* Parse a formula from a string expression */
	/* static Formula Parse(std::string& expression);*/

	/* operator backing functions */
	Formula Formula::negation()                        const { return {        Connective::negation,      *this }; }
	Formula Formula::conjunction(const Formula& rsf)   const { return { *this, Connective::conjunction,   rsf   }; }
	Formula Formula::disjunction(const Formula& rsf)   const { return { *this, Connective::disjunction,   rsf   }; }
	Formula Formula::implication(const Formula& rsf)   const { return { *this, Connective::implication,   rsf   }; }
	Formula Formula::biimplication(const Formula& rsf) const { return { *this, Connective::biimplication, rsf   }; }

	/* convenient aliases */
	Formula Formula::implies(const Formula& rsf) const { return this->implication(rsf);   }
	Formula Formula::equals(const Formula& rsf)  const { return this->biimplication(rsf); }

	/* Formula construction operator overloads */
	Formula operator!(const Formula& formula)                  { return formula.negation(); }
	Formula operator&&(const Formula& lsf, const Formula& rsf) { return lsf.conjunction(rsf); }
	Formula operator||(const Formula& lsf, const Formula& rsf) { return lsf.disjunction(rsf); }
	Formula operator>>(const Formula& lsf, const Formula& rsf) { return lsf.implies(rsf); }
	Formula operator==(const Formula& lsf, const Formula& rsf) { return lsf.equals(rsf); }

	/* composite construction operators */
	Formula operator^(const Formula& lsf, const Formula& rsf)  { return (lsf and not rsf) or (rsf and not lsf); }
	Formula operator!=(const Formula& lsf, const Formula& rsf) { return not lsf == rsf; }

	/* other functions returning a new formula */
	Formula Formula::replace(const std::string& varname, const Formula& replacement) const {
		std::cout << "HECC: " << *this << std::endl;
		/* replace an atomic variable with a formula */

		/* we are an atom */
		if (this->atom.has_value()) {
			/* replace this atom */
			if (atom->name == varname) {
				return replacement;
			}
		} else {
			/* we are a formula */

			/* if we are anything but a negation then do the lsf */
			if (this->connective == Connective::negation) {
				return {
					Connective::negation,
					this->rsf->replace(varname, replacement)
				};
			} else {
				return { 
					this->lsf->replace(varname, replacement),
					Connective::negation,
					this->rsf->replace(varname, replacement)
				};
			}
		}

		/* return a copy of the current node */
		return *this;
	}

	/* stream output operator */
	std::ostream& operator<<(std::ostream& os, const Formula& formula) {
		if (formula.atom.has_value()) {
			/* formula is an atom - output it as a string */
			os << formula.atom->to_string();
		} else {
			/* 
			 * formula is a sub-expression
			 * recursively construct it's representation
			 */
			switch (formula.connective.value()) {
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
				case Connective::biimplication:
					os << "(" << *formula.lsf << "↔" << *formula.rsf << ")";
					break;
			}
		}
		return os;
	}

	/* ASCII string output */
	std::string Formula::to_ascii_string() const {
		std::stringstream unicode_repr, ascii_repr;
		std::wstringstream intermediate_wide_repr, output_wide_repr;

		/* use stringstream to get the unicode representation */
		unicode_repr << *this;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    intermediate_wide_repr << conv.from_bytes(unicode_repr.str());

		wchar_t c;
		while (intermediate_wide_repr >> c) {
			switch (c) {
				case L'¬':
					output_wide_repr << "~";
					break;
				case L'∧':
					output_wide_repr << "/\\";
					break;
				case L'∨':
					output_wide_repr << "\\/";
					break;
				case L'→':
					output_wide_repr << "->";
					break;
				case L'↔':
					output_wide_repr << "<->";
					break;
				case L'⊤':
					intermediate_wide_repr >> c;
					output_wide_repr << "T";
					break;
				case L'⊥':
					intermediate_wide_repr >> c;
					output_wide_repr << "F";
					break;
				default:
					output_wide_repr << c;
			}
		}

		return conv.to_bytes(output_wide_repr.str());
	}

	bool Formula::eval(const Interpretation& I) const {
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
				case Connective::biimplication:
					return lsf->eval(I) == rsf->eval(I);
			}
		}
	}

	std::string Formula::tabulate() const {
		if (variables.size() > 64) {
			throw std::out_of_range("Formula contains too many variables to tabulate.");
		}

		Interpretation I;
		std::stringstream repr;

		for (std::size_t i = 0; i < 1 << variables.size(); i++) {
			std::bitset<64> valuations(i);
			size_t index = 0;
			for (const auto & variable : variables) {
				I[variable] = valuations[index++];
			}

			repr << I << ": " << eval(I) << "\n";
		}

		return repr.str();
	}

	std::optional<Interpretation> Formula::satisfy_naive() const {
		/* naively iterates through all interpretations to find one which satisfies the formula */
		if (variables.size() > 64) {
			throw std::out_of_range("Formula contains too many variables to satisfy like this.");
		}

		Interpretation I;
		std::stringstream repr;

		for (std::size_t i = 0; i < 1 << variables.size(); i++) {
			std::bitset<64> valuations(i);
			size_t index = 0;
			for (const auto & variable : variables) {
				I[variable] = valuations[index++];
			}

			if (eval(I)) return I;
		}

		return std::nullopt;
	}

	bool Formula::satisfiable_naive() const {
		auto model = satisfy_naive();
		
		return model.has_value();
	}

	bool Formula::unsatisfiable_naive() const {
		return !satisfiable_naive();
	}

	bool Formula::is_tautology_naive() const {
		/* if its a tautology the its inverse is unsatisfiable */
		auto inverse = not *this;

		return inverse.unsatisfiable_naive();
	}

	bool Formula::semantically_equivalent_naive(const Formula& formula) const {
		/* if they are equivalent A != B is unsatisfiable */
		auto equivalence_formula = *this != formula;

		return equivalence_formula.unsatisfiable_naive();
	}

	std::size_t Formula::count_satisfying() const {
		if (variables.size() > 64) {
			throw std::out_of_range("Formula contains too many variables to test like this.");
		}

		std::size_t count = 0;

		Interpretation I;
		for (std::size_t i = 0; i < 1 << variables.size(); i++) {
			std::bitset<64> valuations(i);
			size_t index = 0;
			for (const auto & variable : variables) {
				I[variable] = valuations[index++];
			}

			if (eval(I)) count++;
		}

		return count;
	}

	bool Formula::is_parity_check() const {
		/* test whether the formula is a tautology  */
		if (variables.size() > 64) {
			throw std::out_of_range("Formula contains too many variables to test like this.");
		}

		Interpretation I;
		for (std::size_t i = 0; i < 1 << variables.size(); i++) {
			std::bitset<64> valuations(i);
			size_t index = 0;
			for (const auto & variable : variables) {
				I[variable] = valuations[index++];
			}

			if (eval(I) && I.count_satisfied() % 2 != 0) return false;
		}

		return true;
	}
}
