//
// Created by seeu on 2022/8/14.
//

#ifndef GOINTERPRETER_FRAME_H
#define GOINTERPRETER_FRAME_H

#include "CompilerObject.h"

#include <memory>
#include <utility>
#include <vector>

namespace GC {
    using namespace std;

    struct Frame {
        Frame(CompiledFunctionObject functionObject, int basePointer, int ip = -1) :
                functionObject(functionObject), ip(ip), basePointer(basePointer) {}

        CompiledFunctionObject functionObject;
        int ip;
        int basePointer;
    };

    class FrameManager {
    public:
        FrameManager(Instruction instructions) {
            auto mainFrame = Frame{
                    GC::CompiledFunctionObject(instructions, 0, 0),
                    0
            };
            frames.push_back(mainFrame);
        }

        Frame *currentFrame() {
            return &frames[frameIndex];
        }

        void framePush(Frame frame) {
            if (frameIndex > frames.size()) {
                throw "frameIndex in push action is greater than frames' size";
            }
            frameIndex++;
            if (frames.size() == frameIndex) {
                frames.push_back(frame);
            } else {
                frames[frameIndex] = frame;
            }
        }

        Frame framePop() {
            if (frameIndex < 0) {
                throw "frame index is negative";
            }
            auto frame = frames[frameIndex];
            frameIndex--;
            return frame;
        }

        Instruction getInstructions() {
            return currentFrame()->functionObject.instructions;
        }

        std::vector<Frame> frames{};
        int frameIndex{0};

    private:
    };
}


#endif //GOINTERPRETER_FRAME_H
