#pragma once
#include <memory>
#include <iostream>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual void ASTTrav(std::ostream& out) const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override{
    std::cout << "CompUnitAST { "; 
    func_def->Dump();
    std::cout << " }" ;
  }

  void ASTTrav(std::ostream& out) const override {
    func_def->ASTTrav(out);
  }
};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }

  void ASTTrav(std::ostream& out) const override {
    out << "fun @" << ident << "(): ";
    func_type->ASTTrav(out);
    block->ASTTrav(out);
    out << "}" << std::endl;
  }
};

// ...
class FuncTypeAST : public BaseAST {
 public:
  std::string INT;

  void Dump() const override {
    std::cout << "FuncTypeAST { " << INT << " }";
  }

  void ASTTrav(std::ostream& out) const override {
    if(INT == "int"){
      out << "i32 {" << std::endl;
    }
  }
};

class BlockAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> stmt;

    void Dump() const override {
      std::cout << "BlockAST { ";
      stmt->Dump();
      std::cout << " }";
    }

    void ASTTrav(std::ostream& out) const override {
      out << "%entry:" << std::endl;
      stmt->ASTTrav(out);
    }
};

class StmtAST : public BaseAST{
  public:
    std::string RETURN;
    //std::unique_ptr<BaseAST> number;
    int number;
  
    void Dump() const override {
      std::cout << "StmtAST { " << RETURN << ", ";
      //std::cout << "StmtAST { ";
      //number->Dump();
      //std::cout << " }";
      std::cout << number << " }";
    }

    void ASTTrav(std::ostream& out) const override {
      if(RETURN == "return"){
        out << "ret";
      }
      out << ' ' << number << std::endl;
    }
};


/* class NumberAST : public BaseAST {
  public:
    int INT_CONST;

    void Dump() const override {
      std::cout << "NumberAST { " << INT_CONST << " }";
    }
}; */