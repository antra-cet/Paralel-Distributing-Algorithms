import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class OdersRunnable implements Runnable{
	private ExecutorService tpeOrders;
	private ExecutorService tpeProducts;
	
	private AtomicInteger inQueueOrders;
	private AtomicInteger inQueueProducts;
	
	private BufferedReader ordersReader;
	private BufferedWriter ordersWriter;
	
	private BufferedWriter productsWriter;
	
	public OdersRunnable(ExecutorService tpeOrders, ExecutorService tpeProducts,
	                     AtomicInteger inQueueOrders, AtomicInteger inQueueProducts,
	                     BufferedReader ordersReader,
	                     BufferedWriter ordersWriter, BufferedWriter productsWriter) {
		this.tpeOrders = tpeOrders;
		this.tpeProducts = tpeProducts;
		this.inQueueOrders = inQueueOrders;
		this.inQueueProducts = inQueueProducts;
		this.ordersReader = ordersReader;
		this.ordersWriter = ordersWriter;
		this.productsWriter = productsWriter;
	}
	
	@Override
	public void run() {
		try {
			String line = ordersReader.readLine();
			
			if (line != null) {
				// If the line read isn't null
				// Then split the line after comma
				String[] splitLine = line.split(",");
				
				// Retrieving from each line the order id and
				// the number of products
				String orderId = splitLine[0];
				int nrProducts = Integer.parseInt(splitLine[1]);
				
				// Creating a semaphore so that the products' writer
				// closes after all the files have finished writing
				Semaphore productsSemaphore = new Semaphore((-1) * (nrProducts - 1));
				
				// For the number of products start a ProductsRunnable
				// to mark all the products as shipped
				for (int i = 0 ; i < nrProducts; i++) {
					// Create a buffered reader for the order_products.txt
					FileReader inputFr = new FileReader(Tema2.folderInput + "/order_products.txt");
					BufferedReader productsReader = new BufferedReader(inputFr);
					
					inQueueProducts.incrementAndGet();
					tpeProducts.submit(new ProductsRunnable(tpeProducts, inQueueProducts,
						i, orderId,
						productsReader, productsWriter,
						productsSemaphore));
				}
				
				// Acquire the semaphore
				productsSemaphore.acquire();
				
				// Writing in the orders output file the shipped products
				// But if the number of products is 0, then the order
				// isn't shipped
				if (nrProducts > 0){
					ordersWriter.write(line + Tema2.SHIPPED);
				}
				
				// Process the next order
				inQueueOrders.incrementAndGet();
				tpeOrders.submit(new OdersRunnable(tpeOrders, tpeProducts,
					inQueueOrders, inQueueProducts,
					ordersReader, ordersWriter,
					productsWriter
					));
			}
			
			int left = inQueueOrders.decrementAndGet();
			if (left == 0) {
				tpeOrders.shutdown();
				ordersReader.close();
				ordersWriter.close();
			}
		} catch (IOException | InterruptedException e) {
			e.printStackTrace();
		}
	}
}
