#pragma once

#include <iostream>

#include "Macros.h"
#include "Object.h"
#include "Ref.h"
#include "String.h"

#define EXPRESSION_VISITOR                                      \
        virtual Ref<Object> Accept(ExprVisitor & visitor) const \
        {                                                       \
            return visitor.Visit(*this);                        \
        }                                                       \

namespace Finch
{
    class ExprVisitor;
    class Object;
    
    class Expr
    {
    public:
        virtual ~Expr() {}
        
        //### bob: coupling ast to interpreter (object) here is gross.
        // wish you could do template virtual methods. :(
        virtual Ref<Object> Accept(ExprVisitor & visitor) const = 0;
        
        virtual void Trace(std::ostream & stream) const = 0;
    };
    
    std::ostream& operator<<(std::ostream& cout, const Expr & expr);
}