#include "CodeBlock.h"

#ifdef DEBUG
#include "Environment.h"
#endif

namespace Finch
{
    BlockExemplar::BlockExemplar(int methodId, const Array<String> & params)
    :   mMethodId(methodId),
        mParams(params),
        mCode(),
        mConstants(),
        mNumRegisters(0)
    {
    }

    int BlockExemplar::AddConstant(Ref<Object> object)
    {
        // TODO(bob): Unify duplicates.
        mConstants.Add(object);
        return mConstants.Count() - 1;
    }
    
    int BlockExemplar::AddExemplar(Ref<BlockExemplar> exemplar)
    {
        mExemplars.Add(exemplar);
        return mExemplars.Count() - 1;
    }
    
    // Writes an instruction.
    void BlockExemplar::Write(OpCode op, int a, int b, int c)
    {
        ASSERT_RANGE(a, 256);
        ASSERT_RANGE(b, 256);
        ASSERT_RANGE(c, 256);
        
        Instruction instruction = (op << 24) |
                                  ((a & 0xff) << 16) |
                                  ((b & 0xff) << 8) |
                                  (c & 0xff);
        
        mCode.Add(instruction);
    }

#ifdef DEBUG
    void BlockExemplar::DumpInstruction(Environment & environment, const String & prefix, Instruction instruction)
    {
        using namespace std;
        
        cout << prefix;
        
        OpCode op = static_cast<OpCode>((instruction & 0xff000000) >> 24);
        int a = (instruction & 0x00ff0000) >> 16;
        int b = (instruction & 0x0000ff00) >> 8;
        int c = instruction & 0x000000ff;
        switch (op)
        {
            case OP_CONSTANT:
                cout << "CONSTANT     " << a << " (" << mConstants[a] << ") -> " << b;
                break;
            case OP_BLOCK:
                cout << "BLOCK        " << a << " -> " << b;
                break;
            case OP_OBJECT:
                cout << "OBJECT       " << a;
                break;
            case OP_ARRAY:
                cout << "ARRAY        [" << a << "] -> " << b;
                break;
            case OP_ARRAY_ELEMENT:
                cout << "ARRAY_ELEM   " << a << " -> " << b;
                break;
            case OP_MOVE:
                cout << "MOVE         " << a << " -> " << b;
                break;
            case OP_SELF:
                cout << "SELF         -> " << a;
                break;
            case OP_MESSAGE_0:
            case OP_MESSAGE_1:
            case OP_MESSAGE_2:
            case OP_MESSAGE_3:
            case OP_MESSAGE_4:
            case OP_MESSAGE_5:
            case OP_MESSAGE_6:
            case OP_MESSAGE_7:
            case OP_MESSAGE_8:
            case OP_MESSAGE_9:
            case OP_MESSAGE_10:
                cout << "MESSAGE_" << (op - OP_MESSAGE_0) << "   '" << environment.Strings().Find(a) << "' " << b << " -> " << c;
                break;
            case OP_GET_UPVALUE:
                cout << "GET_UPVALUE  " << a << " -> " << b;
                break;
            case OP_SET_UPVALUE:
                cout << "SET_UPVALUE  " << a << " -> " << b;
                break;
            case OP_GET_FIELD:
                cout << "GET_FIELD    '" << environment.Strings().Find(a) << "' -> " << b;
                break;
            case OP_SET_FIELD:
                cout << "SET_FIELD    '" << environment.Strings().Find(a) << "' <- " << b;
                break;
            case OP_GET_GLOBAL:
                cout << "GET_GLOBAL   " << a << " -> " << b;
                break;
            case OP_SET_GLOBAL:
                cout << "SET_GLOBAL   " << a << " <- " << b;
                break;
            case OP_DEF_METHOD:
                cout << "DEF_METHOD   '" << environment.Strings().Find(a) << "' " << b << " -> " << c;
                break;
            case OP_DEF_FIELD:
                cout << "DEF_FIELD    '" << environment.Strings().Find(a) << "' " << b << " -> " << c;
                break;
            case OP_END:
                cout << "END          " << a;
                break;
            case OP_RETURN:
                cout << "RETURN       m" << a << " ^ " << b;
                break;
            case OP_CAPTURE_LOCAL:   // A = register of local
                cout << "CAP_LOCAL    " << a;
                break;
            case OP_CAPTURE_UPVALUE:
                cout << "CAP_UPVALUE  " << a;
                break;
            default:
                cout << "UNKNOWN OP " << op;
        }
        cout << endl;
        
        // Dump the child block too.
        if (op == OP_BLOCK)
        {
            mExemplars[a]->DebugDump(environment, prefix + "  ");
        }
    }
    
    void BlockExemplar::DebugDump(Environment & environment, const String & prefix)
    {
        for (int i = 0; i < mCode.Count(); i++)
        {
            DumpInstruction(environment, prefix, mCode[i]);
        }
    }
#endif
    
    /*    
    void CodeBlock::MarkTailCalls()
    {
        for (int i = 1; i < mInstructions.Count(); i++)
        {
            if ((mInstructions[i].op == OP_END_BLOCK) &&
                (mInstructions[i - 1].op >= OP_MESSAGE_0) &&
                (mInstructions[i - 1].op <= OP_MESSAGE_10))
            {
                int numArgs = mInstructions[i - 1].op - OP_MESSAGE_0;
                mInstructions[i - 1].op =
                    static_cast<OpCode>(OP_TAIL_MESSAGE_0 + numArgs);
            }
        }
    }
     */
}

