#include "ExpressionParser.h"

ExpressionParser* ExpressionParser::expParserInstance = 0;


ExpressionParser::ExpressionParser() {

    expression.register_symbol_table(symbol_table);
}

ExpressionParser* ExpressionParser::getParser() {
    if(expParserInstance == 0){
        expParserInstance = new ExpressionParser();
    }

    return expParserInstance;
}

void ExpressionParser::registerVar(std::string var, double& varRef) {
    symbol_table.add_variable(var, varRef);
    symbol_table.add_constants();
}

bool ExpressionParser::compile(std::string exp){
    return parser.compile(exp, expression);
}

double ExpressionParser::eval() {
    return expression.value();
}
