#ifndef GIT_CL_C_PRE_PROCESSOR_C_PRE_PROCESSOR_H
#define GIT_CL_C_PRE_PROCESSOR_C_PRE_PROCESSOR_H

#include <unordered_map>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <regex>

//macroFunctions
#define substrID(id1, id2) substr((id1),(id2)-(id1))
#define replaceID(id1, id2, arg) replace((id1),(id2)-(id1),arg)

namespace StringUtils {
   std::vector<std::string> split(const std::string &str, const char &delimiter) {
      std::vector<std::string> strings;
      std::string collect;

      for (auto &c: str) {
         if (c == delimiter) {
            strings.push_back(collect);
            collect = "";
         } else collect += c;

      }
      strings.push_back(collect);
      return strings;
   }

   bool findToken(const std::string &line, size_t start, size_t &findStart, size_t &findEnd, bool reverseIter, const std::function<bool(char)>& isToken) {
      bool retValue = false;
      if (reverseIter) {
         for (size_t lIID = std::min(start, line.length()) - 1; lIID > 0; lIID--) {
            const char &found = line[lIID];
            if (isToken(found)) {
               findEnd = lIID;
               findStart = 0;
               retValue = true;
               break;
            }
         }
         for (size_t lIID = std::min(findEnd, line.size()) - 1; lIID > 0; lIID--) {
            const char &found = line[lIID];
            if (!isToken(found)) {
               findStart = lIID;
               break;
            }
         }
         return retValue;
      } else {
         for (size_t lIID = start; lIID < line.size(); lIID++) {
            const char &found = line[lIID];
            if (isToken(found)) {
               findStart = lIID;
               findEnd = line.length();
               retValue = true;
               break;
            }
         }
         for (size_t lIID = findStart; lIID < line.size(); lIID++) {
            const char &found = line[lIID];
            if (!isToken(found)) {
               findEnd = lIID;
               break;
            }
         }
         return retValue;
      }
   }

   bool findMark(const std::string &line, size_t start, size_t &find, const char &search, bool reverseIter, const std::function<bool(char)>& wall) {
      if (reverseIter) {
         for (size_t lIID = std::min(start, line.length()) - 1; lIID > 0; lIID--) {
            const char &found = line[lIID];
            if (found == search) {
               find = lIID;
               return true;
            }
            if (wall(found)) return false;
         }
         return false;
      } else {
         for (size_t lIID = start; lIID < line.length(); lIID++) {
            const char &found = line[lIID];
            if (found == search) {
               find = lIID;
               return true;
            }
            if (wall(found)) return false;
         }
         return false;
      }
   }

   bool findToken(const std::string &line, size_t start, size_t &findStart, size_t &findEnd) {
      auto isToken = [](char found) {
         return (found >= 'A' && found <= 'z') || (found >= '0' && found <= '9');
      };
      return findToken(line, start, findStart, findEnd, false, isToken);
   }

   bool findMark(const std::string &line, size_t start, size_t &find, const char &search) {
      auto checkWall = [](char found) {
         return (found >= 'A' && found <= 'z') || (found >= '0' && found <= '9');
      };
      return findMark(line, start, find, search, false, checkWall);
   }

   bool findToken(const std::string &line, size_t start, size_t &findStart, size_t &findEnd, bool reverseIter) {
      auto isToken = [](char found) {
         return (found >= 'A' && found <= 'z') || (found >= '0' && found <= '9');
      };
      return findToken(line, start, findStart, findEnd, reverseIter, isToken);
   }

   bool findMark(const std::string &line, size_t start, size_t &find, const char &search, bool reverseIter) {
      auto checkWall = [](char found) {
         return (found >= 'A' && found <= 'z') || (found >= '0' && found <= '9');
      };
      return findMark(line, start, find, search, reverseIter, checkWall);
   }

   void removeDuplicates(std::string &input, char character, bool removeAll) {
      std::vector<std::string> segments = split(input, character);
      input = "";
      bool firstWord = true;
      for (std::string &seg: segments) {
         if (seg.empty()) continue;
         input += (firstWord ? "" : " ") + seg;
         if (!removeAll)firstWord = false;
      }
   }

   void removeTabs(std::string &input, const int &indentSizeInSpaces) {
      std::string result;
      std::string spaces(indentSizeInSpaces, ' ');

      for (char ch: input) {
         if (ch == '\t') {
            result += spaces;
         } else result += ch;
      }

      input = result;
   }

   std::string escapeRegex(const std::string& keyword) {
      static const std::regex specialChars{R"([-[\]{}()*+?.,\^$|#\s])"};
      return std::regex_replace(keyword, specialChars, R"(\$&)");
   }

   class KeyWordSearch {
   public:
      struct MatchDetail {
         std::string keyword;
         size_t position;
         size_t id;
      };

      struct SearchKeyword {
         std::string keyWord;
         bool caseSensitive = true;
         bool lineStart = false;
      };

      KeyWordSearch() = default;

      ~KeyWordSearch() = default;

      std::vector<MatchDetail> findKeywords(const std::string& text) const {
         std::vector<MatchDetail> matches;
         std::vector<SearchKeyword> tempKeyWords = keyWords;
         std::vector<size_t> sortedIDs = sortKeywordsByLength(tempKeyWords);
         std::regex pattern(createRegexPattern(tempKeyWords), std::regex::multiline);//2048

         auto wordsBegin = std::sregex_iterator(text.begin(), text.end(), pattern);
         auto wordsEnd = std::sregex_iterator();
         for (std::sregex_iterator it = wordsBegin; it != wordsEnd; ++it) {
            const std::smatch& match = *it;
            for (size_t j = 1; j < match.size(); ++j) {
               if (match[j].matched) {
                  matches.push_back({match.str(j), static_cast<size_t>(match.position(j)), sortedIDs[j-1]});
               }
            }
         }
         return matches;
      }

      std::vector<SearchKeyword> keyWords;

      void addKeyWord(const std::string& keyWord, const bool& caseSensitive, const bool& lineStart) {
         keyWords.push_back({keyWord,caseSensitive,lineStart});
      }

      void removeKeyWord(const std::string &keyWord) {
         keyWords.erase(
               std::remove_if(keyWords.begin(), keyWords.end(),
                              [&keyWord](const SearchKeyword& keyword) {
                                 return keyword.keyWord == keyWord;
                              }),
               keyWords.end()
         );
      }

   private:
      static std::vector<size_t> sortKeywordsByLength(std::vector<SearchKeyword>& keywords) {
         std::vector<std::pair<size_t , SearchKeyword>> pairs(keywords.size());
         for (int i = 0; i < keywords.size(); ++i) {
            pairs[i] = std::make_pair(i, keywords[i]);
         }

         std::sort(pairs.begin(), pairs.end(), [](const std::pair<size_t , SearchKeyword>& a, const std::pair<size_t , SearchKeyword>& b) {
            return a.second.keyWord.size() > b.second.keyWord.size();
         });

         for (int pID = 0; pID < keywords.size(); ++pID) {
            keywords[pID] = pairs[pID].second;
         }

         std::vector<size_t> sortedIDs(keywords.size());
         for (int pID = 0; pID < keywords.size(); ++pID) {
            sortedIDs[pID] = pairs[pID].first;
         }
         return sortedIDs;
      }

      static std::string createRegexPattern(const std::vector<SearchKeyword>& keywords) {
         std::string pattern;
         for (size_t i = 0; i < keywords.size(); ++i) {
            if (i != 0) {
               pattern += "|";
            }
            pattern += "(";
            if (keywords[i].lineStart) {
               pattern += "^[\\t| ]*";
            }

            if (keywords[i].caseSensitive) {
               pattern += escapeRegex(keywords[i].keyWord);
            } else {
               std::string caseless_pattern;
               for (char c : keywords[i].keyWord) {
                  caseless_pattern += "[" + std::string(1, (char)std::toupper(c)) + "|" + std::string(1, (char)std::tolower(c)) + "]";
               }
               pattern += caseless_pattern;
            }
            pattern += ")";
         }
         return pattern;
      }
   };

   template<typename T>
   class ExpressionParser {
   private:
      enum EXOP_En {
         OP_UNKNOWN = 'u',
         //Arithmetic Operators
         OPA_NEG = 'n',
         OPA_ADD = '+',
         OPA_SUB = '-',
         OPA_MUL = '*',
         OPA_DIV = '/',
         OPA_MOD = '%',
         //Comparison Operators
         OPC_EQUAL = '=',
         OPC_NOTEQ = 'e',
         OPC_LESST = '<',
         OPC_GREET = '>',
         OPC_ELEST = 'l',
         OPC_EGRET = 'g',
         //Logical Operators
         OPL_AND = 'a',
         OPL_OR = 'o',
         OPL_NOT = '!',
         //Bitwise Operators
         OPB_BWAND = '&',
         OPB_BWOR = '|',
         OPB_BWXOR = '^',
         OPB_BWNOT = '~',
         OPB_BWLSH = 'L',
         OPB_BWRSH = 'R'

      };
   public:
      enum EXTreeTokenType {
         EXTTT_OP = 0,
         EXTTT_NAMEID = 1,
         EXTTT_NUM = 2
      };

      struct ExOperator {
         EXOP_En op;
         int placeScore = 0;
         int layerScore = 0;

         ExOperator() : op(OP_UNKNOWN), placeScore(0), layerScore(0) {}
      };

      struct EXTreeNode {
         EXTreeTokenType type;
         union {
            struct {
               EXOP_En op;
               EXTreeNode *left;
               EXTreeNode *right;
            };
            size_t valueID;
            T num;
         };
      };

      struct EXTreeToken {
         EXTreeTokenType type;
         union {
            ExOperator exOperator;
            size_t valueID = 0;
            T num;
         };

         EXTreeToken() : type(EXTTT_OP) {
            new(&exOperator) ExOperator();
         }

         ~EXTreeToken() {
            exOperator.~ExOperator();
         }

      };

   public:
      ExpressionParser() = default;

      explicit ExpressionParser(const std::string &expression) {
         exprSTR = expression;
      }

      ~ExpressionParser() {
         exprTokens.clear();
         exprSTR = "";
         deleteTree();
      }

      void buildTree(std::string expression, const std::vector<std::string> &inputs) {
         exprTokens = tokenizeExpression(expression, inputs);
         rootNode = buildTree(exprTokens);
      }

      void buildTree(const std::vector<std::string> &inputs) {
         exprTokens = tokenizeExpression(exprSTR, inputs);
         rootNode = buildTree(exprTokens);
      }

      T evaluate(const std::vector<T> &values) {
         return evaluate(rootNode, values);
      }

      void deleteTree() {
         deleteTree(rootNode);
      }

      std::string exprSTR;
      EXTreeNode *rootNode = nullptr;
      std::vector<EXTreeToken> exprTokens;

   private:
      bool isDigit(char c) {
         return std::isdigit(static_cast<unsigned char>(c));
      }

      EXOP_En getEXOPEnum(const std::string &ch, int &id,const bool& prevWasOp) {
         char mainCh = ch[id];
         bool secUV = (id <= ch.size() - 1);
         switch (mainCh) {
            case '+':
               return OPA_ADD;
            case '-':
               if(prevWasOp) {
                  return OPA_NEG;
               } else return OPA_SUB;
            case '*':
               return OPA_MUL;
            case '/':
               return OPA_DIV;
            case '%':
               return OPA_MOD;
            case '^':
               return OPB_BWXOR;
            case '~':
               return OPB_BWNOT;
            case '=':
               id++;
               return OPC_EQUAL;
            case '!':
               if (secUV) return OPL_NOT;
               if (ch[id + 1] == '=') {
                  id++;
                  return OPC_NOTEQ;
               } else return OPL_NOT;
            case '<':
               if (secUV) return OPC_LESST;
               if (ch[id + 1] == '=') {
                  id++;
                  return OPC_ELEST;
               } else if (ch[id + 1] == '<') {
                  id++;
                  return OPB_BWLSH;
               } else return OPC_LESST;
            case '>':
               if (secUV) return OPC_GREET;
               if (ch[id + 1] == '=') {
                  id++;
                  return OPC_EGRET;
               } else if (ch[id + 1] == '>') {
                  id++;
                  return OPB_BWRSH;
               } else return OPC_GREET;
            case '&':
               if (secUV) return OPB_BWAND;
               if (ch[id + 1] == '&') {
                  id++;
                  return OPL_AND;
               } else return OPB_BWAND;
            case '|':
               if (secUV) return OPB_BWOR;
               if (ch[id + 1] == '|') {
                  id++;
                  return OPL_OR;
               } else return OPB_BWOR;

            default:
               return OP_UNKNOWN;
         }
      }

      std::vector<EXTreeToken> tokenizeExpression(const std::string &expression, const std::vector<std::string> &inputs) {
         std::vector<EXTreeToken> tokens;
         KeyWordSearch search;
         for (const auto & input : inputs) {
            search.addKeyWord(input, true, false);
         }
         std::vector<KeyWordSearch::MatchDetail> foundKeys = search.findKeywords(expression);

         int OPID = 0;
         int opDepth = 0;
         int nextKey = 0;

         bool prevWasOp = true;

         //numEx
         T snum = 0;
         int snumL = 0;
         int dot = 0;
         bool isNum = false;
         bool hasDot = false;

         for (int sID = 0; sID < expression.size(); sID++) {
            bool jumped = false;
            bool numSkipped = false;
            if (expression[sID] == ' ') {
               jumped = true;
               goto skipNumAdd;
            }
            if (expression[sID] == '(') {
               opDepth++;
               jumped = true;
               prevWasOp = true;
               goto skipNumAdd;
            }
            if (expression[sID] == ')') {
               opDepth--;
               jumped = true;
               prevWasOp = false;
               goto skipNumAdd;
            }
            {
               bool interPrew = foundKeys.size() > nextKey && sID == foundKeys[nextKey].position;
               if (interPrew) {
                  sID += static_cast<int>(foundKeys[nextKey].keyword.length() - 1);
                  EXTreeToken token;
                  token.valueID = foundKeys[nextKey++].id;
                  token.type = EXTTT_NAMEID;
                  tokens.push_back(token);
                  jumped = true;
                  numSkipped = true;
                  prevWasOp = false;
                  goto skipNumAdd;
               }
               EXOP_En expr = getEXOPEnum(expression, sID, prevWasOp);
               if (expr != OP_UNKNOWN) {
                  ExOperator exOperator;
                  exOperator.layerScore = opDepth;
                  exOperator.op = expr;
                  exOperator.placeScore = expression.length() - OPID++;

                  EXTreeToken token;
                  token.exOperator = exOperator;
                  token.type = EXTTT_OP;
                  tokens.push_back(token);
                  jumped = true;
                  numSkipped = true;
                  prevWasOp = true;
                  goto skipNumAdd;
               }
               if (isDigit(expression[sID]) || expression[sID] == '.') {
                  if (expression[sID] == '.') {
                     dot = snumL;
                     hasDot = true;
                  } else {
                     isNum = true;
                     snum *= 10;
                     snum += (expression[sID] - 48);
                     snumL++;
                  }
               }
            }

            skipNumAdd:
            bool numEnd = !(isDigit(expression[sID + 1]) || expression[sID + 1] == '.');
            bool exprEnd = sID >= expression.size() - 1;
            if (isNum && (numEnd || exprEnd || jumped)) {
               T num = snum;
               T div = powf(10, (float)snumL - (float)dot);
               if (hasDot)num /= div;
               snum = 0;
               snumL = 0;
               dot = 0;
               isNum = false;
               hasDot = false;


               EXTreeToken token;
               token.num = num;
               token.type = EXTTT_NUM;
               if (numSkipped) {
                  if (!tokens.empty()) tokens.insert(tokens.end() - 1, token);
               } else tokens.push_back(token);
               prevWasOp = false;
            }
         }
         return tokens;
      }

      EXTreeNode *buildTree(const std::vector<EXTreeToken> &tokens) {
         size_t sFID = 0;
         int lim = 999999;
         int mLS = lim;
         int mOS = lim;
         int mPS = lim;
         if (tokens.empty()) return nullptr;
         for (size_t tIID = 0; tIID < tokens.size(); tIID++) {
            EXTreeToken token = tokens[tIID];
            if (token.type == EXTTT_OP) {
               int tLS = token.exOperator.layerScore;
               int tOS = 12 - getPrecedenceScore(token.exOperator.op);
               int tPS = token.exOperator.placeScore;

               #define ANSMALL \
                  mLS = tLS; \
                  mOS = tOS; \
                  mPS = tPS; \
                  sFID = tIID;

               if (tLS <= mLS) {
                  if (tLS < mLS) {
                     ANSMALL
                  }
                  if (tOS <= mOS) {
                     if (tOS < mOS) {
                        ANSMALL
                     }
                     if (tPS <= mPS) {
                        ANSMALL
                     }
                  }
               }
            }
         }
         auto *node = new EXTreeNode();
         if (mLS == lim) {

            EXTreeToken token = tokens[0];
            if (token.type == EXTTT_NUM) {
               node->type = EXTTT_NUM;
               node->num = token.num;
            } else if (token.type == EXTTT_NAMEID) {
               node->type = EXTTT_NAMEID;
               node->valueID = token.valueID;
            } else return nullptr;
            return node;
         } else {
            std::vector<EXTreeToken> before;
            std::vector<EXTreeToken> after;
            before.assign(tokens.begin(), tokens.begin() + sFID);
            after.assign(tokens.begin() + sFID + 1, tokens.end());

            node->op = tokens[sFID].exOperator.op;
            node->type = EXTTT_OP;
            node->left = buildTree(before);
            node->right = buildTree(after);
            return node;
         }
      }

      void deleteTree(EXTreeNode *root) {
         if (!root) return;

         std::queue<EXTreeNode *> queue;

         if (root->type == EXTTT_OP) {
            queue.push(root->left);
            queue.push(root->right);
            root->right = nullptr;
            root->left = nullptr;
         }

         EXTreeNode *current;
         while (!queue.empty()) {
            current = queue.front();
            queue.pop();

            if (!current) continue;

            if (current->type == EXTTT_OP) {
               queue.push(current->left);
               queue.push(current->right);
               current->right = nullptr;
               current->left = nullptr;
            }

            delete current;
         }

         delete root;
         root = nullptr;
      }

      T evaluate(EXTreeNode *node, const std::vector<T> &values) {
         if (!node) return 1;
         if (node->type == EXTTT_NAMEID) {
            size_t nameID = node->valueID;
            if (nameID >= values.size()) return 1;
            return values[nameID];
         }
         if (node->type == EXTTT_NUM) return node->num;
         if (node->type == EXTTT_OP) {
            T leftVal = evaluate(node->left, values);
            T rightVal = evaluate(node->right, values);
            int leftIVal = (int) leftVal;
            int rightIVal = (int) rightVal;
            switch (node->op) {
               case OP_UNKNOWN:
                  return -1;
               case OPA_NEG:
                  return -rightIVal;
               case OPA_ADD:
                  return leftVal + rightVal;
               case OPA_SUB:
                  return leftVal - rightVal;
               case OPA_MUL:
                  return leftVal * rightVal;
               case OPA_DIV:
                  return leftVal / rightVal;
               case OPA_MOD:
                  return fmodf(leftVal, rightVal);

               case OPC_EQUAL:
                  return leftVal == rightVal;
               case OPC_NOTEQ:
                  return leftVal != rightVal;
               case OPC_LESST:
                  return leftVal < rightVal;
               case OPC_GREET:
                  return leftVal > rightVal;
               case OPC_ELEST:
                  return leftVal <= rightVal;
               case OPC_EGRET:
                  return leftVal >= rightVal;

               case OPL_AND:
                  return leftIVal && rightIVal;
               case OPL_NOT:
                  return !rightIVal;
               case OPL_OR:
                  return leftIVal || rightIVal;

               case OPB_BWAND:
                  return leftIVal & rightIVal;
               case OPB_BWOR:
                  return leftIVal | rightIVal;
               case OPB_BWXOR:
                  return leftIVal ^ rightIVal;
               case OPB_BWNOT:
                  return ~rightIVal;
               case OPB_BWLSH:
                  return leftIVal << rightIVal;
               case OPB_BWRSH:
                  return leftIVal >> rightIVal;

               default:
                  return -1;
            }
         }
         return -1;
      }

      static int getPrecedenceScore(EXOP_En op) {
         static std::unordered_map<EXOP_En, int> precedenceMap = {
               // Arithmetic Operators
               {OPA_NEG,   1},
               {OPA_ADD,   3},
               {OPA_SUB,   3},
               {OPA_MUL,   2},
               {OPA_DIV,   2},
               {OPA_MOD,   2},
               // Comparison Operators
               {OPC_EQUAL, 6},
               {OPC_NOTEQ, 6},
               {OPC_LESST, 5},
               {OPC_GREET, 5},
               {OPC_ELEST, 5},
               {OPC_EGRET, 5},
               // Logical Operators
               {OPL_AND,   10},
               {OPL_OR,    11},
               {OPL_NOT,   1},
               // Bitwise Operators
               {OPB_BWAND, 7},
               {OPB_BWOR,  8},
               {OPB_BWXOR, 9},
               {OPB_BWNOT, 1},
               {OPB_BWLSH, 4},
               {OPB_BWRSH, 4}
         };

         auto it = precedenceMap.find(op);
         if (it != precedenceMap.end()) {
            return it->second;
         }
         return 0;
      }

   };

}

using namespace StringUtils;

class CPreProcessor {
public:
   KeyWordSearch keywordSearch;
   KeyWordSearch keywordSearchAny;

   struct MacroDef {
      std::string macroAssembly;
      std::vector<std::string> args;
   };

   struct CPreProcDep {
      std::string code;
      std::string filename;
   };

   CPreProcessor();

   ~CPreProcessor();
private:
   void addMacro(const std::string& key, MacroDef& macroDef);
   void removeMacro(const std::string& key);
public:
   void define(const std::string& key, const std::vector<std::string>& args, const std::string& value);
   void define(const std::string& key, const std::string& value);
   void undef(const std::string& key);
   void preProcess(std::string& code, const std::string& filename = "undefined");
   void setKeyPrefix(const std::string& prefix = "#");

   std::vector<std::string> macroNameIDs;
   std::unordered_map<std::string, MacroDef> macros;
   std::vector<CPreProcDep> dependencies;
   std::vector<CPreProcDep> sysDependencies;

   int macroDefCountLim = 2048;
   int macroCallCountLim = 2048;
   int recursiveCallCountLim = 333;
   std::vector<std::string> errorLog;

private:
   int macroDefCount = 0;
   int macroCallCount = 0;
   int recursiveCallCount = 0;

   std::vector<std::string> includeGuards;
   std::vector<std::string> keyWords;

   void preProcess(std::string& code, const std::string& filename, bool resetsRec);

   void handleDefine(const std::vector<std::string>& lines, size_t& lID, const std::string& line, const KeyWordSearch::MatchDetail& found, const std::string& fileName);

   void handleUndef(const std::string& line, const KeyWordSearch::MatchDetail& found);

   void handleInclude(size_t &lID, std::string &result, std::string &line, KeyWordSearch::MatchDetail &found, const std::string &fileName);

   bool stateBranch(size_t& lID, std::vector<std::string>& lines, bool& state) const;

   bool depthBranch(int& stateDh, size_t& lID, std::vector<std::string>& lines, bool& state, bool& trueValid);

   bool calculateIf(std::string str);

   void handleStatement(std::vector<std::string>& lines, size_t& lID, std::string& result, std::string& line, KeyWordSearch::MatchDetail& found, int stateMentID, const std::string& fileName, bool& needNewLine);

   static void handleMacroArgs(std::string& macroAssembly, const std::vector<std::string>& argNames, const std::vector<std::string>& argValues);

   void handleMacroSearch(std::string& line, const std::string& fileName);

   void handleLine(std::vector<std::string>& lines, size_t& lID, std::string& result, const std::string& fileName, bool& needNewLine);
};
//PreProcessor Declarations:
/*PUBLIC*/
CPreProcessor::CPreProcessor() {
   keyWords.resize(11);
   keyWords[0] = "define ";
   keyWords[1] = "undef ";
   keyWords[2] = "include ";
   keyWords[3] = "if ";
   keyWords[4] = "ifdef ";
   keyWords[5] = "ifndef ";
   keyWords[6] = "else ";
   keyWords[7] = "elif ";
   keyWords[8] = "endif";
   keyWords[9] = "pragma once";
   keyWords[10] = "error";

   keywordSearchAny.addKeyWord("defined(", true, false);
   macroNameIDs.emplace_back("defined(");

   setKeyPrefix();
}

CPreProcessor::~CPreProcessor() = default;

void CPreProcessor::define(const std::string &key, const std::vector<std::string> &args, const std::string &value) {
   MacroDef macro;
   macro.macroAssembly = value;
   macro.args = args;
   addMacro(key, macro);
}

void CPreProcessor::define(const std::string &key, const std::string &value) {
   MacroDef macro;
   macro.macroAssembly = value;
   addMacro(key, macro);
}

void CPreProcessor::undef(const std::string &key) {
   removeMacro(key);
}

void CPreProcessor::preProcess(std::string &code, const std::string &filename) {
   errorLog.clear();
   preProcess(code, filename, true);
}

void CPreProcessor::setKeyPrefix(const std::string &prefix) {
   keywordSearch.keyWords.clear();

   for (const auto &keyWord: keyWords) {
      std::string searchWord = prefix + keyWord;
      keywordSearch.addKeyWord(searchWord, true, true);
   }
}

/*PRIVATE*/
void CPreProcessor::preProcess(std::string &code, const std::string &filename, bool resetsRec) {
   std::string result;
   removeTabs(code, 3);
   std::vector<std::string> lines = split(code, '\n');
   bool needNewLine = false;
   for (size_t lID = 0; lID < lines.size(); lID++) {
      if (resetsRec) recursiveCallCount = 0;
      handleLine(lines, lID, result, filename, needNewLine);
   }
   code = result;
}

void CPreProcessor::addMacro(const std::string &key, MacroDef &macroDef) {
   macros[key] = macroDef;
   macroNameIDs.push_back(key);
   keywordSearchAny.addKeyWord(key, true, false);
}

void CPreProcessor::removeMacro(const std::string &key) {
   macros.erase(key);
   auto it = std::remove(macroNameIDs.begin(), macroNameIDs.end(), key);
   macroNameIDs.erase(it, macroNameIDs.end());
   keywordSearchAny.removeKeyWord(key);
}

void CPreProcessor::handleDefine(const std::vector<std::string> &lines, size_t &lID, const std::string &line, const KeyWordSearch::MatchDetail &found, const std::string &fileName) {
   if (macroDefCount >= macroDefCountLim) return;
   macroDefCount++;

   size_t start = found.position + found.keyword.length();

   size_t tokenStart = 0;
   size_t tokenEnd = 0;
   if (findToken(line, start, tokenStart, tokenEnd)) {
      // getKey
      std::vector<std::string> foundArgs;
      std::string key = line.substrID(tokenStart, tokenEnd);
      // getArgs if there are args
      size_t argsStart = 0;
      size_t valueStart = tokenEnd;
      if (findMark(line, tokenEnd, argsStart, '(')) {
         size_t argsEnd = line.find(')', argsStart);
         argsEnd = std::min(argsEnd, line.length());

         std::string argStr = line.substrID(argsStart + 1, argsEnd);
         removeDuplicates(argStr, ' ', true);
         foundArgs = split(argStr, ',');
         valueStart = argsEnd + 1;
      }

      std::string value;

      bool firstLine = true;
      for (size_t lcID = lID; lcID < lines.size(); lcID++) {
         const std::string &checkML = lines[lcID];

         size_t markPos = checkML.length();
         bool foundMacroL = findMark(checkML, checkML.length(), markPos, '\\', true);

         if (firstLine) {
            value = checkML.substrID(std::min(valueStart + 1, checkML.length()), markPos);
            firstLine = false;
         } else value.append("\n" + checkML.substrID(0, markPos));

         if (!foundMacroL) break;
         lID++;
      }

      define(key, foundArgs, value);
   } else {
      errorLog.push_back("error: Define Ill formed in includeFile: " + fileName + " ,on line: " + std::to_string(lID));
   }
}

void CPreProcessor::handleUndef(const std::string &line, const KeyWordSearch::MatchDetail &found) {
   size_t start = found.position + found.keyword.length();

   size_t tokenStart = 0;
   size_t tokenEnd = 0;
   if (findToken(line, start, tokenStart, tokenEnd)) {
      std::string key = line.substrID(tokenStart, tokenEnd);
      undef(key);
   } else {
      // Nothing To undef(NotError)
   }
}

void CPreProcessor::handleInclude(size_t &lID, std::string &result, std::string &line, KeyWordSearch::MatchDetail &found, const std::string &fileName) {
   size_t start = found.position + found.keyword.length();

   std::string includeName;


   bool systemLib = false;
   size_t markPosStart = 0;
   if (findMark(line, start, markPosStart, '<')) {
      size_t markPosEnd = line.find('>', markPosStart);
      includeName = line.substrID(markPosStart + 1, markPosEnd);
      systemLib = true;

   } else if (findMark(line, start, markPosStart, '"')) {
      size_t markPosEnd = line.find('"', markPosStart + 1);
      includeName = line.substrID(markPosStart + 1, markPosEnd);

   } else {
      errorLog.push_back(
            "error: Ill formed in includeFile: " + fileName + " ,on line: " + std::to_string(lID) + " ,with: " +
            includeName);
      return;
   }

   std::string foundCode;
   bool foundDep = false;

   if (!systemLib) {
      for (const auto &iter: dependencies) {
         if (iter.filename == includeName) {
            foundCode = iter.code;
            foundDep = true;
            break;
         }
      }
   }
   if (systemLib || !foundDep) {
      for (const auto &iter: sysDependencies) {
         if (iter.filename == includeName) {
            foundCode = iter.code;
            foundDep = true;
            break;
         }
      }
   }


   if (!foundDep) {
      if (systemLib) {
         errorLog.push_back("error: Include Error in includeFile: " + fileName + " ,on line: " + std::to_string(lID) +
                            " ,with missing system depency: " + includeName);
      } else errorLog.push_back(
               "error: Include Error in includeFile: " + fileName + " ,on line: " + std::to_string(lID) +
               " ,with missing depency: " + includeName);
      return;
   }

   for (auto &includeGB: includeGuards) {
      if (includeGB == includeName) return;
   }

   preProcess(foundCode, includeName, true);
   result = result + "\n" + foundCode;
}

bool CPreProcessor::stateBranch(size_t &lID, std::vector<std::string> &lines, bool &state) const {
   if (lID >= lines.size() - 1) return false;
   lID++;
   const std::string &sLine = lines[lID];
   std::vector<KeyWordSearch::MatchDetail> foundStateMents = keywordSearch.findKeywords(sLine);
   for (const auto &iter: foundStateMents) {
      if (iter.id == 8) {
         return false;
      } else if (iter.id == 6) {
         state = false;
      } else if (iter.id == 7) {
         state = false;
      }
   }
   return true;
}

bool CPreProcessor::depthBranch(int &stateDh, size_t &lID, std::vector<std::string> &lines, bool &state, bool &trueValid) {
   if (lID >= lines.size() - 1) return false;
   if (stateDh <= 0) return false;
   lID++;

   const std::string &sLine = lines[lID];
   std::vector<KeyWordSearch::MatchDetail> foundStateMents = keywordSearch.findKeywords(sLine);
   for (const auto &iter: foundStateMents) {
      if (iter.id == 3 || iter.id == 4 || iter.id == 5) {
         stateDh++;
      }
      if (iter.id == 8) {
         stateDh--;
      } else if (iter.id == 6) {
         if (trueValid) {
            if (!state) {
               trueValid = false;
               state = true;
            }
         } else state = false;
      } else if (iter.id == 7) {
         if (trueValid) {
            size_t pePos = iter.position + iter.keyword.length();

            if (calculateIf(sLine.substrID(pePos, sLine.size()))) {
               trueValid = false;
               state = true;
            };
         } else state = false;
      }
   }
   return true;
}

bool CPreProcessor::calculateIf(std::string str) {
   std::vector<KeyWordSearch::MatchDetail> foundMacros = keywordSearchAny.findKeywords(str);
   std::vector<std::string> varList;
   std::vector<int> valList;
   for (const auto &keyWord: foundMacros) {
      std::string key = macroNameIDs[keyWord.id];

      if (key == "defined(") {
         size_t pePos = keyWord.position + keyWord.keyword.length();
         while (str[pePos] == ' ') {
            pePos++;
         }
         size_t afPos = pePos;
         std::string checkKey;
         while (!(str[afPos] == ' ' || str[afPos] == ')')) {
            checkKey += str[afPos];
            afPos++;
         }
         int state = macros.find(checkKey) != macros.end();
         while (str[afPos] != ')') {
            afPos++;
         }
         str.replaceID(keyWord.position, afPos, std::to_string(state));
      } else {
         varList.push_back(key);
         bool state = macros.find(key) != macros.end();
         if (state) {
            MacroDef macro = macros[key];
            std::string val = macro.macroAssembly;
            try {
               int value = std::stoi(val);
               valList.push_back(value);
            } catch (const std::invalid_argument &e) {
               valList.push_back(0);
            } catch (const std::out_of_range &e) {
               valList.push_back(0);
            }
         } else valList.push_back(0);
      }
   }

   ExpressionParser<int> parser;
   parser.buildTree(str, varList);
   int returnVal = parser.evaluate(valList);
   return returnVal == 1;
}

void CPreProcessor::handleStatement(std::vector<std::string> &lines, size_t &lID, std::string &result, std::string &line, KeyWordSearch::MatchDetail &found, int stateMentID, const std::string &fileName, bool &needNewLine) {
   switch (stateMentID) {
      case 3: {
         size_t pePos = found.position + found.keyword.length();

         bool state = calculateIf(line.substrID(pePos, line.size()));
         bool trueValid = !state;

         bool branch = true;
         int statementDepth = 1;
         while (branch) {
            if (state) {
               branch = stateBranch(lID, lines, state);
               if (!branch)
                  break;
               handleLine(lines, lID, result, fileName, needNewLine);
            } else {
               branch = depthBranch(statementDepth, lID, lines, state, trueValid);
            }
         }
         break;
      }
      case 4: {
         size_t start = found.position + found.keyword.length();

         size_t tokenStart = 0;
         size_t tokenEnd = 0;
         if (findToken(line, start, tokenStart, tokenEnd)) {
            std::string key = line.substrID(tokenStart, tokenEnd);

            bool state = macros.find(key) != macros.end();
            bool trueValid = !state;
            bool branch = true;
            int statementDepth = 1;
            while (branch) {
               if (state) {
                  branch = stateBranch(lID, lines, state);
                  if (!branch)
                     break;
                  handleLine(lines, lID, result, fileName, needNewLine);
               } else {
                  branch = depthBranch(statementDepth, lID, lines, state, trueValid);
               }
            }
         } else {
            errorLog.push_back(
                  "error: IfDefined Ill Formed in includeFile: " + fileName + " ,on line: " + std::to_string(lID));
         }
         break;
      }
      case 5: {
         size_t start = found.position + found.keyword.length();

         size_t tokenStart = 0;
         size_t tokenEnd = 0;
         if (findToken(line, start, tokenStart, tokenEnd)) {
            std::string key = line.substrID(tokenStart, tokenEnd);

            bool state = !(macros.find(key) != macros.end());
            bool trueValid = !state;
            bool branch = true;
            int statementDepth = 1;
            while (branch) {
               if (state) {
                  branch = stateBranch(lID, lines, state);
                  if (!branch)
                     break;
                  handleLine(lines, lID, result, fileName, needNewLine);
               } else {
                  branch = depthBranch(statementDepth, lID, lines, state, trueValid);
               }
            }
         } else {
            errorLog.push_back(
                  "error: IfNotDefined Ill Formed in includeFile: " + fileName + " ,on line: " + std::to_string(lID));
         }
         break;
      }
      default:
         break;
   }
}

void CPreProcessor::handleMacroArgs(std::string &macroAssembly, const std::vector<std::string> &argNames, const std::vector<std::string> &argValues) {
   if (argNames.size() != argValues.size()) return;

   for (int mArgID = 0; mArgID < argNames.size(); mArgID++) {
      const std::string &argName = argNames[mArgID];
      const std::string &argValue = argValues[mArgID];

      std::string sPattern =
            "\\b" + std::regex_replace(argName, std::regex(R"([-[\]{}()*+?.,\^$|#\s])"), R"(\$&)") + "\\b";
      std::string sPatternStr =
            "[^#]#\\s*\\b" + std::regex_replace(argName, std::regex(R"([-[\]{}()*+?.,\^$|#\s])"), R"(\$&)") + "\\b";

      std::regex pat(sPattern);
      std::regex patStr(sPatternStr);

      std::string replacement = "\"" + argValue + "\"";
      macroAssembly = std::regex_replace(macroAssembly, patStr, replacement);
      macroAssembly = std::regex_replace(macroAssembly, pat, argValue);
   }

   std::string sPatternMerge = R"(\s*##\s*\b)";
   std::regex patMerge(sPatternMerge);
   macroAssembly = std::regex_replace(macroAssembly, patMerge, "");
}

void CPreProcessor::handleMacroSearch(std::string &line, const std::string &fileName) {
   if (macroCallCount >= macroCallCountLim) return;
   if (recursiveCallCount >= recursiveCallCountLim) return;
   macroCallCount++;
   recursiveCallCount++;

   std::vector<KeyWordSearch::MatchDetail> foundMacros = keywordSearchAny.findKeywords(line);
   size_t mcCount = foundMacros.size();
   size_t macroExShift = 0;
   std::string lineReplace = line;
   for (int mcID = 0; mcID < mcCount; mcID++) {
      KeyWordSearch::MatchDetail found = foundMacros[mcID];
      if (found.id == 0) continue;

      std::string macroKey = macroNameIDs[found.id];
      size_t start = found.position + found.keyword.length();

      std::vector<std::string> argValues;

      size_t argsStart = 0;
      size_t macroEnd = start;
      if (findMark(line, start, argsStart, '(')) {
         size_t argsEnd = line.find(')', argsStart);
         argsEnd = std::min(argsEnd, line.length());

         std::string argStr = line.substrID(argsStart + 1, argsEnd);
         removeDuplicates(argStr, ' ', true);
         argValues = split(argStr, ',');
         macroEnd = argsEnd + 1;
      }

      std::string macroExtend = macros[macroKey].macroAssembly;
      std::vector<std::string> argNames = macros[macroKey].args;

      handleMacroArgs(macroExtend, argNames, argValues);
      preProcess(macroExtend, fileName, false);

      size_t sizeSave = lineReplace.size();
      lineReplace.replaceID(macroExShift + found.position, macroExShift + macroEnd, macroExtend);
      macroExShift += lineReplace.size() - sizeSave;
   }
   line = lineReplace;
}

void CPreProcessor::handleLine(std::vector<std::string> &lines, size_t &lID, std::string &result, const std::string &fileName, bool &needNewLine) {
   std::string line = lines[lID];
   if (line.empty()) return;

   std::vector<KeyWordSearch::MatchDetail> foundKeyWords = keywordSearch.findKeywords(line);
   size_t count = foundKeyWords.size();
   for (int kwID = 0; kwID < count; kwID++) {
      KeyWordSearch::MatchDetail found = foundKeyWords[kwID];
      switch (found.id) {
         case 0: {
            handleDefine(lines, lID, line, found, fileName);
            break;
         }
         case 1: {
            handleUndef(line, found);
            break;
         }
         case 2: {
            handleInclude(lID, result, line, found, fileName);
            break;
         }
         case 3: {
            handleStatement(lines, lID, result, line, found, 3, fileName, needNewLine);
            break;
         }
         case 4: {
            handleStatement(lines, lID, result, line, found, 4, fileName, needNewLine);
            break;
         }
         case 5: {
            handleStatement(lines, lID, result, line, found, 5, fileName, needNewLine);
            break;
         }
         case 9: {
            includeGuards.push_back(fileName);
            break;
         }
         case 10: {
            size_t start = found.position + found.keyword.length();
            std::string errorCode = line.substrID(start, line.length());

            std::string message = (errorCode.empty() || errorCode == " ") ? "" : (" ,with error: " + errorCode);
            errorLog.push_back(
                  "error: ERROR(BREAK) in includeFile: " + fileName + " ,on line: " + std::to_string(lID) + message);

            break;
         }
         default:
            break;
      }
   }

   if (count != 0) return;

   std::string newLine = line;
   handleMacroSearch(newLine, fileName);

   if (needNewLine) result += "\n";
   needNewLine = true;
   result += newLine;
}


#undef substrID
#undef replaceID
#endif //GIT_CL_C_PRE_PROCESSOR_C_PRE_PROCESSOR_H
