import java.util.concurrent.Semaphore;

public class Testapd {
	public static void main(String[] args) {
		// Reading N, P and the vector of N elements from the args list
		int N = Integer.parseInt(args[0]);
		int P = Integer.parseInt(args[1]);
		int[] v = new int[N];
		for (int i = 0; i < N; i++) {
			v[i] = Integer.parseInt(args[i + 2]);
		}
		
		// Creating the threads
		Thread[] threads = new Thread[P];
		// Creating the semaphore
		Semaphore semaphore = new Semaphore(P);
		
		// Create the threads a start them
		for (int i = 0; i < P; i++) {
			// Calculate start and end for each thread from the v vector
			int start = (int) (i * (double)N / P);
			int end = Integer.min((int) ((i + 1) * (double)N / P), N);
			
			// Start the thread
			threads[i] = new Thread(new Prime(v, P, semaphore, start, end, i));
			threads[i].start();
		}
		
		// Join the threads
		for (int i = 0; i < P; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}

// Creating the Prime class
class Prime implements Runnable {
	private int[] v;
	private int P;
	private Semaphore semaphore;
	private int start, end, id;
	
	public Prime(int[] v, int P, Semaphore semaphore, int start, int end, int id) {
		this.v = v;
		this.P = P;
		this.semaphore = semaphore;
		this.start = start;
		this.end = end;
		this.id = id;
	}
	
	@Override
	public void run() {
		try {
			// When entering a thread, acquire the semaphore
			semaphore.acquire();
			
			// For the elements in the vector, set them 1 if prime and 0 if not
			for (int i = start; i < end; i++) {
				if (isPrime(v[i])) {
					v[i] = 1;
				} else {
					v[i] = 0;
				}
			}
			
			// When exiting a thread, release the semaphore
			semaphore.release();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		// If the thread is the last one (number P - 1), then print the cnt and sum
		// Wait for the other threads to finish before printing
		if (id == P - 1) {
			try {
				semaphore.acquire();
				int cnt = 0, sum = 0;
				for (int j = 0; j < v.length; j++) {
					if (v[j] == 1) {
						cnt++;
						sum += j;
					}
				}
				System.out.println(cnt + " " + sum);
				semaphore.release();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	// Method to verify if a number is prime or not
	private boolean isPrime(int n) {
		if (n <= 1) {
			return false;
		}
		
		for (int i = 2; i < n; i++) {
			if (n % i == 0) {
				return false;
			}
		}
		
		return true;
	}
}