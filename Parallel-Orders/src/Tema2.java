import java.io.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Tema2 {
	public static String SHIPPED = ",shipped\n";
	
	public static String folderInput;
	public static int nrThreads;
	
	public static void main(String[] args) throws IOException {
		// Setting the command line arguments
		folderInput = args[0];
		nrThreads = Integer.parseInt(args[1]);
		
		// Creating my two executor services
		ExecutorService tpeOrders = Executors.newFixedThreadPool(nrThreads);
		AtomicInteger inQueueOrders = new AtomicInteger(0);
		
		ExecutorService tpeProducts = Executors.newFixedThreadPool(nrThreads);
		AtomicInteger inQueueProducts = new AtomicInteger(0);
		
		// Creating a buffered reader for the orders file
		FileReader inputFr = new FileReader(folderInput + "/orders.txt");
		BufferedReader ordersReader = new BufferedReader(inputFr);
		
		// Creating a buffered writer for the orders_out.txt file
		FileWriter outputFw = new FileWriter("orders_out.txt");
		BufferedWriter ordersWriter = new BufferedWriter(outputFw);
		
		// Creating a buffered writer for the order_products_out.txt file
		outputFw = new FileWriter("order_products_out.txt");
		BufferedWriter productsWriter = new BufferedWriter(outputFw);
		
		// Processing order layer
		for (int i = 0; i < nrThreads; i++) {
			inQueueOrders.incrementAndGet();
			tpeOrders.submit(new OdersRunnable(tpeOrders, tpeProducts,
				inQueueOrders, inQueueProducts,
				ordersReader, ordersWriter,
				productsWriter));
		}
	}
}
