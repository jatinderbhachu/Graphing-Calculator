#include "exprtk.hpp"

/*
 * Singleton object used to parse and evaluate math expressions
 */
class ExpressionParser {
    public:
        static ExpressionParser* getParser();
        bool compile(std::string expression);
        double eval();

        void registerVar(std::string var, double& varRef);

    private:
        ExpressionParser();
        static ExpressionParser* expParserInstance;
        exprtk::symbol_table<double> symbol_table;
        exprtk::expression<double> expression;
        exprtk::parser<double> parser;
        double x;

};
