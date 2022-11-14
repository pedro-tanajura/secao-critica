import java.util.concurrent.Semaphore;

public class TrafficController {
    private Semaphore sem = new Semaphore(1, true);
    
    public void enterLeft() {
        try {
            sem.acquire();
        } catch (InterruptedException e) {}
    }

    public void enterRight() {
        try {
            sem.acquire();
        } catch (InterruptedException e) {}
    }

    public void leaveLeft() {
        sem.release();
    }

    public void leaveRight() {
        sem.release();
    }

}