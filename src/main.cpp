#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "stdafx.h"
#include "steam/steam_api.h"
#include <pthread.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include "stdatomic.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/types.h>
#include <sys/syscall.h>
#include <sched.h>
#include <stdio.h>

#include "GameEngine.h"

#define MICROSEC_PER_MS (1000)

struct sched_attr {
    __u32 size;
    __u32 sched_policy;
    __u64 sched_flags;
    /* SCHED_NORMAL, SCHED_BATCH */
    __s32 sched_nice;
    /* SCHED_FIFO, SCHED_RR */
    __u32 sched_priority;
    /* SCHED_DEADLINE (nsec) */
    __u64 sched_runtime;
    __u64 sched_deadline;
    __u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr* attr,
                  unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

#define NANOSEC_PER_MS (1000000)

extern IGameEngine *CreateGameEngineSDL( );

void RunGameLoop(IGameEngine* engine) {
    if (!engine->BReadyForUse()) {
        return;
    }

    struct sched_attr Attr = {0};
    Attr.size = sizeof(Attr);
    Attr.sched_policy = SCHED_DEADLINE;
    Attr.sched_period = 20 * NANOSEC_PER_MS;
    Attr.sched_deadline = Attr.sched_period / 2;
    Attr.sched_runtime = (20 / 3) * NANOSEC_PER_MS;

    int res = sched_setattr(0, &Attr, 0);
    if (res != 0) {
        int err = errno;
        if (err == EPERM) {
            printf("Process needs to have cap_sys_nice set. sudo setcap 'cap_sys_nice=eip' path/to/proc\n");
            return;
        }
    }

    engine->SetBackgroundColor(0, 0x64, 0x95, 0xED); // Cornflower blue

    while (!engine->BShuttingDown()) {
        if (!engine->StartFrame()) {
            continue;
        }

        engine->UpdateGameTickCount();

        SteamAPI_RunCallbacks();

        // Run Engine
        engine->BDrawFilledRect(100, 100, 200, 200, D3DCOLOR_ARGB(255, 100, 0, 0));
        engine->BDrawFilledRect(250, 100, 300, 500, D3DCOLOR_ARGB(255, 100, 100, 0));

        //printf("%d %d\n", engine->GetViewportWidth(), engine->GetViewportHeight());


        printf("%llu\n", engine->GetGameTicksFrameDelta());

        engine->BFlushQuadBuffer();


        engine->EndFrame();

        sched_yield();
    }
}


int main(int argc, const char* argv[]) {
    if (!SteamAPI_Init()) {
        return 1;
    }

    auto engine = CreateGameEngineSDL();

    if (!SteamInput()->Init(false)) {
        OutputDebugString("SteamInput()->Init failed.\n");
        return EXIT_FAILURE;
    }

    RunGameLoop(engine);


    delete engine;

    SteamAPI_Shutdown();

    return EXIT_SUCCESS;
}