#include <cctype>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

///
/// Returns true is the string represents one of the supported operators
///
bool isOperator(std::string c) {
  return c == "*" || c == "/" || c == "+" || c == "-";
}

///
/// Computes the value as per the operators and returns it
///
double compute(double lhs, double rhs, std::string opr) {
  if (opr == "*")
    return lhs * rhs;
  if (opr == "/")
    return lhs / rhs;
  if (opr == "+")
    return lhs + rhs;
  if (opr == "-")
    return lhs - rhs;

  std::cout << "Warning: Unknown oprerator: " << opr << "\n";
  return 0;
}

///
/// Expects an vector of strings representing a expression in postfix notation.
/// The function evaluate the expression and return the value
/// The only error handling here is for programmer error but not input
/// validation is done here
///
double evalPostfixExpr(std::vector<std::string> &postfix) {
  std::stack<double> numbers;

  for (auto token : postfix) {
    if (isOperator(token)) {
      // Pop top two elements and compute and push it back to the stack
      auto rhs = numbers.top();
      numbers.pop();

      auto lhs = numbers.top();
      numbers.pop();

      numbers.push(compute(lhs, rhs, token));

    } else {
      numbers.push(std::stod(token.c_str()));
    }
  }

  if (numbers.size() != 1) {
    std::cout << "Warning: Not all values are processed; Incorrect result\n";
    return 0;
  }

  return numbers.top();
}

///
/// If the string represents an operator, returns the appropriate precedence
/// otherwise return lowest precedence 0;
///
unsigned getPrecedence(std::string s) {
  if (s == "*" || s == "/")
    return 3;

  if (s == "+" || s == "-")
    return 2;

  return 0;
}

///
/// Coverts and returns and postfix notation vector for a infix string.
/// The vector is used to keep the tokens separate.
/// All input string validation is done here, like incorrect placement of
/// operator, unknown symbols in the input, etc.
///
std::vector<std::string> getPostfixNotation(std::string &infix) {
  std::vector<std::string> postfix;
  std::stack<std::string> oprs;

  std::string digits;
  for (auto c : infix) {
    // ignore spaces
    if (std::isspace(c) > 0)
      continue;

    // collect digits unless we see a non-digit (possibly an operator)
    if (std::isdigit(c) > 0) {
      digits += c;
      continue;
    }

    std::string opr{c};
    if (!isOperator(opr)) {
      std::cout << "Error: Unexpected token: " << c << "\n";
      return {};
    }

    // We have seen an operator and it's should always be followed by a number,
    // unless it's + or - to indicate the positive or negative numbers
    if (digits.empty()) {
      if (opr == "-" || opr == "+") {
        digits = opr + digits;
        continue;
      }

      std::cout << "Ill-formed expression: Operator " << opr
                << " should be preceded by a number\n";
      return {};
    }

    // we have seen an operator, add last scanned digits to postfix vector
    postfix.push_back(digits);
    digits.clear();

    // Nothing in the stack so don't worry about precedence
    if (oprs.empty()) {
      oprs.push(opr);
      continue;
    }

    auto curr_prec = getPrecedence(opr);
    auto top_elem_prec = getPrecedence(oprs.top());

    //  move operator from stack to postfix notation unless the precedence of
    //  the top level operator is lesser the current operator
    while (top_elem_prec >= curr_prec && !oprs.empty()) {
      auto top_opr = oprs.top();
      postfix.push_back(top_opr);
      oprs.pop();

      if (oprs.empty())
        break;

      top_elem_prec = getPrecedence(oprs.top());
    }

    // Push the current operator to the top
    oprs.push(opr);
  }

  // The last scanned digit must be left out to add to postfix string. If that
  // is not the case the our expression ends with an operator, which is not
  // acceptable.
  if (!digits.empty())
    postfix.push_back(digits);

  else {
    std::cout << "Ill-formed expression at the end\n";
    return {};
  }

  // If the operator are left in the stack they are in right precedence order.
  // Add them to postfix string
  while (!oprs.empty()) {
    postfix.push_back(oprs.top());
    oprs.pop();
  }

  return postfix;
}

///
/// Performs the evaluation and prints the value
///
void eval(std::string &expr) {
  std::cout << "> Expr: '" << expr << "'\n";

  if (expr.empty()) {
    std::cout << "Empty expr\n\n";
    return;
  }

  std::vector<std::string> postfix = getPostfixNotation(expr);

  if (postfix.empty()) {
    std::cout << "Not processing expr due to unexpected token\n\n";
    return;
  }

  auto value = evalPostfixExpr(postfix);
  std::cout << "Eval: " << value << "\n\n";
}

int main() {
  std::vector<std::string> inputs = {"1 * 2 + 3 / 4 * 5 - 6",
                                     "1 + 24 * 3",
                                     "1 * 24 - 3",
                                     "4 / 2 * 5",
                                     "4 /2",
                                     "1 *2 /2 *2 /2 +6 ",
                                     "100 * 999 * 10000",
                                     "-1 * -2",
                                     "1 * -2",
                                     "2 + a",
                                     "",
                                     "23",
                                     "2 % 4",
                                     "2 ** 4 ",
                                     "8 + 2 * ",
                                     "*"};

  for (auto &expr : inputs)
    eval(expr);

  return 0;
}
