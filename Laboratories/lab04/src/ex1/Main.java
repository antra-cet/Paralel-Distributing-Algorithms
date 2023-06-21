package ex1;

public class Main {

	public static void main(String[] args) {
		int NUM_THREADS = Runtime.getRuntime().availableProcessors();
		Thread[] t = new Thread[NUM_THREADS];

		for (int i = 0; i < NUM_THREADS; i++) {
			t[i] = new MyThread(i);
			t[i].start();
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			try {
				t[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}
}
