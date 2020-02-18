/*
Copyright (c) 2015-2020, Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//////////////////////////////////////////////////////////////////////////////

#include "expr.h"
#include "context.h"
#include <algorithm>
#include <utility>

using namespace yarpgen;

std::unordered_map<std::shared_ptr<Data>, std::shared_ptr<ScalarVarUseExpr>>
    yarpgen::ScalarVarUseExpr::scalar_var_use_set;
std::unordered_map<std::shared_ptr<Data>, std::shared_ptr<ArrayUseExpr>>
    yarpgen::ArrayUseExpr::array_use_set;
std::unordered_map<std::shared_ptr<Data>, std::shared_ptr<IterUseExpr>>
    yarpgen::IterUseExpr::iter_use_set;

std::shared_ptr<Data> Expr::getValue() {
    // TODO: it might cause some problems in the future, but it is good for now
    return value;
}

ConstantExpr::ConstantExpr(IRValue _value) {
    // TODO: maybe we need a constant data type rather than an anonymous scalar
    // variable
    value = std::make_shared<ScalarVar>(
        "", IntegralType::init(_value.getIntTypeID()), _value);
}

Expr::EvalResType ConstantExpr::evaluate(EvalCtx &ctx) { return value; }

Expr::EvalResType ConstantExpr::rebuild(EvalCtx &ctx) { return evaluate(ctx); }

void ConstantExpr::emit(std::ostream &stream, std::string offset) {
    assert(value->isScalarVar() &&
           "ConstExpr can represent only scalar constant");
    auto scalar_var = std::static_pointer_cast<ScalarVar>(value);

    assert(scalar_var->getType()->isIntType() &&
           "ConstExpr can represent only scalar integral constant");
    auto int_type =
        std::static_pointer_cast<IntegralType>(scalar_var->getType());

    IRValue val = scalar_var->getCurrentValue();
    IRValue min_val = int_type->getMin();
    if (!int_type->getIsSigned() || (val != min_val).getValueRef<bool>()) {
        stream << val << int_type->getLiteralSuffix();
        return;
    }

    // INT_MIN is defined as -INT_MAX - 1, so we have to do the same
    IRValue one(val.getIntTypeID());
    // TODO: this is not an appropriate way to do it
    one.setValue(IRValue::AbsValue{false, 1});
    IRValue min_one_val = min_val + one;
    stream << "(" << min_one_val << " - " << one << ")";
}

std::shared_ptr<ScalarVarUseExpr>
ScalarVarUseExpr::init(std::shared_ptr<Data> _val) {
    assert(_val->isScalarVar() &&
           "ScalarVarUseExpr accepts only scalar variables!");
    auto find_res = scalar_var_use_set.find(_val);
    if (find_res != scalar_var_use_set.end())
        return find_res->second;

    auto ret = std::make_shared<ScalarVarUseExpr>(_val);
    scalar_var_use_set[_val] = ret;
    return ret;
}

void ScalarVarUseExpr::setValue(std::shared_ptr<Expr> _expr) {
    std::shared_ptr<Data> new_val = _expr->getValue();
    assert(new_val->isScalarVar() && "Can store only scalar variables!");
    if (value->getType() != new_val->getType())
        ERROR("Can't assign different types!");

    std::static_pointer_cast<ScalarVar>(value)->setCurrentValue(
        std::static_pointer_cast<ScalarVar>(new_val)->getCurrentValue());
}

Expr::EvalResType ScalarVarUseExpr::evaluate(EvalCtx &ctx) {
    // This variable is defined and we can just return it.
    auto find_res = ctx.input.find(value->getName());
    if (find_res != ctx.input.end()) {
        return find_res->second;
    }
    return value;
}

Expr::EvalResType ScalarVarUseExpr::rebuild(EvalCtx &ctx) {
    return evaluate(ctx);
}

std::shared_ptr<ArrayUseExpr> ArrayUseExpr::init(std::shared_ptr<Data> _val) {
    assert(_val->isArray() &&
           "ArrayUseExpr can be initialized only with Arrays");
    auto find_res = array_use_set.find(_val);
    if (find_res != array_use_set.end())
        return find_res->second;

    auto ret = std::make_shared<ArrayUseExpr>(_val);
    array_use_set[_val] = ret;
    return ret;
}

void ArrayUseExpr::setValue(std::shared_ptr<Expr> _expr) {
    std::shared_ptr<Data> new_val = _expr->getValue();
    assert(new_val->isArray() && "ArrayUseExpr can store only Arrays");
    auto new_array = std::static_pointer_cast<Array>(new_val);
    if (value->getType() != new_array->getType()) {
        ERROR("Can't assign incompatible types");
    }
    auto arr_val = std::static_pointer_cast<Array>(value);
    arr_val->setValue(new_val);
}

Expr::EvalResType ArrayUseExpr::evaluate(EvalCtx &ctx) {
    // This Array is defined and we can just return it.
    auto find_res = ctx.input.find(value->getName());
    if (find_res != ctx.input.end()) {
        return find_res->second;
    }

    return value;
}

Expr::EvalResType ArrayUseExpr::rebuild(EvalCtx &ctx) { return evaluate(ctx); }

std::shared_ptr<IterUseExpr> IterUseExpr::init(std::shared_ptr<Data> _iter) {
    assert(_iter->isIterator() && "IterUseExpr accepts only iterators!");
    auto find_res = iter_use_set.find(_iter);
    if (find_res != iter_use_set.end())
        return find_res->second;

    auto ret = std::make_shared<IterUseExpr>(_iter);
    iter_use_set[_iter] = ret;
    return ret;
}

void IterUseExpr::setValue(std::shared_ptr<Expr> _expr) {
    std::shared_ptr<Data> new_val = _expr->getValue();
    assert(new_val->isIterator() && "IterUseExpr can store only iterators!");
    auto new_iter = std::static_pointer_cast<Iterator>(new_val);
    if (value->getType() != new_iter->getType())
        ERROR("Can't assign different types!");

    std::static_pointer_cast<Iterator>(value)->setParameters(
        new_iter->getStart(), new_iter->getEnd(), new_iter->getStep());
}

Expr::EvalResType IterUseExpr::evaluate(EvalCtx &ctx) {
    // This iterator is defined and we can just return it.
    auto find_res = ctx.input.find(value->getName());
    if (find_res != ctx.input.end()) {
        return find_res->second;
    }

    return value;
}

Expr::EvalResType IterUseExpr::rebuild(EvalCtx &ctx) { return evaluate(ctx); }

TypeCastExpr::TypeCastExpr(std::shared_ptr<Expr> _expr,
                           std::shared_ptr<Type> _to_type, bool _is_implicit)
    : expr(std::move(_expr)), to_type(std::move(_to_type)),
      is_implicit(_is_implicit) {
    std::shared_ptr<Type> base_type = expr->getValue()->getType();
    // Check that we try to convert between compatible types.
    if (!((base_type->isIntType() && to_type->isIntType()) ||
          (base_type->isArrayType() && to_type->isArrayType()))) {
        ERROR("Can't create TypeCastExpr for types that can't be casted");
    }

    if (base_type->isIntType() && expr->getValue()->isScalarVar()) {
        std::shared_ptr<IntegralType> to_int_type =
            std::static_pointer_cast<IntegralType>(to_type);
        value = std::make_shared<ScalarVar>(
            "", to_int_type, IRValue(to_int_type->getIntTypeId()));
        std::shared_ptr<ScalarVar> scalar_val =
            std::static_pointer_cast<ScalarVar>(value);
        std::shared_ptr<ScalarVar> base_scalar_var =
            std::static_pointer_cast<ScalarVar>(expr->getValue());
        scalar_val->setCurrentValue(
            base_scalar_var->getCurrentValue().castToType(
                to_int_type->getIntTypeId()));
    }
    else {
        // TODO: extend it
        ERROR("We can cast only integer scalar variables for now");
    }
}

void TypeCastExpr::emit(std::ostream &stream, std::string offset) {
    // TODO: add switch for C++ style conversions and switch for implicit casts
    stream << "((" << (is_implicit ? "/* implicit */" : "")
           << to_type->getName() << ") ";
    expr->emit(stream);
    stream << ")";
}

std::shared_ptr<Expr> ArithmeticExpr::integralProm(std::shared_ptr<Expr> arg) {
    if (!arg->getValue()->isScalarVar()) {
        ERROR("Can perform integral promotion only on scalar variables");
    }

    //[conv.prom]
    assert(arg->getValue()->getType()->isIntType() &&
           "Scalar variable can have only Integral Type");
    std::shared_ptr<IntegralType> int_type =
        std::static_pointer_cast<IntegralType>(arg->getValue()->getType());
    if (int_type->getIntTypeId() >=
        IntTypeID::INT) // can't perform integral promotion
        return arg;
    // TODO: we need to check if type fits in int or unsigned int
    return std::make_shared<TypeCastExpr>(
        arg, IntegralType::init(IntTypeID::INT), true);
}

std::shared_ptr<Expr> ArithmeticExpr::convToBool(std::shared_ptr<Expr> arg) {
    if (!arg->getValue()->isScalarVar()) {
        ERROR("Can perform conversion to bool only on scalar variables");
    }

    std::shared_ptr<IntegralType> int_type =
        std::static_pointer_cast<IntegralType>(arg->getValue()->getType());
    if (int_type->getIntTypeId() == IntTypeID::BOOL)
        return arg;
    return std::make_shared<TypeCastExpr>(
        arg, IntegralType::init(IntTypeID::BOOL), true);
}

std::shared_ptr<ArithmeticExpr> ArithmeticExpr::create(std::shared_ptr<PopulateCtx> ctx) {
    auto inp_var_id = rand_val_gen->getRandValue(static_cast<size_t>(0), ctx->getExtInpSymTablet()->getVars().size() - 1);
    auto inp_var = ctx->getExtInpSymTablet()->getVars().at(inp_var_id);
    auto inp_var_expr = std::make_shared<ScalarVarUseExpr>(inp_var);
    UnaryOp op = rand_val_gen->getRandId(ctx->getGenPolicy()->unary_op_distr);
    return std::make_shared<UnaryExpr>(op, inp_var_expr);
}

bool UnaryExpr::propagateType() {
    arg->propagateType();
    switch (op) {
        case UnaryOp::PLUS:
        case UnaryOp::NEGATE:
        case UnaryOp::BIT_NOT:
            arg = integralProm(arg);
            break;
        case UnaryOp::LOG_NOT:
            arg = convToBool(arg);
            break;
        case UnaryOp::MAX_UN_OP:
            ERROR("Bad unary operator");
            break;
    }
    return true;
}

Expr::EvalResType UnaryExpr::evaluate(EvalCtx &ctx) {
    assert(arg->getValue()->getKind() == DataKind::VAR &&
           "Unary operations are supported for Scalar Variables only");
    auto scalar_arg = std::static_pointer_cast<ScalarVar>(arg->getValue());
    IRValue new_val;
    switch (op) {
        case UnaryOp::PLUS:
            new_val = +scalar_arg->getCurrentValue();
            break;
        case UnaryOp::NEGATE:
            new_val = -scalar_arg->getCurrentValue();
            break;
        case UnaryOp::LOG_NOT:
            new_val = !scalar_arg->getCurrentValue();
            break;
        case UnaryOp::BIT_NOT:
            new_val = ~scalar_arg->getCurrentValue();
            break;
        case UnaryOp::MAX_UN_OP:
            ERROR("Bad unary operator");
            break;
    }
    assert(scalar_arg->getType()->isIntType() &&
           "Unary operations are supported for Scalar Variables of Integral "
           "Types only");
    auto int_type =
        std::static_pointer_cast<IntegralType>(scalar_arg->getType());
    value = std::make_shared<ScalarVar>("", int_type, new_val);
    return value;
}

Expr::EvalResType UnaryExpr::rebuild(EvalCtx &ctx) {
    EvalResType eval_res = evaluate(ctx);
    assert(eval_res->getKind() == DataKind::VAR &&
           "Unary operations are supported for Scalar Variables of Integral "
           "Types only");
    auto eval_scalar_res = std::static_pointer_cast<ScalarVar>(eval_res);
    if (!eval_scalar_res->getCurrentValue().hasUB()) {
        value = eval_res;
        return value;
    }

    if (op == UnaryOp::NEGATE) {
        op = UnaryOp::PLUS;
    }
    else {
        ERROR("Something went wrong, this should be unreachable");
    }

    do {
        eval_res = evaluate(ctx);
        eval_scalar_res = std::static_pointer_cast<ScalarVar>(eval_res);
        if (!eval_scalar_res->getCurrentValue().hasUB())
            break;
        rebuild(ctx);
    } while (eval_scalar_res->getCurrentValue().hasUB());

    value = eval_res;
    return value;
}

void UnaryExpr::emit(std::ostream &stream, std::string offset) {
    stream << offset << "(";
    switch (op) {
        case UnaryOp::PLUS:
            stream << "+";
            break;
        case UnaryOp::NEGATE:
            stream << "-";
            break;
        case UnaryOp::LOG_NOT:
            stream << "!";
            break;
        case UnaryOp::BIT_NOT:
            stream << "~";
            break;
        case UnaryOp::MAX_UN_OP:
            ERROR("Bad unary operator");
            break;
    }
    stream << "(";
    arg->emit(stream);
    stream << "))";
}

bool BinaryExpr::propagateType() {
    lhs->propagateType();
    rhs->propagateType();

    switch (op) {
        case BinaryOp::ADD:
        case BinaryOp::SUB:
        case BinaryOp::MUL:
        case BinaryOp::DIV:
        case BinaryOp::MOD:
        case BinaryOp::LT:
        case BinaryOp::GT:
        case BinaryOp::LE:
        case BinaryOp::GE:
        case BinaryOp::EQ:
        case BinaryOp::NE:
        case BinaryOp::BIT_AND:
        case BinaryOp::BIT_OR:
        case BinaryOp::BIT_XOR:
            // Arithmetic conversions
            lhs = integralProm(lhs);
            rhs = integralProm(rhs);
            arithConv();
            break;
        case BinaryOp::SHL:
        case BinaryOp::SHR:
            lhs = integralProm(lhs);
            rhs = integralProm(rhs);
            break;
        case BinaryOp::LOG_AND:
        case BinaryOp::LOG_OR:
            lhs = convToBool(lhs);
            rhs = convToBool(rhs);
            break;
        case BinaryOp::MAX_BIN_OP:
            ERROR("Bad operation code");
            break;
    }
    return true;
}

void BinaryExpr::arithConv() {
    if (!lhs->getValue()->getType()->isIntType() ||
        !rhs->getValue()->getType()->isIntType()) {
        ERROR("We assume that we can perform binary operations only in Scalar "
              "Variables with integral type");
    }

    std::shared_ptr<IntegralType> lhs_type =
        std::static_pointer_cast<IntegralType>(lhs->getValue()->getType());
    std::shared_ptr<IntegralType> rhs_type =
        std::static_pointer_cast<IntegralType>(rhs->getValue()->getType());

    //[expr.arith.conv]
    // 1.5.1
    if (lhs_type->getIntTypeId() == rhs_type->getIntTypeId())
        return;

    // 1.5.2
    if (lhs_type->getIsSigned() == rhs_type->getIsSigned()) {
        std::shared_ptr<IntegralType> max_type = IntegralType::init(
            std::max(lhs_type->getIntTypeId(), rhs_type->getIntTypeId()));
        if (lhs_type->getIntTypeId() > rhs_type->getIntTypeId())
            rhs = std::make_shared<TypeCastExpr>(rhs, max_type,
                                                 /*is_implicit*/ true);
        else
            lhs = std::make_shared<TypeCastExpr>(lhs, max_type,
                                                 /*is_implicit*/ true);
        return;
    }

    // 1.5.3
    // Helper function that converts signed type to "bigger" unsigned type
    auto signed_to_unsigned_conv = [](std::shared_ptr<IntegralType> &a_type,
                                      std::shared_ptr<IntegralType> &b_type,
                                      std::shared_ptr<Expr> &b_expr) -> bool {
        if (!a_type->getIsSigned() &&
            (a_type->getIntTypeId() >= b_type->getIntTypeId())) {
            b_expr = std::make_shared<TypeCastExpr>(b_expr, a_type,
                                                    /*is_implicit*/ true);
            return true;
        }
        return false;
    };
    if (signed_to_unsigned_conv(lhs_type, rhs_type, rhs) ||
        signed_to_unsigned_conv(rhs_type, lhs_type, lhs))
        return;

    // 1.5.4
    // Same idea, but for unsigned to signed conversions
    auto unsigned_to_signed_conv = [](std::shared_ptr<IntegralType> &a_type,
                                      std::shared_ptr<IntegralType> &b_type,
                                      std::shared_ptr<Expr> &b_expr) -> bool {
        if (a_type->getIsSigned() &&
            IntegralType::canRepresentType(a_type->getIntTypeId(),
                                           b_type->getIntTypeId())) {
            b_expr = std::make_shared<TypeCastExpr>(b_expr, a_type,
                                                    /*is_implicit*/ true);
            return true;
        }
        return false;
    };
    if (unsigned_to_signed_conv(lhs_type, rhs_type, rhs) ||
        unsigned_to_signed_conv(rhs_type, lhs_type, lhs))
        return;

    // 1.5.5
    auto final_conversion = [](std::shared_ptr<IntegralType> &a_type,
                               std::shared_ptr<Expr> &a_expr,
                               std::shared_ptr<Expr> &b_expr) -> bool {
        if (a_type->getIsSigned()) {
            std::shared_ptr<IntegralType> new_type = IntegralType::init(
                IntegralType::getCorrUnsigned(a_type->getIntTypeId()));
            a_expr = std::make_shared<TypeCastExpr>(a_expr, new_type,
                                                    /*is_implicit*/ true);
            b_expr = std::make_shared<TypeCastExpr>(b_expr, new_type,
                                                    /*is_implicit*/ true);
            return true;
        }
        return false;
    };
    if (final_conversion(lhs_type, lhs, rhs) ||
        final_conversion(rhs_type, lhs, rhs))
        return;

    ERROR("Unreachable: conversions went wrong");
}

Expr::EvalResType BinaryExpr::evaluate(EvalCtx &ctx) {
    propagateType();
    if (lhs->getValue()->getKind() != DataKind::VAR ||
        rhs->getValue()->getKind() != DataKind::VAR) {
        ERROR("Binary operations are supported only for scalar variables");
    }

    lhs->evaluate(ctx);
    rhs->evaluate(ctx);

    auto lhs_scalar_var = std::static_pointer_cast<ScalarVar>(lhs->getValue());
    auto rhs_scalar_var = std::static_pointer_cast<ScalarVar>(rhs->getValue());

    IRValue lhs_val = lhs_scalar_var->getCurrentValue();
    IRValue rhs_val = rhs_scalar_var->getCurrentValue();

    IRValue new_val(lhs_val.getIntTypeID());

    switch (op) {
        case BinaryOp::ADD:
            new_val = lhs_val + rhs_val;
            break;
        case BinaryOp::SUB:
            new_val = lhs_val - rhs_val;
            break;
        case BinaryOp::MUL:
            new_val = lhs_val * rhs_val;
            break;
        case BinaryOp::DIV:
            new_val = lhs_val / rhs_val;
            break;
        case BinaryOp::MOD:
            new_val = lhs_val % rhs_val;
            break;
        case BinaryOp::LT:
            new_val = lhs_val < rhs_val;
            break;
        case BinaryOp::GT:
            new_val = lhs_val > rhs_val;
            break;
        case BinaryOp::LE:
            new_val = lhs_val <= rhs_val;
            break;
        case BinaryOp::GE:
            new_val = lhs_val >= rhs_val;
            break;
        case BinaryOp::EQ:
            new_val = lhs_val == rhs_val;
            break;
        case BinaryOp::NE:
            new_val = lhs_val != rhs_val;
            break;
        case BinaryOp::LOG_AND:
            new_val = lhs_val && rhs_val;
            break;
        case BinaryOp::LOG_OR:
            new_val = lhs_val || rhs_val;
            break;
        case BinaryOp::BIT_AND:
            new_val = lhs_val & rhs_val;
            break;
        case BinaryOp::BIT_OR:
            new_val = lhs_val | rhs_val;
            break;
        case BinaryOp::BIT_XOR:
            new_val = lhs_val ^ rhs_val;
            break;
        case BinaryOp::SHL:
            new_val = lhs_val << rhs_val;
            break;
        case BinaryOp::SHR:
            new_val = lhs_val >> rhs_val;
            break;
        case BinaryOp::MAX_BIN_OP:
            ERROR("Bad operator code");
            break;
    }

    assert(lhs->getValue()->getType()->isIntType() &&
           "Binary operations are supported only for Scalar Variables of "
           "Integral Type");
    auto res_int_type =
        std::static_pointer_cast<IntegralType>(lhs->getValue()->getType());
    value = std::make_shared<ScalarVar>("", res_int_type, new_val);
    return value;
}

Expr::EvalResType BinaryExpr::rebuild(EvalCtx &ctx) {
    std::shared_ptr<Data> eval_res = evaluate(ctx);
    assert(eval_res->getKind() == DataKind::VAR &&
           "Binary operations are supported only for Scalar Variables");

    auto eval_scalar_res = std::static_pointer_cast<ScalarVar>(eval_res);
    if (!eval_scalar_res->getCurrentValue().hasUB()) {
        value = eval_res;
        return eval_res;
    }

    UBKind ub = eval_scalar_res->getCurrentValue().getUBCode();

    switch (op) {
        case BinaryOp::ADD:
            op = BinaryOp::SUB;
            break;
        case BinaryOp::SUB:
            op = BinaryOp::ADD;
            break;
        case BinaryOp::MUL:
            op = ub == UBKind::SignOvfMin ? BinaryOp::SUB : BinaryOp::DIV;
            break;
        case BinaryOp::DIV:
        case BinaryOp::MOD:
            op = ub == UBKind::ZeroDiv ? BinaryOp::MUL : BinaryOp::SUB;
            break;
        case BinaryOp::SHR:
        case BinaryOp::SHL:
            if (ub == UBKind::ShiftRhsLarge || ub == UBKind::ShiftRhsNeg) {
                // First of all, we need to find the maximal valid shift value
                assert(lhs->getValue()->getType()->isIntType() &&
                       "Binary operations are supported only for Scalar "
                       "Variables of Integral Types");
                auto lhs_int_type = std::static_pointer_cast<IntegralType>(
                    lhs->getValue()->getType());
                assert(lhs->getValue()->getKind() == DataKind::VAR &&
                       "Binary operations are supported only for Scalar "
                       "Variables");
                auto lhs_scalar_var =
                    std::static_pointer_cast<ScalarVar>(lhs->getValue());
                // We can't shift pass the type size
                size_t max_sht_val = lhs_int_type->getBitSize();
                // And we can't shift MSB pass the type size
                if (op == BinaryOp::SHL && lhs_int_type->getIsSigned() &&
                    ub == UBKind::ShiftRhsLarge) {
                    IRValue::AbsValue lhs_abs_val =
                        lhs_scalar_var->getCurrentValue().getAbsValue();
                    max_sht_val -= findMSB(lhs_abs_val.value);
                }

                // Secondly, we choose a new shift value in a valid range
                size_t new_val = rand_val_gen->getRandValue(
                    static_cast<size_t>(0), max_sht_val);

                // Thirdly, we need to combine the chosen value with the
                // existing one
                assert(rhs->getValue()->getType()->isIntType() &&
                       "Binary operations are supported only for Scalar "
                       "Variables of Integral Types");
                auto rhs_int_type = std::static_pointer_cast<IntegralType>(
                    rhs->getValue()->getType());
                assert(rhs->getValue()->getKind() == DataKind::VAR &&
                       "Binary operations are supported only for Scalar "
                       "Variables");
                auto rhs_scalar_var =
                    std::static_pointer_cast<ScalarVar>(rhs->getValue());
                IRValue::AbsValue rhs_abs_val =
                    rhs_scalar_var->getCurrentValue().getAbsValue();
                if (ub == UBKind::ShiftRhsNeg)
                    // TODO: it won't work for INT_MIN
                    new_val = std::min(
                        new_val + rhs_abs_val.value,
                        static_cast<uint64_t>(rhs_int_type->getBitSize()));
                // UBKind::ShiftRhsLarge
                else
                    new_val = rhs_abs_val.value - new_val;

                // Finally, we need to make changes to the program
                IRValue adjust_val = IRValue(rhs_int_type->getIntTypeId());
                adjust_val.setValue(IRValue::AbsValue{false, new_val});
                auto const_val = std::make_shared<ConstantExpr>(adjust_val);
                if (ub == UBKind::ShiftRhsNeg)
                    rhs = std::make_shared<BinaryExpr>(BinaryOp::ADD, rhs,
                                                       const_val);
                // UBKind::ShiftRhsLarge
                else
                    rhs = std::make_shared<BinaryExpr>(BinaryOp::SUB, rhs,
                                                       const_val);
            }
            // UBKind::NegShift
            else {
                // We can just add maximal value of the type
                assert(lhs->getValue()->getType()->isIntType() &&
                       "Binary operations are supported only for Scalar "
                       "Variables of Integral Types");
                auto lhs_int_type = std::static_pointer_cast<IntegralType>(
                    lhs->getValue()->getType());
                auto const_val =
                    std::make_shared<ConstantExpr>(lhs_int_type->getMax());
                lhs =
                    std::make_shared<BinaryExpr>(BinaryOp::ADD, lhs, const_val);
            }
            break;
        case BinaryOp::LT:
        case BinaryOp::GT:
        case BinaryOp::LE:
        case BinaryOp::GE:
        case BinaryOp::EQ:
        case BinaryOp::NE:
        case BinaryOp::BIT_AND:
        case BinaryOp::BIT_OR:
        case BinaryOp::BIT_XOR:
        case BinaryOp::LOG_AND:
        case BinaryOp::LOG_OR:
            break;
        case BinaryOp::MAX_BIN_OP:
            ERROR("Bad binary operator");
            break;
    }

    do {
        eval_res = evaluate(ctx);
        eval_scalar_res = std::static_pointer_cast<ScalarVar>(eval_res);
        if (!eval_scalar_res->getCurrentValue().hasUB())
            break;
        rebuild(ctx);
    } while (eval_scalar_res->getCurrentValue().hasUB());

    value = eval_res;
    return eval_res;
}

void BinaryExpr::emit(std::ostream &stream, std::string offset) {
    stream << offset << "(";
    lhs->emit(stream);
    stream << ")";
    switch (op) {
        case BinaryOp::ADD:
            stream << " + ";
            break;
        case BinaryOp::SUB:
            stream << " - ";
            break;
        case BinaryOp::MUL:
            stream << " * ";
            break;
        case BinaryOp::DIV:
            stream << " / ";
            break;
        case BinaryOp::MOD:
            stream << " % ";
            break;
        case BinaryOp::LT:
            stream << " < ";
            break;
        case BinaryOp::GT:
            stream << " > ";
            break;
        case BinaryOp::LE:
            stream << " <= ";
            break;
        case BinaryOp::GE:
            stream << " >= ";
            break;
        case BinaryOp::EQ:
            stream << " == ";
            break;
        case BinaryOp::NE:
            stream << " != ";
            break;
        case BinaryOp::LOG_AND:
            stream << " && ";
            break;
        case BinaryOp::LOG_OR:
            stream << " || ";
            break;
        case BinaryOp::BIT_AND:
            stream << " & ";
            break;
        case BinaryOp::BIT_OR:
            stream << " | ";
            break;
        case BinaryOp::BIT_XOR:
            stream << " ^ ";
            break;
        case BinaryOp::SHL:
            stream << " << ";
            break;
        case BinaryOp::SHR:
            stream << " >> ";
            break;
        case BinaryOp::MAX_BIN_OP:
            ERROR("Bad binary operator");
            break;
    }
    stream << "(";
    rhs->emit(stream);
    stream << ")";
}

bool SubscriptExpr::propagateType() {
    array->propagateType();
    idx->propagateType();
    return true;
}

bool SubscriptExpr::inBounds(size_t dim, std::shared_ptr<Data> idx_val,
                             EvalCtx &ctx) {
    if (idx_val->isScalarVar()) {
        auto scalar_var = std::static_pointer_cast<ScalarVar>(idx_val);
        IRValue idx_scalar_val = scalar_var->getCurrentValue();
        idx_int_type_id = idx_scalar_val.getIntTypeID();
        IRValue zero(idx_scalar_val.getIntTypeID());
        zero.setValue({false, 0});
        IRValue size(idx_scalar_val.getIntTypeID());
        size.setValue({false, dim});
        return (zero <= idx_scalar_val).getValueRef<bool>() &&
               (idx_scalar_val <= size).getValueRef<bool>();
    }
    else if (idx_val->isIterator()) {
        auto iter_var = std::static_pointer_cast<Iterator>(idx_val);
        return inBounds(dim, iter_var->getStart()->evaluate(ctx), ctx) &&
               inBounds(dim, iter_var->getEnd()->evaluate(ctx), ctx);
    }
    else {
        ERROR("We can use only Scalar Variables or Iterator as index");
    }
    return false;
}

Expr::EvalResType SubscriptExpr::evaluate(EvalCtx &ctx) {
    propagateType();

    EvalResType array_eval_res = array->evaluate(ctx);
    if (!array_eval_res->getType()->isArrayType()) {
        ERROR("Subscription operation is supported only for Array");
    }
    auto array_type =
        std::static_pointer_cast<ArrayType>(array_eval_res->getType());

    IRNodeKind base_expr_kind = array->getKind();
    if (base_expr_kind == IRNodeKind::ARRAY_USE)
        active_dim = 0;
    else if (base_expr_kind == IRNodeKind::SUBSCRIPT) {
        auto base_expr = std::static_pointer_cast<SubscriptExpr>(array);
        active_dim = base_expr->getActiveDim() + 1;
    }
    else
        ERROR("Bad base expression for Subscription operation");

    active_size = array_type->getDimensions().at(active_dim);
    UBKind ub_code = UBKind::NoUB;

    EvalResType idx_eval_res = idx->evaluate(ctx);
    if (!inBounds(active_size, idx_eval_res, ctx))
        ub_code = UBKind::OutOfBounds;

    if (active_dim < array_type->getDimensions().size())
        value = array_eval_res;
    else {
        auto array_val = std::static_pointer_cast<Array>(array_eval_res);
        value = array_val->getCurrentValues();
    }

    value->setUBCode(ub_code);

    return value;
}

Expr::EvalResType SubscriptExpr::rebuild(EvalCtx &ctx) {
    EvalResType eval_res = evaluate(ctx);
    if (!eval_res->hasUB())
        return eval_res;

    assert(eval_res->getUBCode() == UBKind::OutOfBounds &&
           "Every other UB should be handled before");

    IRValue active_size_val(idx_int_type_id);
    active_size_val.setValue({false, active_size});
    auto size_constant = std::make_shared<ConstantExpr>(active_size_val);
    idx = std::make_shared<BinaryExpr>(BinaryOp::MOD, idx, size_constant);

    eval_res = evaluate(ctx);
    assert(eval_res->hasUB() && "All of the UB should be fixed by now");
    value = eval_res;
    return eval_res;
}

void SubscriptExpr::emit(std::ostream &stream, std::string offset) {
    stream << offset;
    // TODO: it may cause some problems in the future
    array->emit(stream);
    stream << " [";
    idx->emit(stream);
    stream << "]";
}

bool AssignmentExpr::propagateType() {
    to->propagateType();
    from->propagateType();
    // We need to cast the type of the expression, but we can't always do it
    // here.
    // The problem is that the type of the Subscript Expression is unknown utill
    // we evaluate it.
    return true;
}

Expr::EvalResType AssignmentExpr::evaluate(EvalCtx &ctx) {
    propagateType();
    EvalResType to_eval_res = to->evaluate(ctx);
    // TODO: we don't need to always do it
    from = std::make_shared<TypeCastExpr>(from, to_eval_res->getType(),
                                          /*is_implicit*/ true);
    EvalResType from_eval_res = from->evaluate(ctx);
    if (to_eval_res->getKind() != from_eval_res->getKind())
        ERROR("We can't assign incompatible data types");

    if (!taken)
        return from_eval_res;

    if (to->getKind() == IRNodeKind::SCALAR_VAR_USE) {
        auto to_scalar = std::static_pointer_cast<ScalarVarUseExpr>(to);
        to_scalar->setValue(from);
    }
    else if (to->getKind() == IRNodeKind::ITER_USE) {
        auto to_iter = std::static_pointer_cast<IterUseExpr>(to);
        to_iter->setValue(from);
    }
    else if (to->getKind() == IRNodeKind::ARRAY_USE) {
        auto to_array = std::static_pointer_cast<ArrayUseExpr>(to);
        to_array->setValue(from);
    }
    else
        ERROR("Bad IRNodeKind");

    return from_eval_res;
}

Expr::EvalResType AssignmentExpr::rebuild(EvalCtx &ctx) {
    return evaluate(ctx);
}

void AssignmentExpr::emit(std::ostream &stream, std::string offset) {
    stream << offset;
    to->emit(stream);
    stream << " = ";
    from->emit(stream);
}

std::shared_ptr<AssignmentExpr> AssignmentExpr::create(std::shared_ptr<PopulateCtx> ctx) {
    auto new_var = ScalarVar::create(ctx);
    ctx->getExtOutSymTablet()->addVar(new_var);
    auto to = std::make_shared<ScalarVarUseExpr>(new_var);
    auto from = ArithmeticExpr::create(ctx);
    return std::make_shared<AssignmentExpr>(to, from);
}
