import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class ProductsRunnable implements Runnable{
	private ExecutorService tpeProducts;
	private AtomicInteger inQueueProducts;
	
	private int productIndex;
	private String orderId;
	
	private BufferedReader productsReader;
	private BufferedWriter productsWriter;
	
	private Semaphore productsSemaphore;
	
	public ProductsRunnable(ExecutorService tpeProducts, AtomicInteger inQueueProducts,
	                        int productIndex, String orderId,
	                        BufferedReader productsReader, BufferedWriter productsWriter,
	                        Semaphore productsSemaphore) {
		this.tpeProducts = tpeProducts;
		this.inQueueProducts = inQueueProducts;
		this.productIndex = productIndex;
		this.orderId = orderId;
		this.productsReader = productsReader;
		this.productsWriter = productsWriter;
		this.productsSemaphore = productsSemaphore;
	}
	
	@Override
	public void run() {
		try {
			String line = productsReader.readLine();
			int index = 0;
			
			while (line != null) {
				// If the line read isn't null
				// Then split the line after comma
				String[] splitLine = line.split(",");
				
				// Retrieving from each line the order id and the
				// id of the products
				String orderId = splitLine[0];
				String productId = splitLine[1];
				
				if (orderId.equals(this.orderId)) {
					if (index == this.productIndex) {
						// Writing in the output products file the
						// product found as shipped
						productsWriter.write(line + Tema2.SHIPPED);
						
						break;
					}
					
					index++;
				}
				
				line = productsReader.readLine();
			}
			
			// After reading the necessary lines, close the reader
			productsReader.close();
			
			// After each item found release the semaphore
			productsSemaphore.release();
			
			int left = inQueueProducts.decrementAndGet();
			if (left == 0) {
				tpeProducts.shutdown();
				productsWriter.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
