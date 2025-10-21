#include <iostream>
#include <memory>
#include "ast.h"

class BaseIR {
    public:
        virtual ~BaseIR() = default;

        virtual void ASTTrav(std::ostream& out, std::unique_ptr<BaseAST> &ast) const = 0;
};

class ProgramIR : public BaseIR {
    public:
        std::unique_ptr<FunctionIR> function;

        virtual void ASTTrav(std::ostream& out, std::unique_ptr<BaseAST> &ast) const override {
            /* auto next_ast = ast;        
            function->ASTTrav(out, std::unique_ptr<CompUnitAST>(ast)->func_def); */
        }
};

class FunctionIR : public BaseIR {
    public:
        std::string name;

        std::unique_ptr<BasicBlockIR> basic_block;

        virtual void ASTTrav(std::ostream& out, std::unique_ptr<BaseAST> &ast) const override {
            
        }
};

class BasicBlockIR : public BaseIR {
    public:
        std::string name;

        std::unique_ptr<ValueIR> Return;
        std::unique_ptr<ValueIR> Integer;
};

class ValueIR : public BaseIR {
    public:
        std::string value;
};