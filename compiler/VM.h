//
// Created by seeu on 2022/8/9.
//

#ifndef GOINTERPRETER_VM_H
#define GOINTERPRETER_VM_H

#include <vector>
#include "GIObject.h"
#include "Code.h"
#include "Compiler.h"
#include "Frame.h"

#define STACK_SIZE 1024
#define GLOBALS_SIZE 65536

namespace GC {
    using namespace std;

    class VMException : public std::runtime_error {
    public:
        explicit VMException(const string &msg) : std::runtime_error(msg) {}
    };

    class VM {
    public:
        explicit VM(ByteCode byteCode) : constants{byteCode.constants}, frameManager{byteCode.instructions} {
            stack.reserve(STACK_SIZE);
            globals.reserve(GLOBALS_SIZE);
        }

        void run();

        shared_ptr<Common::GIObject> lastStackElem();

    private:
        Frame *currentFrame() {
            return frameManager.currentFrame();
        }

        Instruction getInstructions() {
            return frameManager.getInstructions();
        }

        void stackPush(const shared_ptr<Common::GIObject> &object);

        shared_ptr<Common::GIObject> stackPop();

        void closurePush(int constIndex, int numFree);

        int readUint16(int index);

        int readUint8(int index);

        std::vector<shared_ptr<Common::GIObject>> constants;

        std::vector<shared_ptr<Common::GIObject>> stack{};
        int sp{0};
        Code code{};

        FrameManager frameManager;

        std::map<int, string> builtinIndexMap{{0, "len"}};

        std::vector<shared_ptr<Common::GIObject>> globals;
    };
}


#endif //GOINTERPRETER_VM_H
