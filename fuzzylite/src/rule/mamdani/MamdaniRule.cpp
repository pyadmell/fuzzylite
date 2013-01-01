/*
 * MamdaniRule.cpp
 *
 *  Created on: 3/12/2012
 *      Author: jcrada
 */

#include "fl/rule/mamdani/MamdaniRule.h"

#include "fl/rule/mamdani/MamdaniAntecedent.h"
#include "fl/rule/mamdani/MamdaniConsequent.h"

#include "fl/operator/Operator.h"

#include "fl/definitions.h"
#include "fl/Exception.h"

#include <vector>

namespace fl {

    MamdaniRule::MamdaniRule()
    : Rule() { }

    MamdaniRule::~MamdaniRule() { }

    MamdaniRule* MamdaniRule::parse(const std::string& rule, const Engine* engine) {
        MamdaniRule* result = new MamdaniRule();
        result->setUnparsedRule(rule);
        
        std::istringstream tokenizer(rule);
        std::string token;
        std::ostringstream ossAntecedent, ossConsequent;

        enum FSM {
            S_NONE, S_IF, S_THEN, S_WITH, S_END
        };
        FSM state = S_NONE;
        try {
            while (tokenizer >> token) {

                switch (state) {
                    case S_NONE:
                        if (token == Rule::FL_IF) state = S_IF;
                        else FL_LOG("ignored token <" << token << "> in rule: " << rule);
                        break;
                    case S_IF:
                        if (token == Rule::FL_THEN) state = S_THEN;
                        else ossAntecedent << token << " ";
                        break;
                    case S_THEN:
                        if (token == Rule::FL_WITH) state = S_WITH;
                        else ossConsequent << token << " ";
                        break;
                    case S_WITH:
                        try {
                            result->setWeight(fl::Op::toScalar(token));
                            state = S_END;
                        } catch (fl::Exception& e) {
                            std::ostringstream ex;
                            ex << "[syntax error] expected a numeric value as the weight of the rule: "
                                    << rule;
                            e.appendDetail(ex.str());
                            throw e;
                        }
                        break;
                    case S_END:
                        std::ostringstream ex;
                        ex << "[syntax error] unexpected token <" << token << "> after the end of rule";
                        throw fl::Exception(ex.str());
                }
            }
            if (state == S_NONE) {
                std::ostringstream ex;
                ex << "[syntax error] keyword <" << Rule::FL_IF << "> not found in rule: " << rule;
                throw fl::Exception(ex.str());
            } else if (state == S_IF) {
                std::ostringstream ex;
                ex << "[syntax error] keyword <" << Rule::FL_THEN << "> not found in rule: " << rule;
                throw fl::Exception(ex.str());
            } else if (state == S_WITH) {
                std::ostringstream ex;
                ex << "[syntax error] expected a numeric value as the weight of the rule: "
                        << rule;
                throw fl::Exception(ex.str());
            }

            MamdaniAntecedent* antecedent = new MamdaniAntecedent;
            antecedent->load(ossAntecedent.str(), engine);
            result->setAntecedent(antecedent);

            MamdaniConsequent* consequent = new MamdaniConsequent;
            consequent->load(ossConsequent.str(), engine);
            result->setConsequent(consequent);
        } catch (fl::Exception& ex) {
            delete result;
            throw ex;
        }
        return result;
    }

    void MamdaniRule::main() {
        //        std::vector<std::string> tokens;
        //        regex_t regexCompiled;
        //        regcomp(&regexCompiled, "(^\\s*if\\s+)(.*)(\\s+then\\s+)(.*)", REG_EXTENDED);
        //
        //        int numberOfTokens = regexCompiled.re_nsub + 1;
        //        regmatch_t matches[numberOfTokens];
        //
        //        if (regexec(&regexCompiled, rule.c_str(), numberOfTokens, matches, 0)
        //                == 0) {
        //            for (int i = 0; i < numberOfTokens; ++i) {
        //                if (matches[i].rm_so == -1) { //FL_LOG("No more tokens");
        //                    break;
        //                }
        //                std::string token = rule.substr(matches[i].rm_so,
        //                        matches[i].rm_eo - matches[i].rm_so);
        //                tokens.push_back(token);
        //            }
        //        }
        //        regfree(&regexCompiled);
        //        if (tokens.size() != 5) {
        //            FL_LOG("expected five (5) tokens, but found " << tokens.size() << " in rule: " << rule);
        //        }
    }

}
