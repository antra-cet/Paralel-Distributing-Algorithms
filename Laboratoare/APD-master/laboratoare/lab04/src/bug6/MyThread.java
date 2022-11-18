package bug6;

public class MyThread extends Thread {
    @Override
    public void run() {
		// synchronizes class
        synchronized (Singleton.class) {
            Singleton.getInstance();
        }
    }
}
