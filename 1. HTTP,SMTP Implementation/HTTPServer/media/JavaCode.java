import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;

public class HTTPServer {
    public static final int PORT = 8080;
    private static  int ID = 1;
    public static String DEFAULT = "index.html";
    public static String NotFOUND = "404.html";
    public static PrintWriter log;
    
    public static void main(String[] args) throws IOException {
        //creates the log file
        new PrintWriter(new FileWriter("log.txt")).close();
        
        log = new PrintWriter(new FileWriter("log.txt",true));
        
        try{
            //opens seversocket for listenning
            ServerSocket serverSocket = new ServerSocket(PORT);
            System.out.println(new Date() + "\r\nHTTPServer Starts listenning at PORT:: " + PORT + "\r\n");
            log.println(new Date() + "\r\nHTTPServer Starts listenning at PORT:: " + PORT + "\r\n");
            log.flush();

            while(true){
                Socket clientSocket = serverSocket.accept();
                HTTPClient client = new HTTPClient(clientSocket, ID);
                Thread t = new Thread(client);
                t.start();
                
                System.out.println(new Date() + "\r\nClient[" + ID + "] is now connected.\r\n");
                log.println(new Date() + "\r\nClient[" + ID + "] is now connected.\r\n");
                log.flush();
                
                ID++;
            }
            
        }catch(Exception e){
            System.out.println(e.getMessage());
            System.out.println(new Date() + "\r\nHTTPServer Failed to Start! Exiting!\r\n");
            
            log.println(new Date() + "\r\nHTTPServer Failed to Start! Exiting!\r\n");
            log.flush();
            log.close();
        }
    }    
}
