# logicpp

This is a logic framework for constructing and interpreting symbolic logic expressions in C++.
It aims to produce expressions which are visually similar to those in written logic.

Below is an example of tabulating an expression.
```cpp
auto P = Formula::PropVar("P");
auto Q = Formula::PropVar("Q");
auto q1 = (P >> (Q and not P)) or (P == (not Q or P));

std::cout << q1 << std::endl;
std::cout << q1.tabulate() << std::endl;
```

It is also possible to check for semantic equivalence:
```cpp
auto P = Formula::PropVar("P");
auto Q = Formula::PropVar("Q");
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
```

There is much more functionality supported as well, all of which has examples in `worksheets/`.
