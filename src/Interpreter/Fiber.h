#pragma once

#include "CodeBlock.h"
#include "Macros.h"
#include "Object.h"
#include "Ref.h"
#include "Scope.h"
#include "Stack.h"
#include "Upvalue.h"

//#define TRACE_INSTRUCTIONS

namespace Finch
{
    class Environment;
    class Expr;
    class Interpreter;
    
    // A single bytecode execution thread in the interpreter. A Fiber has a
    // virtual callstack and is responsible for executing bytecode. In other
    // words, it's where stuff actually happens.
    class Fiber
    {
    public:
        Fiber(Interpreter & interpreter, Ref<Object> block);

        bool IsRunning() const { return mIsRunning && !IsDone(); }
        
        bool IsDone() const;

        Ref<Object> Execute();
        
        Interpreter & GetInterpreter() { return mInterpreter; }

        void Pause() { mIsRunning = false; }

        Ref<Object> Nil();
        Ref<Object> CreateBool(bool value);
        Ref<Object> CreateNumber(double value);
        Ref<Object> CreateString(const String & value);
        
        // Pushes the given block onto the call stack.
        void CallBlock(Ref<Object> receiver, Ref<Object> blockObj, ArgReader & args);

        // Displays a runtime error to the user.
        void Error(const String & message);
        
        // Gets the current number of stack frames on the callstack. Used as a
        // diagnostic to ensure that tail call optimization is working.
        int GetCallstackDepth() const;
    private:
        // A single stack frame on the virtual callstack.
        struct CallFrame
        {
            // The instruction pointer. Stores the index of the current bytecode
            // instruction in the bytecode for this frame.
            int ip;
            
            // The index on the stack of the first register for this frame.
            int stackStart;

            // The current receiver.
            Ref<Object> receiver;
            
            // The block of code being executed by this frame.
            Ref<Object> block;
            
            CallFrame()
            :   ip(0),
                stackStart(0),
                receiver(),
                block()
            {}
            
            CallFrame(int stackStart, Ref<Object> receiver, Ref<Object> block)
            :   ip(0),
                stackStart(stackStart),
                receiver(receiver),
                block(block)
            {}

            // Gets the code object for this frame.
            const BlockObject & Block() const { return *(block->AsBlock()); }
        };
        
        // Loads a register for the given callframe.
        Ref<Object> Load(const CallFrame & frame, int reg);
        
        // Stores a register for the given callframe.
        void Store(const CallFrame & frame, int reg, Ref<Object> value);
        
        void PopCallFrame();
        void StoreMessageResult(Ref<Object> result);

        Environment & GetEnvironment();
        
        Ref<Object> SendMessage(int messageId, int receiverReg, int numArgs);

        Ref<Object> Self();
        
        Ref<Upvalue> CaptureUpvalue(int stackIndex);
        
#ifdef TRACE_INSTRUCTIONS
        void TraceInstruction(Instruction instruction);
        void TraceStack();
#endif
        
        bool mIsRunning;
        Interpreter & mInterpreter;
        Array<Ref<Object> >  mStack;
        Stack<CallFrame>     mCallFrames;
        Array<Ref<Upvalue> > mOpenUpvalues;

        NO_COPY(Fiber);
    };
}

