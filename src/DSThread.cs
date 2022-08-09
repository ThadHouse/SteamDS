using System.Threading;

namespace SteamDS;

class DSThread {
    private Thread commsThread;
    private CancellationTokenSource cts;

    public DSThread() {
        cts = new CancellationTokenSource();
    }

    private void ThreadMain() {
        while (cts.IsCancellationRequested) {

        }
    }
}
