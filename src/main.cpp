#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <fstream>
#include <sstream>

#include "ast.h"
#include "koopa.h"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto paser_ret = yyparse(ast);
  assert(!paser_ret);

  ofstream yyout(output);
  
  stringstream buffer;
  ast->ASTTrav(buffer);
  string ast_string = buffer.str();
  const char* str = ast_string.c_str();

  //解析字符串，得到IR程序
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);

  assert(ret == KOOPA_EC_SUCCESS);
  yyout << "  .text\n";

  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  //将IR程序转换为raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  //释放Koopa IR程序占有的内存
  koopa_delete_program(program);

  assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
  yyout << "  .globl main" << endl;
  for(size_t i=0; i<raw.funcs.len; i++){

    koopa_raw_function_t func = (koopa_raw_function_t)(raw.funcs.buffer[i]);
    yyout << func->name+1 << ":" << endl;

    assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
    for(size_t j=0; j < func->bbs.len; j++){

      koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)(func->bbs.buffer[j]);
      //yyout << bb->name << ":" << endl;

      assert(bb->insts.kind == KOOPA_RSIK_VALUE);
      for(size_t k=0; k<bb->insts.len; k++){
        koopa_raw_value_t value = (koopa_raw_value_t)(bb->insts.buffer[k]);
        
        assert(value->kind.tag == KOOPA_RVT_RETURN);
        //return指令中，value代表返回值
        koopa_raw_value_t ret_value = value->kind.data.ret.value;
        assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
        int32_t int_value = ret_value->kind.data.integer.value;
        yyout << "  li a0, " << int_value << endl << "  ret";
        
        //assert(int_value == 0);

      }
    }
  }


  // 处理完成, 释放 raw program builder 占用的内存
  // 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
  // 所以不要在 raw program 处理完毕之前释放 builder
  koopa_delete_raw_program_builder(builder);

  return 0;
}
