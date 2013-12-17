#include <iostream>
#include "codegen.h"
#include "node.h"

extern "C" int wow();

llvm::Function* insertWowFunction(CodeGenContext& context)
{
    llvm::FunctionType* type =
    llvm::FunctionType::get(
        llvm::Type::getInt32Ty(getGlobalContext()), false);

    llvm::Function *func = llvm::Function::Create(
        type, llvm::Function::ExternalLinkage,
        llvm::Twine("wow"),
                                                  context.module
    );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* insertPlzFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> args;
    args.push_back(llvm::Type::getInt8PtrTy(getGlobalContext())); //char*

    llvm::FunctionType* printf_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(getGlobalContext()), args, false);

    llvm::Function *func = llvm::Function::Create(
        printf_type, llvm::Function::ExternalLinkage,
        llvm::Twine("plz"),
                                                  context.module
    );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* insertPrintfFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(getGlobalContext())); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(getGlobalContext()), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}


void createSuchFunction(CodeGenContext& context, llvm::Function* printfFn)
{
    std::vector<llvm::Type*> echo_arg_types;
    echo_arg_types.push_back(llvm::Type::getInt64Ty(getGlobalContext()));

    llvm::FunctionType* echo_type =
        llvm::FunctionType::get(
            llvm::Type::getVoidTy(getGlobalContext()), echo_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                echo_type, llvm::Function::InternalLinkage,
                llvm::Twine("such"),
                context.module
           );
    llvm::BasicBlock *bblock = llvm::BasicBlock::Create(getGlobalContext(), "entry", func, 0);
	context.pushBlock(bblock);

    const char *constValue = "Such %d\n";
    llvm::Constant *format_const = llvm::ConstantDataArray::getString(getGlobalContext(), constValue);
    llvm::GlobalVariable *var =
        new llvm::GlobalVariable(
            *context.module, llvm::ArrayType::get(llvm::IntegerType::get(getGlobalContext(), 8), strlen(constValue)+1),
            true, llvm::GlobalValue::PrivateLinkage, format_const, ".str");
    llvm::Constant *zero =
        llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(getGlobalContext()));

    std::vector<llvm::Constant*> indices;
    indices.push_back(zero);
    indices.push_back(zero);
    llvm::Constant *var_ref = llvm::ConstantExpr::getGetElementPtr(var, indices);

    std::vector<Value*> args;
    args.push_back(var_ref);

    Function::arg_iterator argsValues = func->arg_begin();
    Value* toPrint = argsValues++;
    toPrint->setName("toPrint");
    args.push_back(toPrint);

	CallInst *call = CallInst::Create(printfFn, makeArrayRef(args), "", bblock);
	ReturnInst::Create(getGlobalContext(), bblock);
	context.popBlock();
}

void createCoreFunctions(CodeGenContext& context){
    insertWowFunction(context);
    insertPlzFunction(context);
    llvm::Function* printfFn = insertPrintfFunction(context);
    if (!printfFn) {
        std::cout << "Error" << std::endl;
        wow(); //We can actually use it here xD
    }
    createSuchFunction(context, printfFn);
}
