#ifndef IR_H
#define IR_H
#include <string>
#include <vector>
#include <utility>
#include <assert.h>
#include <stdio.h>

#include "IRVisitor.h"

namespace HalideInternal {

    using std::string;
    using std::vector;
    using std::pair;

    struct Type {
        enum {Int, UInt, Float} t;
        int bits;
        int width;        
    };

    Type Int(int bits, int width = 1);
    Type UInt(int bits, int width = 1);
    Type Float(int bits, int width = 1);


    struct IRNode {
        virtual void accept(IRVisitor *v) const = 0;
        IRNode() : ref_count(0) {}
        virtual ~IRNode() {}
        mutable int ref_count;
    };

    struct BaseExprNode : public IRNode {
    };

    struct BaseStmtNode : public IRNode {
    };

    template<typename T>
    struct ExprNode : public BaseExprNode {
        void accept(IRVisitor *v) const {
            v->visit((const T *)this);
        }
    };

    template<typename T>
    struct StmtNode : public BaseStmtNode {
        void accept(IRVisitor *v) const {
            v->visit((const T *)this);
        }
    };
    
    struct IRHandle {
    private:
        const IRNode *node;
        void incref() {
            if (node) {
                node->ref_count++;
            }
        };
        void decref() {
            if (node) {
                node->ref_count--;
                if (node->ref_count == 0) {
                    delete node;
                    node = NULL;
                }
            }
        }

    protected:
        ~IRHandle() {
            decref();
        }
        IRHandle() : node(NULL) {}
        IRHandle(const IRNode *n) : node(n) {
            incref();
        }
        IRHandle(const IRHandle &other) : node(other.node) {
            incref();
        }
        IRHandle &operator=(const IRHandle &other) {
            decref();
            node = other.node;
            incref();
            return *this;
        }
    public:
        void accept(IRVisitor *v) const {
            node->accept(v);
        }
        bool defined() const {
            return node;
        }
        // Equality of reference
        bool sameAs(const IRHandle &other) {
            return node == other.node;
        }
    };

    struct Expr : public IRHandle {
        Expr() : IRHandle() {}
        Expr(const BaseExprNode *n) : IRHandle(n) {}

        // Some more constructors for convenience to make constants
        // TODO: cache these for efficiency
        Expr(int);
        Expr(float);
    };

    struct Stmt : public IRHandle {
        Stmt() : IRHandle() {}
        Stmt(const BaseStmtNode *n) : IRHandle(n) {}
        virtual const Allocate *asAllocate() {return NULL;}
    };

    struct IntImm : public ExprNode<IntImm> {
        int value;

        IntImm(int v) : value(v) {}
    };

    struct FloatImm : public ExprNode<FloatImm> {
        float value;

        FloatImm(float v) : value(v) {}
    };

    struct Cast : public ExprNode<Cast> {
        Type type;
        Expr value;

        Cast(Type t, Expr v) : type(t), value(v) {
            assert(v.defined() && "Cast of undefined");
        }
    };

    struct Var : public ExprNode<Var> {
        Type type;
        string name;

        Var(Type t, string n) : type(t), name(n) {}
    };

    struct Add : public ExprNode<Add> {
        Expr a, b;

        Add(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Add of undefined");
            assert(b.defined() && "Add of undefined");
        }
    };

    struct Sub : public ExprNode<Sub> {
        Expr a, b;

        Sub(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Sub of undefined");
            assert(b.defined() && "Sub of undefined");
        }
    };

    struct Mul : public ExprNode<Mul> {
        Expr a, b;

        Mul(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Mul of undefined");
            assert(b.defined() && "Mul of undefined");
        }        
    };

    struct Div : public ExprNode<Div> {
        Expr a, b;

        Div(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Div of undefined");
            assert(b.defined() && "Div of undefined");
        }
    };

    struct Mod : public ExprNode<Mod> {
        Expr a, b;

        Mod(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Mod of undefined");
            assert(b.defined() && "Mod of undefined");
        }
    };

    struct Min : public ExprNode<Min> {
        Expr a, b;

        Min(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Min of undefined");
            assert(b.defined() && "Min of undefined");
        }
    };

    struct Max : public ExprNode<Max> {
        Expr a, b;

        Max(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Max of undefined");
            assert(b.defined() && "Max of undefined");
        }
    };

    struct EQ : public ExprNode<EQ> {
        Expr a, b;

        EQ(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "EQ of undefined");
            assert(b.defined() && "EQ of undefined");
        }
    };

    struct NE : public ExprNode<NE> {
        Expr a, b;

        NE(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "NE of undefined");
            assert(b.defined() && "NE of undefined");
        }
    };

    struct LT : public ExprNode<LT> {
        Expr a, b;

        LT(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "LT of undefined");
            assert(b.defined() && "LT of undefined");
        }
    };

    struct LE : public ExprNode<LE> {
        Expr a, b;

        LE(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "LE of undefined");
            assert(b.defined() && "LE of undefined");
        }
    };

    struct GT : public ExprNode<GT> {
        Expr a, b;

        GT(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "GT of undefined");
            assert(b.defined() && "GT of undefined");
        }
    };

    struct GE : public ExprNode<GE> {
        Expr a, b;

        GE(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "GE of undefined");
            assert(b.defined() && "GE of undefined");
        }
    };

    struct And : public ExprNode<And> {
        Expr a, b;

        And(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "And of undefined");
            assert(b.defined() && "And of undefined");
        }
    };

    struct Or : public ExprNode<Or> {
        Expr a, b;

        Or(Expr _a, Expr _b) : a(_a), b(_b) {
            assert(a.defined() && "Or of undefined");
            assert(b.defined() && "Or of undefined");
        }
    };

    struct Not : public ExprNode<Not> {
        Expr a;

        Not(Expr _a) : a(_a) {
            assert(a.defined() && "Not of undefined");
        }
    };

    struct Select : public ExprNode<Select> {
        Expr condition, true_value, false_value;

        Select(Expr c, Expr t, Expr f) : 
            condition(c), true_value(t), false_value(f) {
            assert(condition.defined() && "Select of undefined");
            assert(true_value.defined() && "Select of undefined");
            assert(false_value.defined() && "Select of undefined");
        }
    };

    struct Load : public ExprNode<Load> {
        Type type;
        string buffer;
        Expr index;

        Load(Type t, string b, Expr i) : 
            type(t), buffer(b), index(i) {
            assert(index.defined() && "Load of undefined");
        }
    };

    struct Ramp : public ExprNode<Ramp> {
        Expr base, stride;
        int width;

        Ramp(Expr b, Expr s, int w) : 
            base(b), stride(s), width(w) {
            assert(base.defined() && "Ramp of undefined");
            assert(stride.defined() && "Ramp of undefined");
            assert(w > 0 && "Ramp of width <= 0");
        }
    };

    struct Call : public ExprNode<Call> {
        Type type;
        string buffer;
        vector<Expr > args;
        typedef enum {Image, Extern, Halide} CallType;
        CallType call_type;

        Call(Type t, string b, const vector<Expr > &a, CallType ct) : 
            type(t), buffer(b), args(a), call_type(ct) {
            for (size_t i = 0; i < args.size(); i++) {
                assert(args[i].defined() && "Call of undefined");
            }
        }
    };

    struct Let : public ExprNode<Let> {
        string name;
        Expr value, body;

        Let(string n, Expr v, Expr b) : 
            name(n), value(v), body(b) {
            assert(value.defined() && "Let of undefined");
            assert(body.defined() && "Let of undefined");
        }
    };

    struct LetStmt : public StmtNode<LetStmt> {
        string name;
        Expr value;
        Stmt body;

        LetStmt(string n, Expr v, Stmt b) : 
            name(n), value(v), body(b) {
            assert(value.defined() && "LetStmt of undefined");
            assert(body.defined() && "LetStmt of undefined");
        }
    };

    struct PrintStmt : public StmtNode<PrintStmt> {
        string prefix;
        vector<Expr > args;

        PrintStmt(string p, const vector<Expr > &a) :
            prefix(p), args(a) {
            for (size_t i = 0; i < args.size(); i++) {
                assert(args[i].defined() && "PrintStmt of undefined");
            }
        }
    };

    struct AssertStmt : public StmtNode<AssertStmt> {
        // if condition then val else error out with message
        Expr condition;
        string message;

        AssertStmt(Expr c, string m) :
            condition(c), message(m) {
            assert(condition.defined() && "AssertStmt of undefined");
        }
    };

    struct Pipeline : public StmtNode<Pipeline> {
        string buffer;
        Stmt produce, update, consume;

        Pipeline(string b, Stmt p, Stmt u, Stmt c) : 
            buffer(b), produce(p), update(u), consume(c) {
            assert(produce.defined() && "Pipeline of undefined");
            // update is allowed to be null
            assert(consume.defined() && "Pipeline of undefined");
        }
    };
    
    struct For : public StmtNode<For> {
        string name;
        Expr min, extent;
        typedef enum {Serial, Parallel, Vectorized, Unrolled} ForType;
        ForType for_type;
        Stmt body;

        For(string n, Expr m, Expr e, ForType f, Stmt b) :
            name(n), min(m), extent(e), for_type(f), body(b) {
            assert(min.defined() && "For of undefined");
            assert(extent.defined() && "For of undefined");
            assert(body.defined() && "For of undefined");
        }
    };

    struct Store : public StmtNode<Store> {
        string buffer;
        Expr value, index;

        Store(string b, Expr v, Expr i) :
            buffer(b), value(v), index(i) {
            assert(value.defined() && "Store of undefined");
            assert(index.defined() && "Store of undefined");
        }
    };

    struct Provide : public StmtNode<Provide> {
        string buffer;
        Expr value;
        vector<Expr > args;

        Provide(string b, Expr v, const vector<Expr > &a) : 
            buffer(b), value(v), args(a) {
            assert(value.defined() && "Provide of undefined");
            for (size_t i = 0; i < args.size(); i++) {
                assert(args[i].defined() && "Provide of undefined");
            }
        }
    };

    struct Allocate : public StmtNode<Allocate> {
        string buffer;
        Type type;
        Expr size;
        Stmt body;

        Allocate(string buf, Type t, Expr s, Stmt bod) : 
            buffer(buf), type(t), size(s), body(bod) {
            assert(size.defined() && "Allocate of undefined");
            assert(body.defined() && "Allocate of undefined");
        }
    };

    struct Realize : public StmtNode<Realize> {
        string buffer;
        Type type;
        vector<pair<Expr , Expr > > bounds;
        Stmt body;

        Realize(string buf, Type t, const vector<pair<Expr, Expr> > &bou, Stmt bod) : 
            buffer(buf), type(t), bounds(bou), body(bod) {
            for (size_t i = 0; i < bounds.size(); i++) {
                assert(bounds[i].first.defined() && "Realize of undefined");
                assert(bounds[i].second.defined() && "Realize of undefined");
            }
            assert(body.defined() && "Realize of undefined");
        }
    };

    struct Block : public StmtNode<Block> {
        Stmt first, rest;
        
        Block(Stmt f, Stmt r) : 
            first(f), rest(r) {
            assert(first.defined() && "Block of undefined");
            // rest is allowed to be null
        }
    };
}

#endif
