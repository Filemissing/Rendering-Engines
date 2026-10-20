//
// GPU timer built on GL_TIMESTAMP query pairs. Unlike GL_TIME_ELAPSED these may be issued while another
// GL_TIME_ELAPSED query is active, so they can wrap code that runs its own timing (Renderer, Raymarcher::Bake).
//

#ifndef RAWENGINE_GPUTIMER_H
#define RAWENGINE_GPUTIMER_H

#include <string>
#include <vector>
#include <glad/glad.h>

namespace core {
    class GpuTimer {
        struct Slot {
            GLuint queries[2] = {0, 0};
            bool pending = false;
            std::string tag;
        };
        std::vector<Slot> slots;
        int next = 0;

        static double ReadMs(const Slot& slot) {
            GLuint64 t0 = 0, t1 = 0;
            glGetQueryObjectui64v(slot.queries[0], GL_QUERY_RESULT, &t0);
            glGetQueryObjectui64v(slot.queries[1], GL_QUERY_RESULT, &t1);
            return static_cast<double>(t1 - t0) * 1e-6;
        }

    public:
        explicit GpuTimer(int ringSize = 8) : slots(ringSize) {}
        ~GpuTimer() {
            for (auto& slot : slots)
                if (slot.queries[0]) glDeleteQueries(2, slot.queries);
        }

        // returns the slot index to pass to End, or -1 if every slot is still waiting on the GPU (sample dropped)
        int Begin(const std::string& tag) {
            for (int tries = 0; tries < static_cast<int>(slots.size()); ++tries) {
                int i = next;
                next = (next + 1) % static_cast<int>(slots.size());
                Slot& slot = slots[i];
                if (slot.pending) continue;
                if (!slot.queries[0]) glGenQueries(2, slot.queries);
                slot.tag = tag;
                glQueryCounter(slot.queries[0], GL_TIMESTAMP);
                return i;
            }
            return -1;
        }
        void End(int index) {
            if (index < 0) return;
            glQueryCounter(slots[index].queries[1], GL_TIMESTAMP);
            slots[index].pending = true;
        }

        // blocks until the measurement at index is done
        double Wait(int index) {
            if (index < 0) return -1.0;
            Slot& slot = slots[index];
            double ms = ReadMs(slot);
            slot.pending = false;
            return ms;
        }

        // hands every finished measurement to callback(tag, ms); block = true waits for all outstanding ones
        template <typename F>
        void Collect(F&& callback, bool block = false) {
            for (auto& slot : slots) {
                if (!slot.pending) continue;
                if (!block) {
                    GLint available = 0;
                    glGetQueryObjectiv(slot.queries[1], GL_QUERY_RESULT_AVAILABLE, &available);
                    if (!available) continue;
                }
                callback(slot.tag, ReadMs(slot));
                slot.pending = false;
            }
        }
    };
} // core

#endif //RAWENGINE_GPUTIMER_H
