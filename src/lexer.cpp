#include "lexer.h"
#include <iostream>

namespace babycpp {
namespace lexer {

int inline handleNoMatchFromRegex(const char *start) {
  if ((*start == EOF) || (*start == 0)) {
    return tok_eof;
  }
  // log ERROR
  return tok_no_match;
}

int inline isBuiltInKeyword(const std::string &str) {
  const auto iter = KEYWORDS.find(str);
  if (iter != KEYWORDS.end()) {
    return iter->second;
  }
  return tok_no_match;
}

inline std::string extractStringFromMatch(const Charmatch &matcher,
                                          int *offset) {
  uint32_t matcherSize = matcher.size();
  for (uint32_t i = 1; i < matcherSize; ++i) {
    if (matcher[i].matched) {
      (*offset) = matcher.length();
      return matcher[i];
    }
  }
  return "";
}

int processNumber(std::string &str, Lexer &L) {
  const char *ptr = str.c_str();
  uint32_t cLen = str.length();
  bool dotFound = false;
  for (uint32_t i = 0; i < cLen; ++i) {
    const char c = (*(ptr + i));
    if (isdigit(c)) {
      continue;
    }
    if (c == '.' && !dotFound) {
      dotFound = true;
      continue;
    }
    return tok_malformed_number;
  }

  // if we get to this point it is a valid number!
  if (dotFound) {
    // it means is a floating point
    L.value.floatNumber = std::stof(str);
    L.value.type = NumberType::FLOAT;
  } else {
    L.value.integerNumber = std::stoi(str);
    L.value.type = NumberType::INTEGER;
  }
  return tok_number;
}

int Lexer::gettok() {
  // making sure the lexer is initialized
  if (start == nullptr) {
    return tok_empty_lexer;
  }

  bool gotMatch = std::regex_search(start, matcher, expr,
                                    std::regex_constants::match_continuous);

  // in the offset variable we are going to store how many char will be
  // eaten by the token
  int offset;
  std::string extractedString = extractStringFromMatch(matcher, &offset);
  // handling case of not match
  if (!gotMatch) {
    return handleNoMatchFromRegex(start);
  }

  // handling builtin word
  int tok = isBuiltInKeyword(extractedString);
  if (tok != tok_no_match) {
    start += offset; // eating the token;
	identifierStr = extractedString;
    return tok;
  }

  // if is not a built in word it must be an identifier or an ascii value
  if (isdigit(extractedString[0])) {
    // procerssing number since variables are not allowed to start with a number
    start += offset; // eating the token;
    return processNumber(extractedString, *this);
  } else if (isalpha(extractedString[0])) {
    start += offset; // eating the token;
    identifierStr = extractedString;
    return tok_identifier;
  }

  // for (uint32_t i = 1; i < m.size(); ++i) {
  //  if (m[i].matched) {
  //    std::cout << m[i] << std::endl;
  //    running += m.length();
  //    break;
  //  }
  //}
  return -1;
}

/*
//old int gettok(Database &D) {
  // now we skipp all the white spaces
  while (isspace(D.lastChar) != 0) {
    D.lastChar = D.getchar();
  }

   if (isalpha(D.lastChar) != 0) {
    D.identifierStr = static_cast<char>(D.lastChar);
    // reading next char
    D.lastChar = D.getchar();
    // supporting alphanumeric tokens and '_'
    while ((isalnum(D.lastChar) != 0) || (D.lastChar == '_')) {
      D.identifierStr += static_cast<char>(D.lastChar);
      // pulling in next char
      D.lastChar = D.getchar();
    }

    //if (D.identifierStr == "def") {
    //  return tok_def;
    //}
    if (D.identifierStr == "extern") {
      return tok_extern;
    }
    return tok_identifier;
  }
  return -1;
  // if ((isdigit(D.lastChar) != 0) || D.lastChar == '.') {
  //  std::string numStr;
  //  do {
  //    numStr += static_cast<char>(D.lastChar);
  //    D.lastChar = D.getchar();
  //  } while ((isdigit(D.lastChar) != 0) || D.lastChar == '.');

  //  D.numVal = strtod(numStr.c_str(), nullptr);
  //  return tok_number;
  //}

  // if (D.lastChar == '#') {
  //  // we eat until the end of the line
  //  do {
  //    D.lastChar = D.getchar();
  //  } while (D.lastChar != EOF && D.lastChar != '\n' && D.lastChar != '\r');

  //  if (D.lastChar != EOF) {
  //    return gettok(D);
  //  }
  //}

  //// only remaining things are operators
  // if (D.lastChar == EOF) {
  //  return tok_eof;
  //}

  //// return as asci value
  // int thisChar = D.lastChar;
  // D.lastChar = D.getchar();
  // return thisChar;
}
*/

} // namespace lexer
} // namespace babycpp
