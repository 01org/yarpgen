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

#pragma once

#include "context.h"
#include "enums.h"
#include "expr.h"
#include "ir_node.h"

#include <iostream>
#include <memory>
#include <utility>

namespace yarpgen {

class Stmt : public IRNode {
  public:
    virtual IRNodeKind getKind() { return IRNodeKind::MAX_STMT_KIND; }
};

class ExprStmt : public Stmt {
  public:
    explicit ExprStmt(std::shared_ptr<Expr> _expr) : expr(std::move(_expr)) {}
    IRNodeKind getKind() final { return IRNodeKind::EXPR; }

    std::shared_ptr<Expr> getExpr() { return expr; }

    void emit(std::ostream &stream, std::string offset = "") final;
    static std::shared_ptr<ExprStmt> create(std::shared_ptr<PopulateCtx> ctx);

  private:
    std::shared_ptr<Expr> expr;
};

class DeclStmt : public Stmt {
  public:
    explicit DeclStmt(std::shared_ptr<Data> _data) : data(std::move(_data)) {}
    DeclStmt(std::shared_ptr<Data> _data, std::shared_ptr<Expr> _expr)
        : data(std::move(_data)), init_expr(std::move(_expr)) {}
    IRNodeKind getKind() final { return IRNodeKind::DECL; }
    void emit(std::ostream &stream, std::string offset = "") final;

  private:
    std::shared_ptr<Data> data;
    std::shared_ptr<Expr> init_expr;
};

class StmtBlock : public Stmt {
  public:
    StmtBlock() = default;
    explicit StmtBlock(std::vector<std::shared_ptr<Stmt>> _stmts)
        : stmts(std::move(_stmts)) {}
    IRNodeKind getKind() override { return IRNodeKind::BLOCK; }

    void addStmt(std::shared_ptr<Stmt> stmt) {
        stmts.push_back(std::move(stmt));
    }

    void emit(std::ostream &stream, std::string offset = "") override;
    static std::shared_ptr<StmtBlock>
    generateStructure(std::shared_ptr<GenCtx> ctx);
    void populate(std::shared_ptr<PopulateCtx> ctx) override;

  protected:
    std::vector<std::shared_ptr<Stmt>> stmts;
};

class ScopeStmt : public StmtBlock {
  public:
    IRNodeKind getKind() final { return IRNodeKind::SCOPE; }
    void emit(std::ostream &stream, std::string offset = "") final;
    static std::shared_ptr<ScopeStmt>
    generateStructure(std::shared_ptr<GenCtx> ctx);
};

class LoopStmt : public Stmt {};

class LoopHead {
  public:
    std::shared_ptr<StmtBlock> getPrefix() { return prefix; }
    void addPrefix(std::shared_ptr<StmtBlock> _prefix) {
        prefix = std::move(_prefix);
    }
    void addIterator(std::shared_ptr<Iterator> _iter) {
        iters.push_back(std::move(_iter));
    }
    std::shared_ptr<StmtBlock> getSuffix() { return suffix; }
    void addSuffix(std::shared_ptr<StmtBlock> _suffix) {
        suffix = std::move(_suffix);
    }
    void emitPrefix(std::ostream &stream, std::string offset = "");
    void emitHeader(std::ostream &stream, std::string offset = "");
    void emitSuffix(std::ostream &stream, std::string offset = "");

  private:
    std::shared_ptr<StmtBlock> prefix;
    // Loop iterations space is defined by the iterators that we can use
    std::vector<std::shared_ptr<Iterator>> iters;
    std::shared_ptr<StmtBlock> suffix;
};

// According to the agreement, a single standalone loop should be represented as a LoopSeqStmt of size one
class LoopSeqStmt : public LoopStmt {
  public:
    IRNodeKind getKind() final { return IRNodeKind::LOOP_SEQ; }
    void addLoop(std::pair<std::shared_ptr<LoopHead>,
                           std::shared_ptr<ScopeStmt>> _loop) {
        loops.push_back(std::move(_loop));
    }
    void emit(std::ostream &stream, std::string offset = "") final;
    static std::shared_ptr<LoopSeqStmt>
    generateStructure(std::shared_ptr<GenCtx> ctx);
    void populate(std::shared_ptr<PopulateCtx> ctx) override;

  private:
    std::vector<
        std::pair<std::shared_ptr<LoopHead>, std::shared_ptr<ScopeStmt>>>
        loops;
};

class LoopNestStmt : public LoopStmt {
  public:
    IRNodeKind getKind() final { return IRNodeKind::LOOP_NEST; }
    void addLoop(std::shared_ptr<LoopHead> _loop) {
        loops.push_back(std::move(_loop));
    }
    void addBody(std::shared_ptr<ScopeStmt> _body) { body = std::move(_body); }
    void emit(std::ostream &stream, std::string offset = "") final;
    static std::shared_ptr<LoopNestStmt>
    generateStructure(std::shared_ptr<GenCtx> ctx);
    void populate(std::shared_ptr<PopulateCtx> ctx) override;

  private:
    std::vector<std::shared_ptr<LoopHead>> loops;
    std::shared_ptr<StmtBlock> body;
};

class StubStmt : public Stmt {
  public:
    explicit StubStmt(std::string _text) : text(std::move(_text)) {}
    IRNodeKind getKind() final { return IRNodeKind::STUB; }

    void emit(std::ostream &stream, std::string offset = "") final;
    static std::shared_ptr<StubStmt>
    generateStructure(std::shared_ptr<GenCtx> ctx);

  private:
    std::string text;
};
} // namespace yarpgen
